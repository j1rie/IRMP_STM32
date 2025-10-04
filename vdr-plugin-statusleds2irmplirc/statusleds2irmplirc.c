/*
 * StatusLeds2irmplirc.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/i18n.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>
#include <vdr/videodir.h>
#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <vdr/status.h>
#include <vdr/osd.h>
#include <ctype.h>
#include <sys/kd.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <errno.h>
#include <fcntl.h>

extern char **environ;

static const char *VERSION        = "0.4";
static const char *DESCRIPTION    = tr("show vdr status on irmplirc device");

enum access {
	ACC_GET,
	ACC_SET,
	ACC_RESET
};

enum command {
	CMD_EMIT,
	CMD_CAPS,
	CMD_HID_TEST,
	CMD_ALARM,
	CMD_MACRO,
	CMD_WAKE,
	CMD_REBOOT,
	CMD_EEPROM_RESET,
	CMD_EEPROM_COMMIT,
	CMD_EEPROM_GET_RAW,
	CMD_STATUSLED,
	CMD_NEOPIXEL,
};

enum status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

enum report_id {
	REPORT_ID_IR = 1,
	REPORT_ID_CONFIG_IN = 2,
	REPORT_ID_CONFIG_OUT = 3
};

static int stm32fd = -1;
uint8_t inBuf[64];
uint8_t outBuf[64];
volatile bool stop = false;

static bool open_stm32(const char *devicename) {
	stm32fd = open(devicename, O_RDWR);
	if (stm32fd == -1) {
		dsyslog("statusleds2irmplirc: error opening stm32 device: %s\n",strerror(errno));
		printf("error opening stm32 device: %s\n",strerror(errno));
		return false;
	}
	//printf("opened stm32 device\n");
	return true;
}

static void read_stm32() {
	int retVal;
	retVal = read(stm32fd, inBuf, sizeof(inBuf));
	if (retVal < 0) {
		dsyslog("statusleds2irmplirc: read error\n");
		printf("read error\n");
        } /*else {
                printf("read %d bytes:\n\t", retVal);
                for (int i = 0; i < retVal; i++)
                        printf("%02hhx ", inBuf[i]);
                puts("\n");
        }*/
} 

static void write_stm32() {
	int retVal;
	retVal = write(stm32fd, outBuf, sizeof(outBuf));
	if (retVal < 0) {
		dsyslog("statusleds2irmplirc: write error\n");
		printf("write error\n");
        } /*else {
                printf("written %d bytes:\n\t", retVal);
                for (int i = 0; i < retVal; i++)
                        printf("%02hhx ", outBuf[i]);
                puts("\n");
        }*/
}

static void send_report(uint8_t led_state, const char *device) {
	if (stop)
	    return;
	open_stm32(device != NULL ? device : "/dev/irmp_stm32");
        outBuf[0] = REPORT_ID_CONFIG_OUT;
	outBuf[1] = STAT_CMD;

	    outBuf[2] = ACC_SET;
	    outBuf[3] = CMD_STATUSLED;
	    outBuf[4] = led_state;
	    write_stm32();
	    usleep(3000);
	    read_stm32();
	    while (inBuf[0] == REPORT_ID_IR)
		read_stm32();
	
	if (stm32fd >= 0) close(stm32fd);
}

class cStatusUpdate : public cThread, public cStatus {
public:
    cStatusUpdate();
    ~cStatusUpdate();
#if VDRVERSNUM >= 10338
    virtual void Recording(const cDevice *Device, const char *Name, const char *FileName, bool On);
#else
    virtual void Recording(const cDevice *Device, const char *Name);
#endif
protected:
    virtual void Action(void);
};

class cRecordingPresignal : public cThread {
protected:
    virtual void Action(void);
};

// Global variables that control the overall behaviour:
int iOnDuration = 1;
int iOffDuration = 10;
int iOnPauseDuration = 5; 
bool bPerRecordBlinking = false;
int iRecordings = 0;
int iPrewarnBeeps = 3;
int iPrewarnBeepPause = 500;
bool bPrewarnBeep = false;
int iPrewarnBeepTime = 120;
int iPrewarnBeepOnDuration = 1;

const char * irmplirc_device = NULL;

//cStatusUpdate * oStatusUpdate = NULL;
//cRecordingPresignal * oRecordingPresignal = NULL;

class cPluginStatusLeds2irmplirc : public cPlugin {
private:
  cStatusUpdate *oStatusUpdate;
  cRecordingPresignal *oRecordingPresignal;
public:
  cPluginStatusLeds2irmplirc(void);
  virtual ~cPluginStatusLeds2irmplirc() override;
  virtual const char *Version(void) override { return VERSION; }
  virtual const char *Description(void) override { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void) override;
  virtual bool ProcessArgs(int argc, char *argv[]) override;
  virtual bool Start(void) override;
  virtual void Stop(void) override;
  virtual const char *MainMenuEntry(void) override { return NULL; }
  virtual cMenuSetupPage *SetupMenu(void) override;
  virtual bool SetupParse(const char *Name, const char *Value) override;
  };

// --- cMenuSetupStatusLeds2irmplirc -------------------------------------------------------

class cMenuSetupStatusLeds2irmplirc : public cMenuSetupPage {
private:
  int iNewOnDuration;
  int iNewOffDuration;
  int iNewOnPauseDuration;
  int bNewPerRecordBlinking;
  int iNewPrewarnBeeps;
  int iNewPrewarnBeepPause;
  int bNewPrewarnBeep;
  int iNewPrewarnBeepTime;
  int iNewPrewarnBeepOnDuration;
protected:
  virtual void Store(void);
  void Set(void);
  void Save();
  eOSState ProcessKey(eKeys Key);
public:
  cMenuSetupStatusLeds2irmplirc(void);
  };

cMenuSetupStatusLeds2irmplirc::cMenuSetupStatusLeds2irmplirc(void)
{
  iNewOnDuration = iOnDuration;
  iNewOffDuration = iOffDuration;
  iNewOnPauseDuration = iOnPauseDuration;
  bNewPerRecordBlinking = bPerRecordBlinking;
  iNewPrewarnBeeps = iPrewarnBeeps;
  iNewPrewarnBeepPause = iPrewarnBeepPause;
  bNewPrewarnBeep = bPrewarnBeep;
  iNewPrewarnBeepTime = iPrewarnBeepTime;
  iNewPrewarnBeepOnDuration = iPrewarnBeepOnDuration;

  Set();
}

void cMenuSetupStatusLeds2irmplirc::Set(void)
{
  int current = Current();
  Clear();

  Add(new cMenuEditBoolItem( tr("Setup.StatusLeds2irmplirc$Prewarn beep"), &bNewPrewarnBeep));
  if (bNewPrewarnBeep)
  {
    Add(new cMenuEditIntItem(  tr("Setup.StatusLeds2irmplirc$Prewarn time (s)"), &iNewPrewarnBeepTime, 1, 32768));
    Add(new cMenuEditIntItem(  tr("Setup.StatusLeds2irmplirc$Prewarn Beeps"), &iNewPrewarnBeeps, 1, 100));
    Add(new cMenuEditIntItem(  tr("Setup.StatusLeds2irmplirc$Prewarn Pause (100ms)"), &iNewPrewarnBeepPause));
    Add(new cMenuEditIntItem(  tr("Setup.StatusLeds2irmplirc$Prewarn On time (100ms)"), &iNewPrewarnBeepOnDuration));
  }

  Add(new cMenuEditBoolItem( tr("Setup.StatusLeds2irmplirc$One blink per recording"), &bNewPerRecordBlinking));

  // Add ioctl() options
  Add(new cMenuEditIntItem( tr("Setup.StatusLeds2irmplirc$On time (100ms)"), &iNewOnDuration, 1, 99));
  Add(new cMenuEditIntItem( tr("Setup.StatusLeds2irmplirc$On pause time (100ms)"), &iNewOnPauseDuration, 1, 99));
  Add(new cMenuEditIntItem( tr("Setup.StatusLeds2irmplirc$Off time (100ms)"), &iNewOffDuration, 1, 99));

  SetCurrent(Get(current));
}

eOSState cMenuSetupStatusLeds2irmplirc::ProcessKey(eKeys Key)
{
  bool bOldPrewarnBeep = bNewPrewarnBeep;

  eOSState state = cMenuSetupPage::ProcessKey(Key);
  if (bOldPrewarnBeep != bNewPrewarnBeep)
  {
    Set();
    Display();
  }

  return state;
}

void cMenuSetupStatusLeds2irmplirc::Save(void)
{
  iOnDuration = iNewOnDuration;
  iOffDuration = iNewOffDuration;
  iOnPauseDuration = iNewOnPauseDuration;
  bPerRecordBlinking = bNewPerRecordBlinking;
  bPrewarnBeep = bNewPrewarnBeep;
  iPrewarnBeeps = iNewPrewarnBeeps;
  iPrewarnBeepTime = iNewPrewarnBeepTime;
  iPrewarnBeepPause = iNewPrewarnBeepPause;
  iPrewarnBeepOnDuration = iNewPrewarnBeepOnDuration;
}

void cMenuSetupStatusLeds2irmplirc::Store(void)
{
  Save();

  SetupStore("OnDuration", iOnDuration);
  SetupStore("OffDuration", iOffDuration);
  SetupStore("OnPauseDuration", iOnPauseDuration);
  SetupStore("PerRecordBlinking", bPerRecordBlinking);
  SetupStore("PrewarnBeep", bPrewarnBeep);
  SetupStore("PrewarnBeeps", iPrewarnBeeps);
  SetupStore("PrewarnBeepPause", iPrewarnBeepPause);
  SetupStore("PrewarnBeepTime", iPrewarnBeepTime);
  SetupStore("PrewarnBeepOnDuration", iPrewarnBeepOnDuration);
}

// --- cPluginStatusLeds2irmplirc ----------------------------------------------------------

cPluginStatusLeds2irmplirc::cPluginStatusLeds2irmplirc(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  oStatusUpdate = NULL;
  oRecordingPresignal = NULL;
}

cPluginStatusLeds2irmplirc::~cPluginStatusLeds2irmplirc()
{
  // Clean up after yourself!
    delete oStatusUpdate;
    delete oRecordingPresignal;
}

const char *cPluginStatusLeds2irmplirc::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return

"  -p, --perrecordblinking                    LED blinks one times per recording\n"
"  -d [on[,off[,pause]]],                     LED blinking timing\n"
"     --duration[=On-Time[,Off-Time[,On-Pause-Time]]]\n"
"  -w [time,beeps,pause,on],                     warn before recording\n"
"     --prewarn[=Time,Beeps,Pause,on]\n"
"  -i irmplirc_device, --irmplirc_device=irmplirc_device  irmplirc_device\n"
;
}

bool cPluginStatusLeds2irmplirc::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  static struct option long_options[] = {
       { "duration",		optional_argument,	NULL, 'd' },
       { "perrecordblinking",	no_argument,		NULL, 'p' },
       { "prewarn",		required_argument,	NULL, 'w' },
       { "irmplirc_device",	optional_argument,	NULL, 'i' },
       { NULL,			no_argument,		NULL, 0 }
     };

  int c;
  while ((c = getopt_long(argc, argv, "d:pw:i", long_options, NULL)) != -1) {
        switch (c) {
          case 'd':
            //iOnDuration = 1;
            //iOffDuration = 10;
            //iOnPauseDuration = 5;
            if (optarg && *optarg)
              sscanf(optarg, "%d,%d,%d", &iOnDuration, &iOffDuration, &iOnPauseDuration);
            break;
          case 'p': 
            bPerRecordBlinking = true;
            break;
          case 'w':
            bPrewarnBeep = true;
            if (optarg && *optarg)
              sscanf(optarg, "%d,%d,%d,%d", &iPrewarnBeepTime, &iPrewarnBeeps, &iPrewarnBeepPause,&iPrewarnBeepOnDuration);
            break;
          case 'i':
            irmplirc_device = optarg;
            break;
          default:
            return false;
          }
        }
  return true;
}

cStatusUpdate::cStatusUpdate()
{
}

cStatusUpdate::~cStatusUpdate()
{
}

void cStatusUpdate::Action(void)
{
    dsyslog("statusleds2irmplirc: Thread started (pid=%d)", getpid());

    bool blinking = false;
    // turn the LED's on at start of VDR
    send_report(1 ,irmplirc_device);
    dsyslog("statusleds2irmplirc: turned LED on at start");

    while(Running()) {
        if (iRecordings > 0) {
          //  let the LED's blink, if there's a recording
          if(!blinking) {
            blinking = true;
          }
          for(int i = 0; i < (bPerRecordBlinking ? iRecordings : 1) && Running(); i++) {
            if (!stop) send_report(1 ,irmplirc_device);
            usleep(iOnDuration * 100000);
            send_report(0 ,irmplirc_device);
            usleep(iOnPauseDuration * 100000);
          }
          usleep(iOffDuration * 100000);
        } else {
          //  turn the LED's on, if there's no recording
          if(blinking) {
            if (!stop) send_report(1 ,irmplirc_device);
            blinking = false;
          }
          sleep(1);
        }
    }
}

bool cPluginStatusLeds2irmplirc::Start(void)
{
    // Start any background activities the plugin shall perform.
    oStatusUpdate = new cStatusUpdate;
    oStatusUpdate->Start();

    oRecordingPresignal = new cRecordingPresignal;
    oRecordingPresignal->Start();

    return true;
}

void cPluginStatusLeds2irmplirc::Stop(void)
{
  // turn the LED's off, when VDR stops
  send_report(0 ,irmplirc_device);
  stop = true;
  dsyslog("statusleds2irmplirc: stopped (pid=%d)", getpid());
}

cMenuSetupPage *cPluginStatusLeds2irmplirc::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupStatusLeds2irmplirc;
}

bool cPluginStatusLeds2irmplirc::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.

  if (!strcasecmp(Name, "OnDuration"))
  {
    iOnDuration = atoi(Value);
    if (iOnDuration < 0 || iOnDuration > 99)
      iOnDuration = 1;
  }
  else if (!strcasecmp(Name, "OffDuration")) 
  {
    iOffDuration = atoi(Value);
    if (iOffDuration < 0 || iOffDuration > 99)
      iOffDuration = 10;
  }
  else if (!strcasecmp(Name, "OnPauseDuration"))
  {
    iOnPauseDuration = atoi(Value);
    if (iOnPauseDuration < 0 || iOnPauseDuration > 99)
      iOnPauseDuration = 5;
  }
  else if (!strcasecmp(Name, "PerRecordBlinking"))
  {
    bPerRecordBlinking = atoi(Value);
  }
  else if (!strcasecmp(Name, "PrewarnBeep"))
  {
    bPrewarnBeep = atoi(Value);
  }
  else if (!strcasecmp(Name, "PrewarnBeeps"))
  {
    iPrewarnBeeps = atoi(Value);
  }
  else if (!strcasecmp(Name, "PrewarnBeepTime"))
  {
    iPrewarnBeepTime = atoi(Value);
  }
  else if (!strcasecmp(Name, "PrewarnBeepPause"))
  {
    iPrewarnBeepPause = atoi(Value);
  }
  else if (!strcasecmp(Name, "PrewarnBeepOnDuration"))
  {
    iPrewarnBeepOnDuration = atoi(Value);
    if (iPrewarnBeepOnDuration < 0 || iPrewarnBeepOnDuration > 99)
      iPrewarnBeepOnDuration = 1;
  }
  else
    return false;

  return true;
}

#if VDRVERSNUM >= 10338
void cStatusUpdate::Recording(const cDevice *Device, const char *Name, const char *FileName, bool On)
{
  if (On)
#else
void cStatusUpdate::Recording(const cDevice *Device, const char *Name)
{
  if (Name)
#endif
    iRecordings++;
  else
    iRecordings--;
}

void cRecordingPresignal::Action(void)
{
  dsyslog("Status LED's: Presignal-Thread started (pid=%d)", getpid());

  time_t LastTime = 0;

  // Observe the timer list
  while(Running()) {
    // get next timer
    {
      LOCK_TIMERS_READ;
      const cTimer * NextTimer = Timers->GetNextActiveTimer();

      if (NextTimer) {
        time_t StartTime = NextTimer->StartTime();

        if (LastTime != StartTime) {
          // get warn time
          time_t Now = time(NULL);

          // Start signalisation?
          if (StartTime - iPrewarnBeepTime < Now) {
            if (bPrewarnBeep) {
              for(int i = 0; i < iPrewarnBeeps; i++) {
                if (!stop) send_report(1 ,irmplirc_device);
                usleep(iPrewarnBeepOnDuration * 100000);
                send_report(0 ,irmplirc_device);
                usleep(iPrewarnBeepPause * 100000);
              }
              if (!stop) send_report(1 ,irmplirc_device);
            }

            // remember last signaled time
            LastTime = StartTime;
          }
        }
      }
    }

    sleep(1);
  }
}

VDRPLUGINCREATOR(cPluginStatusLeds2irmplirc); // Don't touch this!

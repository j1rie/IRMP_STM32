/*
 * irmplirc.c: An IRMP HID plugin for the Video Disk Recorder
 *
 * Copyright (C) 20025-2025 Joerg Riechardt <J.Riechardt@gmx.de>
 *
 */

#include <vdr/plugin.h>
#include <linux/input.h>
#include <vdr/i18n.h>
#include <vdr/remote.h>
#include <vdr/thread.h>
#include "protocols.h"
#include <locale.h>

static const char *VERSION        = "0.0.6";
static const char *DESCRIPTION    = tr("Send keypresses from IRMP HID-Device to VDR");

#define DEBUG 1
#define RECONNECTDELAY 3000 // ms
#define REPORT_ID_IR 1

const char* irmp_device = "/dev/irmp_stm32";
int fd;
uint8_t buf[64];

class cIrmpRemote : public cRemote, private cThread {
private:
  void Action(void);
  bool Ready();
  cMutex mutex;
  cCondVar keyReceived;
public:
  cIrmpRemote(const char *Name);
  ~cIrmpRemote();
  void Receive() {
    cMutexLock MutexLock(&mutex);
    keyReceived.Broadcast();
  }
};

cIrmpRemote::cIrmpRemote(const char *Name)
:cRemote(Name)
,cThread("IRMP_LIRC remote control")
{
  Start();
}

cIrmpRemote::~cIrmpRemote()
{
  Cancel(3);
}

bool cIrmpRemote::Ready(void)
{
  return fd >= 0;
}

void cIrmpRemote::Action(void)
{
  cTimeMs FirstTime;
  cTimeMs LastTime;
  cTimeMs ThisTime;
  cString magic_key = "ff0000000000";
  uint8_t only_once = 1;
  bool release_needed = false;
  bool repeat = false;
  int timeout = INT_MAX;
  cString key = "";
  cString lastkey = "";
  uint8_t protocol = 0, lastprotocol = 0, count = 0;
  uint8_t min_delta = 255;
  uint8_t delta =255;

  if(DEBUG) printf("IrmpRemote action!\n");

  while(Running()){

    cMutexLock MutexLock(&mutex);
    if (keyReceived.TimedWait(mutex, timeout)) { // keypress

	key = cString::sprintf("%02hhx%02hhx%02hhx%02hhx%02hhx00", buf[1],buf[3],buf[2],buf[5],buf[4]);

	if(only_once && strcmp(key, magic_key) == 0) {
	    FILE *out = fopen("/var/log/started_by_IRMP_STM32", "a");
	    setlocale(LC_TIME, "de_DE.UTF-8");
	    time_t date = time(NULL);
	    struct tm *ts = localtime(&date);
	    char outstr[30];
	    strftime(outstr, sizeof(outstr), "%a %e. %b %H:%M:%S %Z %Y", ts); // wie date
	    fprintf(out, "%s\n", outstr);
	    fclose(out);
	    isyslog("irmplircd: started by IRMP_LIRC\n");
	    only_once = 0;
	}

	if (buf[1] == 0xFF) continue; // ignore magic

	protocol = buf[1];
	count = buf[6];
	timeout = buf[59];

	min_delta = buf[62];
	delta = buf[63];
	if (delta < 111 || (delta > 117 && delta < 255) || min_delta < 111 || (min_delta > 117 && min_delta < 255)) isyslog("irmplircd: /////////////  ACHTUNG  \\\\\\\\\\\\  delta: %d min_delta: %d\n", delta, min_delta); // kommt raus nach Testen

	if (protocol != lastprotocol) { // new protocol
	    lastprotocol = protocol;
	    if(DEBUG) printf("protocol: %02x, %s\n", protocol, (const char *)protocols[protocol]);
	    isyslog("irmplircd: protocol: %02x, %s\n", protocol, (const char *)protocols[protocol]);
	}

	int Delta = ThisTime.Elapsed(); // the time between two consecutive events
	if (DEBUG) printf("Delta: %d\n", Delta);
	ThisTime.Set();

	if (DEBUG) printf("key: %s, lastkey: %s, timeout: %d\n", (const char*)key, (const char*)lastkey, timeout);

	if (count == 0) { // new key
	    if (DEBUG) printf("new key\n");
	    if (repeat) {
		if (DEBUG) printf("put release for %s\n", (const char*)lastkey);
		Put(lastkey, false, true); // generated release for previous repeated key
	    }
	    lastkey = key;
	    repeat = false;
	    FirstTime.Set();
	} else { // repeat
	    if (DEBUG) printf("repeat\n");
	    if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay) {
		if (DEBUG) printf("continue Delay\n\n");
		continue; // repeat function kicks in after a short delay
	    }
	    if (LastTime.Elapsed() < (uint)Setup.RcRepeatDelta) {
		if (DEBUG)  printf("continue Delta\n\n");
                continue; // skip same keys coming in too fast
	    }
	    repeat = true;
	}

	/* send key */
	if(DEBUG) printf("delta send: %ld\n", LastTime.Elapsed());
	LastTime.Set();
	if (DEBUG) printf("put %s %s\n", (const char*)key, repeat ? "Repeat" : "");
	Put(key, repeat);
	release_needed = true;

    } else { // no key within timeout
	if (release_needed && repeat) {
	    if(DEBUG) printf("put release for %s, delta %ld\n", (const char *)lastkey, ThisTime.Elapsed());
	    Put(lastkey, false, true);
	}
	release_needed = false;
	repeat = false;
	lastkey = "";
	timeout = INT_MAX;
	if (DEBUG) printf("reset\n");
    }
    if (DEBUG) printf("\n");
  }
}

cIrmpRemote *myIrmpRemote = NULL;

class cReadIR : public cThread {
private:
  bool Connect(void);
  void Action(void);
protected:
public:
  cReadIR();
  ~cReadIR();
};

cReadIR::cReadIR(void)
{
  Connect();
  Start();
}

cReadIR::~cReadIR()
{
  Cancel();
  //ioctl(fd, EVIOCGRAB, 0);
  if (fd >= 0)
     close(fd);
  fd = -1;
}

bool cReadIR::Connect()
{
  fd = open(irmp_device, O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    if(DEBUG) printf("Cannot open %s. %s.\n", irmp_device, strerror(errno));
    esyslog("Cannot open %s. %s.\n", irmp_device, strerror(errno));
    return false;
  } else {
    if(DEBUG) printf("opened %s\n", irmp_device);
    isyslog("irmplircd: opened %s\n", irmp_device);
  }

  /*if(ioctl(fd, EVIOCGRAB, 1)){
    if(DEBUG) printf("Cannot grab %s. %s.\n", kbd_device, strerror(errno));
  } else {
    if(DEBUG) printf("Grabbed %s!\n", kbd_device);
  }*/

  return true;
}

void cReadIR::Action(void)
{
  if(DEBUG) printf("ReadIR action!\n");

  while(Running()){

    bool ready = fd >= 0 && cFile::FileReady(fd, -1); // implizit mindestens 100 ms!!! bei timeout 0, wenn was angekommen ist > 0
    int ret = ready ? safe_read(fd, buf, sizeof(buf)) : -1; //  bei timeout, error -1, bei 0 bytes 0, sonst > 0

    if (fd < 0 || ready && ret <= 0) { // kein fd oder error oder 0 bytes
	esyslog("ERROR: irmplirc connection broken, trying to reconnect every %.1f seconds", float(RECONNECTDELAY) / 1000);
	if (fd >= 0)
	    close(fd);
	fd = -1;
	while (Running() && fd < 0) {
	    cCondWait::SleepMs(RECONNECTDELAY);
	    if (Connect()) {
		isyslog("reconnected to irmplirc");
		break;
	    }
	}
    }

    if (buf[0] == REPORT_ID_IR) {
	//if(DEBUG) printf("IR report: %016lx\n", *((uint64_t*)buf));
	myIrmpRemote->Receive();
    } else {
	//if(DEBUG) printf("configuration report\n");
    }
  }
}

class cPluginIrmplirc : public cPlugin {
public:
  cPluginIrmplirc(void);
  virtual ~cPluginIrmplirc() override;
  virtual const char *Version(void) override { return VERSION; }
  virtual const char *Description(void) override { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void) override;
  virtual bool ProcessArgs(int argc, char *argv[]) override;
  virtual bool Start(void) override;
};

cPluginIrmplirc::cPluginIrmplirc(void)
{
}

cPluginIrmplirc::~cPluginIrmplirc()
{
}

const char *cPluginIrmplirc::CommandLineHelp(void)
{
  return "  hid device (/dev/hidraw... )\n"
         "  default /dev/irmp_stm32\n";
}

bool cPluginIrmplirc::ProcessArgs(int argc, char *argv[])
{
  if(argc > 1) irmp_device = argv[1];

  return true;
}

bool cPluginIrmplirc::Start(void)
{
  myIrmpRemote = new cIrmpRemote("IRMP_LIRC");
  new cReadIR();
  return true;
}


VDRPLUGINCREATOR(cPluginIrmplirc); // Don't touch this!

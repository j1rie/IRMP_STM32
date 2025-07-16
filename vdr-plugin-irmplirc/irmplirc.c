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

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = tr("Send keypresses from IRMP HID-Device to VDR");

const char* irmp_device = "/dev/irmp_stm32";

uint8_t debug = 1;

#define RECONNECTDELAY 3000 // ms

class cIrmpRemote : public cRemote, private cThread {
private:
  bool Connect(void);
  virtual void Action(void) override;
public:
  cIrmpRemote(const char *Name);
  bool Stop();
  bool Ready();
  int fd;
};

cIrmpRemote::cIrmpRemote(const char *Name)
:cRemote(Name)
,cThread("IRMP_LIRC remote control")
{
  Connect();
  Start();
}

bool cIrmpRemote::Connect()
{
  fd = open(irmp_device, O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    printf("Cannot open %s. %s.\n", irmp_device, strerror(errno));
    return false;
  } else {
    printf("opened %s\n", irmp_device);
  }

  /*if(ioctl(fd, EVIOCGRAB, 1)){
    printf("Cannot grab %s. %s.\n", kbd_device, strerror(errno));
  } else {
    if(debug) printf("Grabbed %s!\n", kbd_device);
  }*/

  return true;
}

bool cIrmpRemote::Stop()
{
  //ioctl(fd, EVIOCGRAB, 0);
  if (fd)
	close(fd);
  return true;
}

bool cIrmpRemote::Ready(void)
{
  return fd >= 0;
}

void cIrmpRemote::Action(void)
{
  if(debug) printf("action!\n");
  cTimeMs FirstTime;
  cTimeMs LastTime;
  cTimeMs ThisTime;
  uint8_t buf[64];
  uint64_t magic = 0xFF01; // testen!
  uint8_t only_once = 1;
  bool release_needed = false;
  bool repeat = false;
  int lasttimeout = 155, timeout = 155;  // konfigurierbar?!
  uint64_t code = 0, lastcode = 0;

  while(Running()){

  bool ready = fd >= 0 && cFile::FileReady(fd, timeout); // implizit mindestens 100 ms!!!
    int ret = ready ? safe_read(fd, buf, sizeof(buf)) : -1;

    if (fd < 0 || ready && ret <= 0) {
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

    if (ready && ret > 0) { // keypress
	code = *((uint64_t*)buf);
	//if(debug) printf("code: %016lX\n", code);
	code = ((code & 0x00000000FFFFFFFFull) << 32) | ((code & 0xFFFFFFFF00000000ull) >> 32); // make code look like IRMP data
	code = ((code & 0x0000FFFF0000FFFFull) << 16) | ((code & 0xFFFF0000FFFF0000ull) >> 16);
	//code = ((code & 0x00FF00FF00FF00FFull) << 8)  | ((code & 0xFF00FF00FF00FF00ull) >> 8);
	code = ((code & 0x00FF0000000000FFull) << 8)  | ((code & 0xFF0000000000FF00ull) >> 8) | (code & 0x0000FFFFFFFF0000ull);
	if(debug) printf("code neu: %016lX\n", code);

	//if(debug) printf("code: %016lX\n", code);
	if(only_once && code == magic) {
	    if(debug) printf("magic\n");
	    FILE *out = fopen("/var/log/started_by_IRMP_STM32", "a");
	    time_t date = time(NULL);
	    struct tm *ts = localtime(&date);
	    fprintf(out, "%s", asctime(ts));
	    fclose(out);
	    only_once = 0;
	}

	int Delta = ThisTime.Elapsed(); // the time between two consecutive events
	if (debug) printf("Delta: %d\n", Delta);
	ThisTime.Set();
	if(buf[6] == 0) { // new key
	    if (debug) printf("Neuer\n");
	    if (repeat) {
		printf("put release for previous repeat %016lX\n", lastcode);
		Put(lastcode, false, true); // generated release for previous repeated key
	    }
	    lastcode = code;
	    repeat = false;
	    FirstTime.Set();
	    timeout = -1;
	} else { // repeat
	    if (debug) printf("Repeat\n");
	    if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay || LastTime.Elapsed() < (uint)Setup.RcRepeatDelta) {
		if (debug) printf("continue\n\n");
		continue; // don't send key
	    } else {
		repeat = true;
		timeout = Delta * 11 / 10; // 10 % more should be enough // implizit mindestens 100 ms!!!
	    }
	}

	/* send key */
	if(debug) printf("delta send: %ld\n", LastTime.Elapsed());
	LastTime.Set();
	Put(code, repeat);
	release_needed = true;
    } else { // no key within timeout // implizit mindestens 100 ms!!!
	if (release_needed && repeat) {
	    if(debug) printf("delta release: %ld timeout: %d code: %016lX\n", ThisTime.Elapsed(), timeout, code);
	    Put(code, false, true);
	}
	release_needed = false;
	repeat = false;
	count = 0;
	lastcode = 0;
	timeout = -1;
	printf("reset\n");
    }
    printf("\n");
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
  new cIrmpRemote("IRMP_LIRC");
  return true;
}


VDRPLUGINCREATOR(cPluginIrmplirc); // Don't touch this!

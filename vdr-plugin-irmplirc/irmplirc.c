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

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = tr("Send keypresses from IRMP HID-Device to VDR");

const char* irmp_device = "/dev/irmp_stm32";

uint8_t debug = 1;

#define RECONNECTDELAY 3000 // ms

class cIrmpRemote : public cRemote, private cThread {
private:
  bool Connect(void);
  void Action(void);
  bool Ready();
  int fd;
public:
  cIrmpRemote(const char *Name);
  ~cIrmpRemote();
};

cIrmpRemote::cIrmpRemote(const char *Name)
:cRemote(Name)
,cThread("IRMP_LIRC remote control")
{
  Connect();
  Start();
}

cIrmpRemote::~cIrmpRemote()
{
  //ioctl(fd, EVIOCGRAB, 0);
  int fh = fd;
  fd = -1;
  Cancel();
  if (fh >= 0)
     close(fh);
}

bool cIrmpRemote::Connect()
{
  fd = open(irmp_device, O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    if(debug) printf("Cannot open %s. %s.\n", irmp_device, strerror(errno));
    esyslog("Cannot open %s. %s.\n", irmp_device, strerror(errno));
    return false;
  } else {
    if(debug) printf("opened %s\n", irmp_device);
    isyslog("opened %s\n", irmp_device);
  }

  /*if(ioctl(fd, EVIOCGRAB, 1)){
    if(debug) printf("Cannot grab %s. %s.\n", kbd_device, strerror(errno));
  } else {
    if(debug) printf("Grabbed %s!\n", kbd_device);
  }*/

  return true;
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
  uint8_t buf[64];
  uint64_t magic = 0xFF01; // testen!
  uint8_t only_once = 1;
  bool release_needed = false;
  bool repeat = false;
  int timeout = -1;
  uint64_t code = 0, lastcode = 0;
  int RepeatRate = 118;
  unsigned int protocol = 0;
  bool toggle = false;

  if(debug) printf("irmplirc action!\n");

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
	code = code & 0xFFFFFFFFFFFF0000ull; // remove flag repetition

	//protocol = (code & 0x00FF000000000000);

	if (buf[1] != protocol) { // new protocol, reset RepeatRate
	    RepeatRate = 118;
	    protocol = buf[1];
	    if(debug) printf("protocol: %02d, %s\n", protocol, (const char *)protocols[protocol]);
	    isyslog("protocol: %02d, %s\n", protocol, (const char *)protocols[protocol]);
	}

	if (protocol == 6 || protocol == 7 || protocol == 9 || protocol == 12 || protocol == 21 || protocol == 30 || protocol == 45 || protocol == 55) { // RECS80, RC5, RC6, RECS80EXT, RC6A, THOMSON, (S100), METZ
	    toggle = true;
	} else {
	    toggle = false;
	}

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
	if (RepeatRate > Delta)
	    RepeatRate = Delta; // autodetect repeat rate
	ThisTime.Set();
	// don't set own timeout for each protocol, because some are unknown and it is to error prone, so prefer autodetect and treat NEC and Sky+ extra
	timeout = RepeatRate * 103 / 100 + 1;  // 3 % + 1 should presumably be enough
	if (protocol == 2) {
	    timeout = 112; // NEC + APPLE + ONKYO first 40, than 108
	    if(debug) printf("NEC detected, timeout set\n");
	}
	if (protocol == 61) {
	    timeout = 155; // Sky+ 150
	    if(debug) printf("Sky+ detected, timeout set\n");
	}
	if (protocol == 62) {
	    timeout = 155; // Sky+ Pro 150
	    if(debug) printf("Sky+ Pro detected, timeout set\n");
	}
	if (debug) printf("code: %016lX, lastcode: %016lX, toggle: %d, timeout: %d\n", code, lastcode, toggle, timeout);
	// if the protocol toggles count == 0 is reliable, else regard same keys as new only after a timeout
	if (toggle && buf[6] == 0 || !toggle && lastcode != code) { // new key
	    if(debug) printf("Neuer\n");
	    if (repeat) {
		if (debug) printf("put release for %016lX\n", lastcode);
		Put(lastcode, false, true); // generated release for previous repeated key
	    }
	    lastcode = code;
	    repeat = false;
	    FirstTime.Set();
	} else { // repeat
	    if (debug) printf("Repeat\n");
	    if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay) {
		if (debug) printf("continue Delay\n\n");
		continue; // repeat function kicks in after a short delay
	    }
	    if (LastTime.Elapsed() < (uint)Setup.RcRepeatDelta) {
		if (debug)  printf("continue Delta\n\n");
                 continue; // skip same keys coming in too fast
	    }
	    repeat = true;
	}

	/* send key */
	if(debug) printf("delta send: %ld\n", LastTime.Elapsed());
	LastTime.Set();
	Put(code, repeat);
	release_needed = true;

    } else { // no key within timeout
	if (release_needed && repeat) {
	    if(debug) printf("put release for %016lX, delta %ld\n", lastcode, ThisTime.Elapsed());
	    Put(lastcode, false, true);
	}
	release_needed = false;
	repeat = false;
	lastcode = 0;
	timeout = -1;
	if (debug) printf("reset\n");
    }
    if (debug) printf("\n");
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
  //ioctl(fd, EVIOCGRAB, 0);
/*  int fh = fd;
  fd = -1;
  Cancel();
  if (fh >= 0)
     close(fh);*/
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

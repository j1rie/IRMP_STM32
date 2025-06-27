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

class cIrmpRemote : public cRemote, private cThread {
private:
  virtual void Action(void) override;
public:
  cIrmpRemote(const char *Name);
  virtual bool Initialize(void) override;
  virtual bool Stop();
  int fd;
};

cIrmpRemote::cIrmpRemote(const char *Name)
:cRemote(Name)
{
  Initialize();
  Start();
}

bool cIrmpRemote::Initialize()
{
  fd = open(irmp_device, O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    printf("Cannot open %s. %s.\n", irmp_device, strerror(errno));
    //return false;
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

void cIrmpRemote::Action(void)
{
  if(debug) printf("action!\n");
  cTimeMs FirstTime;
  cTimeMs LastTime;
  cTimeMs ThisTime;
  uint8_t buf[64];
  uint64_t magic = 0xFF01; // testen!
  uint8_t only_once = 1;
  bool pressed = false;
  bool repeat = false;
  unsigned int timeout = 0;
  uint64_t code = 0;
  int RepeatRate = 100000;
  uint8_t protocol = 0;

  while(1){
	usleep(1000); // don't eat too much cpu
	if (fd < 0) {
		fd = open(irmp_device, O_RDONLY | O_NONBLOCK);
		if(fd == -1){
			printf("Cannot open %s. %s.\n", irmp_device, strerror(errno));
			sleep(10);
			continue;
		} else {
			printf("opened %s\n", irmp_device);
		}
	}

	if (read(fd, buf, sizeof(buf)) != -1) { // keypress
		code = *((uint64_t*)buf); // TODO swap endianess
		if (buf[1] != protocol) { // new protocol, reset RepeatRate
			RepeatRate = 100000;
			protocol = buf[1];
			if(debug) printf("protocol: %02d\n", protocol);
		}
		//if(debug) printf("code: %016lx\n", code);
		if(only_once && code == magic) {
			if(debug) printf("magic\n");
			FILE *out = fopen("/var/log/started_by_IRMP_STM32", "a");
			time_t date = time(NULL);
			struct tm *ts = localtime(&date);
			fprintf(out, "%s", asctime(ts));
			fclose(out);
			only_once = 0;
		}
		int Delta = ThisTime.Elapsed();
		if (debug) printf("Delta: %d\n", Delta);
		if (RepeatRate > Delta)
			RepeatRate = Delta; // determine repeat rate
		if (debug) printf("RepeatRate: %d\n", RepeatRate);
		ThisTime.Set();
		if(buf[6] == 0 || Delta > RepeatRate * 11 / 10) { // new key
			if (debug) printf("Neuer\n");
			pressed = true;
			repeat = false;
			FirstTime.Set();
		} else { // repeat
			if (debug) printf("Repeat\n");
			if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay || LastTime.Elapsed() < (uint)Setup.RcRepeatDelta) {
				if (debug) printf("continue\n\n");
				continue; // don't send key
			} else {
				pressed = true;
				repeat = true;
				timeout = Delta * 3 / 2; // 11 / 10; // 10 % more should be enough
			}
		}

		/* send key */
		if (pressed){
			if(debug) printf("delta send: %ld\n", LastTime.Elapsed());
			LastTime.Set();
			cRemote::Put(code, repeat);
		}
	}

	/* send release */
	if (ThisTime.Elapsed() > timeout && pressed && repeat) {
		pressed = false;
		if(debug) printf("delta release: %ld\n", ThisTime.Elapsed());
		cRemote::Put(code, false, true);
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
  new cIrmpRemote("IRMP_LIRC");
  return true;
}


VDRPLUGINCREATOR(cPluginIrmplirc); // Don't touch this!

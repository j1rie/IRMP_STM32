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
  bool release_needed = false;
  bool repeat = false;
  unsigned int timeout = 155;  // konfigurierbar?!
  uint64_t code = 0, lastcode = 0;

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
		code = *((uint64_t*)buf);
		//if(debug) printf("code: %016lx\n", code);
		code = ((code & 0x00000000FFFFFFFFull) << 32) | ((code & 0xFFFFFFFF00000000ull) >> 32); // make code look like IRMP data
		code = ((code & 0x0000FFFF0000FFFFull) << 16) | ((code & 0xFFFF0000FFFF0000ull) >> 16);
		//code = ((code & 0x00FF00FF00FF00FFull) << 8)  | ((code & 0xFF00FF00FF00FF00ull) >> 8);
		code = ((code & 0x00FF0000000000FFull) << 8)  | ((code & 0xFF0000000000FF00ull) >> 8) | (code & 0x0000FFFFFFFF0000ull);
		if(debug) printf("code neu: %016lx\n", code);

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
		int Delta = ThisTime.Elapsed(); // the time between two consecutive events
		if (debug) printf("Delta: %d\n", Delta);
		ThisTime.Set();
		if(buf[6] == 0) { // new key
			if (debug) printf("Neuer\n");
			if (repeat) {
				printf("put release for previous repeat %016lx\n", lastcode);
				Put(lastcode, false, true); // generated release for previous repeated key
			}
			lastcode = code;
			repeat = false;
			FirstTime.Set();
		} else { // repeat
			if (debug) printf("Repeat\n");
			if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay || LastTime.Elapsed() < (uint)Setup.RcRepeatDelta) {
				if (debug) printf("continue\n\n");
				continue; // don't send key
			} else {
				repeat = true;
				timeout = Delta * 3 / 2; // 11 / 10; // 10 % more should be enough
			}
		}

		/* send key */
		if(debug) printf("delta send: %ld\n", LastTime.Elapsed());
		LastTime.Set();
		cRemote::Put(code, repeat);
		release_needed = true;
	}

	/* send release */
	if (ThisTime.Elapsed() > timeout && release_needed && repeat) {
		release_needed = false;
		if(debug) printf("delta release: %ld timeout: %d code: %016lx\n", ThisTime.Elapsed(), timeout, code);
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

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
//#include "input-event-codes.h"

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
  uint8_t buf[64];
  uint64_t magic = 0xFF01; // testen!
  uint8_t only_once = 1;
  uint8_t release_needed = 0, repeat = 0, skip = 0;
  long int this_time, last_sent = 0, last_received = 0, timeout = 0;
  uint64_t code = 0;
  struct timespec now; // TODO unstellen auf VDR's cTimeMs
  int Delta; // the time between two subsequent LIRC events
  int RepeatRate = 100000;

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

	if (read(fd, buf, sizeof(buf)) != -1) {
		code = *((uint64_t*)buf);
		//printf("code: %016lx\n", code);
		clock_gettime(CLOCK_MONOTONIC, &now);
		this_time = now.tv_sec * 1000 + now.tv_nsec / 1000 / 1000;
		if(only_once && code == magic) {
			printf("magic\n");
			FILE *out = fopen("/var/log/started_by_IRMP_STM32", "a");
			time_t date = time(NULL);
			struct tm *ts = localtime(&date);
			fprintf(out, "%s", asctime(ts));
			fclose(out);
			only_once = 0;
		}
		Delta = this_time - last_received;
		if (debug) printf("Delta: %d\n", Delta);
		if (RepeatRate > Delta)
			RepeatRate = Delta; // determine repeat rate
		last_received = this_time;
		if(buf[6] == 0 || Delta > RepeatRate * 11 / 10) { // new key
			printf("Neuer\n");
			repeat = 0;
			skip = 0;
		} else { // repeat
			printf("Repeat\n");
			if (this_time - last_sent < (uint)Setup.RcRepeatDelay || this_time - last_sent < (uint)Setup.RcRepeatDelta) {
				skip = 1;
				continue; // don't send key
			} else {
				repeat = 1;
				skip = 0;
				timeout = Delta * 11 / 10; // 10 % more should be enough
			}
		}

		/* send key */
		if (!skip){
			if(debug) printf("delta send: %ld\n", this_time - last_sent); /// TODO checken!!!
			cRemote::Put(code, repeat);
			last_sent = this_time;
			//if(debug) printf("put code: %016lx, %s\n", code, repeat ? "repeat" : "first");
			release_needed = 1;
		}
	}

	/* send release */
	clock_gettime(CLOCK_MONOTONIC, &now);
	this_time = now.tv_sec * 1000 + now.tv_nsec / 1000 / 1000;
	if (this_time - last_received > timeout) {
		if (release_needed && repeat) { // statt release_needed !skip
			release_needed = 0;
			if(debug) printf("delta release: %ld\n", this_time - last_received); /// TODO checken!!!
			cRemote::Put(code, false, true);
			//if(debug) printf("put %ld %016lx release\n\n", this_time, code);
		}
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

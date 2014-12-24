
PROJECTNAME=stm32IRconfig
VERSION=0.0.14

CC              = g++
//PKG_CONFIG      = pkg-config

CFLAGS          = -O -Wall
//CFLAGS          = `$(PKG_CONFIG) --cflags libusb` -O -Wall
//LIBS            = `$(PKG_CONFIG) --libs libusb`

TMPDIR = /tmp


all: stm32IRconfig

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32IRconfig: stm32IRconfig.o
	$(CC) -o stm32IRconfig stm32IRconfig.o

#$(LIBS)

clean:
	rm -f stm32IRconfig.o
	rm -f stm32IRconfig

dist: clean
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@mkdir $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@cp -a * $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@tar cjf $(PROJECTNAME)-$(VERSION).tar.bz2 -C $(TMPDIR) $(PROJECTNAME)-$(VERSION)
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@echo Distribution package created as $(PROJECTNAME)-$(VERSION).tar.bz2

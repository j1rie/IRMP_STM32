
PROJECTNAME=stm32IRalarm
VERSION=0.0.7

CC              = g++
//PKG_CONFIG      = pkg-config

CFLAGS          = -O -Wall
//CFLAGS          = `$(PKG_CONFIG) --cflags libusb` -O -Wall
//LIBS            = `$(PKG_CONFIG) --libs libusb`

TMPDIR = /tmp


all: stm32IRalarm

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32IRalarm: stm32IRalarm.o
	$(CC) -o stm32IRalarm stm32IRalarm.o

#$(LIBS)

clean:
	rm -f stm32IRalarm.o
	rm -f stm32IRalarm

dist: clean
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@mkdir $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@cp -a * $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@tar cjf $(PROJECTNAME)-$(VERSION).tar.bz2 -C $(TMPDIR) $(PROJECTNAME)-$(VERSION)
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@echo Distribution package created as $(PROJECTNAME)-$(VERSION).tar.bz2

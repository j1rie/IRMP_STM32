#ifndef _UPGRADE_h_
#define _UPGRADE_h_
#include "dfu.h"
#include <fx.h>
#ifdef WIN32
#include <FXCP1252Codec.h>
#endif

class Upgrade : public FXThread {
private:
	const char* firmwarefile{NULL};
	char* print{NULL};
	char* printcollect{NULL};
	FXGUISignal* guisignal{NULL};
public:
	void set_firmwarefile(const char* pfirmwarefile);
	void set_print(char* pprint);
	void set_printcollect(char* pprintcollect);
	void set_signal(FXGUISignal* pguisignal);
	FXint run();
};

#endif

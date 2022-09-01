/*
 *  GUI Config Tool for IRMP STM32 devices
 *
 *  Copyright (C) 2015-2022 Joerg Riechardt
 *
 *  based on work by Alan Ott
 *  Copyright 2010  Alan Ott
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <fx.h>

#include "hidapi.h"
//#include "mac_support.h"
#include <limits.h>
#include <inttypes.h>
#include <FXArray.h>
#include "icons.h"
#include "upgrade.h"

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
	#include <FXCP1252Codec.h>
#else
	#include <unistd.h>
#endif

class MainWindow : public FXMainWindow {
	FXDECLARE(MainWindow)
	
public:
	enum {
		ID_FIRST = FXMainWindow::ID_LAST,
		ID_CONNECT,
		ID_DISCONNECT,
		ID_RESCAN,
		ID_REBOOT,
		ID_SEND_OUTPUT_REPORT,
		ID_PWAKEUP,
		ID_PMACRO,
		ID_PRWAKEUP,
		ID_PRMACRO,
		ID_GWAKEUP,
		ID_GMACRO,
		ID_GCAP,
		ID_AGET,
		ID_ASET,
		ID_RWAKEUP,
		ID_RMACRO,
		ID_RALARM,
		ID_SEND,
		ID_READ_CONT,
		ID_UPGRADE,
		ID_PRINT,
		ID_CLEAR,
		ID_TIMER,
		ID_READIR_TIMER,
		ID_RED_TIMER,
		ID_MAC_TIMER,
		ID_LAST,
		ID_WSLISTBOX,
		ID_MNLISTBOX,
		ID_MSLISTBOX,
		ID_RPLISTBOX,
		ID_OPEN,
		ID_SAVE,
		ID_SAVE_LOG,
		ID_APPEND,
		ID_WRITE_IR,
		ID_DEVLIST,
		ID_REEPROM
	};

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
	CMD_EEPROM_RESET
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

	FXGUISignal *guisignal = new FXGUISignal(getApp(), this, ID_PRINT);
	Upgrade doUpgrade;

private:
	FXList *device_list;
	FXButton *connect_button;
	FXButton *disconnect_button;
	FXButton *rescan_button;
	FXButton *output_button;
	FXButton *reboot_button;
	FXButton *pwakeup_button;
	FXButton *pmacro_button;
	FXButton *prwakeup_button;
	FXButton *prmacro_button;
	FXButton *gwakeup_button;
	FXButton *gmacro_button;
	FXButton *gcap_button;
	FXButton *aget_button;
	FXButton *aset_button;
	FXButton *rwakeup_button;
	FXButton *rmacro_button;
	FXButton *ralarm_button;
	FXButton *send_button;
	FXButton *read_cont_button;
	FXButton *upgrade_button;
	FXButton *open_button;
	FXButton *save_button;
	FXButton *append_button;
	FXButton *write_ir_button;
	FXButton *reset_button;
	FXLabel *connected_label;
	FXLabel *connected_label2;
	FXLabel *connected_label3;
	FXTextField *output_text;
	FXTextField *protocol_text;
	FXTextField *address_text;
	FXTextField *command_text;
	FXTextField *flag_text;
	FXTextField *days_text;
	FXTextField *hours_text;
	FXTextField *minutes_text;
	FXTextField *seconds_text;
	FXText *protocol1_text;
	FXText *address1_text;
	FXText *command1_text;
	FXText *flag1_text;
	FXText *days1_text;
	FXText *hours1_text;
	FXText *minutes1_text;
	FXText *seconds1_text;
	FXText *input_text;
	FXText *map_text21;
	FXListBox* wslistbox;
	FXListBox* mnlistbox;
	FXListBox* mslistbox;
	FXText *map_text;
	FXStatusBar *statusbar;
	struct hid_device_info *devices;
	hid_device *connected_device;
	size_t getDataFromTextField(FXTextField *tf, uint8_t *buf, size_t len);
	uint8_t buf[64];
	uint8_t bufw[64];
	uint8_t ReadIRcontActive;
	uint8_t ReadIRActive;
	uint8_t reduce_timeout;
	int wakeupslots;
	int macrodepth;
	int macroslots;
	int in_size;
	int out_size;
	FXString protocols;
	FXString firmware;
	FXString firmware1;
	FXString uC;
	FXColor storedShadowColor;
	FXColor storedBaseColor;
	FXColor storedBackColor;
	int RepeatCounter;
	FXString map[400];
	int mapbeg[200];
	int active_lines;
	int max;
	int count;
	char firmwarefile[512];
	char print[1024];
	char printcollect[4096];
	FXint cur_item;
	FXint num_devices_before_upgrade;
	FXint num_devices_after_rescan;

protected:
	MainWindow() {};
public:
	MainWindow(FXApp *app);
	~MainWindow();
	virtual void create();
	
	long onConnect(FXObject *sender, FXSelector sel, void *ptr);
	long onDisconnect(FXObject *sender, FXSelector sel, void *ptr);
	long onRescan(FXObject *sender, FXSelector sel, void *ptr);
	long onReboot(FXObject *sender, FXSelector sel, void *ptr);
	long onSendOutputReport(FXObject *sender, FXSelector sel, void *ptr);
	long onPwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onPmacro(FXObject *sender, FXSelector sel, void *ptr);
	long onPRwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onPRmacro(FXObject *sender, FXSelector sel, void *ptr);
	long onGwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onGmacro(FXObject *sender, FXSelector sel, void *ptr);
	long onGcaps(FXObject *sender, FXSelector sel, void *ptr);
	long onAget(FXObject *sender, FXSelector sel, void *ptr);
	long onAset(FXObject *sender, FXSelector sel, void *ptr);
	long onRwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onRmacro(FXObject *sender, FXSelector sel, void *ptr);
	long onRalarm(FXObject *sender, FXSelector sel, void *ptr);
	long onSendIR(FXObject *sender, FXSelector sel, void *ptr);
	long onReadIR(FXObject *sender, FXSelector sel, void *ptr);
	long onReadIRcont(FXObject *sender, FXSelector sel, void *ptr);
	long onUpgrade(FXObject *sender, FXSelector sel, void *ptr);
	long onPrint(FXObject *sender, FXSelector sel, void *ptr);
	long onClear(FXObject *sender, FXSelector sel, void *ptr);
	long onTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onReadIRTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onRedTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onMacTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onCmdwsListBox(FXObject*,FXSelector,void*);
	long onCmdmnListBox(FXObject*,FXSelector,void*);
	long onCmdmsListBox(FXObject*,FXSelector,void*);
	long onNew(FXObject *sender, FXSelector sel, void *ptr);
	long onOpen(FXObject *sender, FXSelector sel, void *ptr);
	long onSave(FXObject *sender, FXSelector sel, void *ptr);
	long onSaveLog(FXObject *sender, FXSelector sel, void *ptr);
	long onReeprom(FXObject *sender, FXSelector sel, void *ptr);
	long Write(int out_len);
	long Read(int show_len);
	long Write_and_Check(int out_len, int show_len);
	long saveFile(const FXString& file);
	long saveLogFile(const FXString& file);
	long onAppend(FXObject *sender, FXSelector sel, void *ptr);
	long onApply(FXObject *sender, FXSelector sel, void *ptr);
	long onWrite_IR(FXObject *sender, FXSelector sel, void *ptr);
	long onDevDClicked(FXObject *sender, FXSelector sel, void *ptr);
	long onCmdQuit(FXObject *sender, FXSelector sel, void *ptr);
};

// FOX 1.7 changes the timeouts to all be nanoseconds.
// Fox 1.6 had all timeouts as milliseconds. // this is true only for addTimeout(), but NOT for FXThread::sleep() !
#if (FOX_MINOR >= 7)
	const int timeout_scalar = 1000*1000;
#else
	const int timeout_scalar = 1;
#endif

FXMainWindow *g_main_window;

FXDEFMAP(MainWindow) MainWindowMap [] = {
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_CONNECT, MainWindow::onConnect ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_DISCONNECT, MainWindow::onDisconnect ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESCAN, MainWindow::onRescan ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_REBOOT, MainWindow::onReboot ),
	FXMAPFUNC(SEL_DOUBLECLICKED, MainWindow::ID_DEVLIST, MainWindow::onDevDClicked ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SEND_OUTPUT_REPORT, MainWindow::onSendOutputReport ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PWAKEUP, MainWindow::onPwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PMACRO, MainWindow::onPmacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PRWAKEUP, MainWindow::onPRwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PRMACRO, MainWindow::onPRmacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GWAKEUP, MainWindow::onGwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GMACRO, MainWindow::onGmacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GCAP, MainWindow::onGcaps ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_REEPROM, MainWindow::onReeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_AGET, MainWindow::onAget ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_ASET, MainWindow::onAset ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RWAKEUP, MainWindow::onRwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RMACRO, MainWindow::onRmacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RALARM, MainWindow::onRalarm ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SEND, MainWindow::onSendIR ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_READ_CONT, MainWindow::onReadIRcont ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_UPGRADE, MainWindow::onUpgrade ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_CLEAR, MainWindow::onClear ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_TIMER, MainWindow::onTimeout ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_READIR_TIMER, MainWindow::onReadIRTimeout ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_RED_TIMER, MainWindow::onRedTimeout ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_MAC_TIMER, MainWindow::onMacTimeout ),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_WSLISTBOX, MainWindow::onCmdwsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_WSLISTBOX, MainWindow::onCmdwsListBox),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_MNLISTBOX, MainWindow::onCmdmnListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_MNLISTBOX, MainWindow::onCmdmnListBox),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_MSLISTBOX, MainWindow::onCmdmsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_MSLISTBOX, MainWindow::onCmdmsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_OPEN, MainWindow::onOpen ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SAVE, MainWindow::onSave ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SAVE_LOG, MainWindow::onSaveLog ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_APPEND, MainWindow::onAppend ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_WRITE_IR, MainWindow::onWrite_IR ),
	FXMAPFUNC(SEL_CLOSE,   0, MainWindow::onCmdQuit ),
	FXMAPFUNC(SEL_IO_READ, MainWindow::ID_PRINT, MainWindow::onPrint),
};

FXIMPLEMENT(MainWindow, FXMainWindow, MainWindowMap, ARRAYNUMBER(MainWindowMap));

MainWindow::MainWindow(FXApp *app)
	: FXMainWindow(app, "IRMP STM32 Configuration", NULL, NULL, DECOR_ALL, 425, 39, 1050, 1030)  // for 1920x1080
{
	this->setIcon(new FXGIFIcon(app,Icon)); // for taskbar
	this->setMiniIcon(new FXICOIcon(app,MiniIcon)); // for titlebar
	devices = NULL;
	connected_device = NULL;

	// create horizontal frame with two vertical frames
	FXHorizontalFrame *hf1 = new FXHorizontalFrame(this, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 2,0);
	FXVerticalFrame *vf1 = new FXVerticalFrame(hf1, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
	FXVerticalFrame *vf2 = new FXVerticalFrame(hf1, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0/*,0,0,0*/);

	// second vertical frame for map_text21
	new FXLabel(vf2, "map                                    ");
	FXVerticalFrame *innerVF21 = new FXVerticalFrame(vf2, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
	map_text21 = new FXText(new FXHorizontalFrame(innerVF21,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0,0,0), NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y/*,0,0,0,0, 0,0,0,0*/);

	// first vertical frame: everything else
	// Device List and Connect/Disconnect buttons
	FXHorizontalFrame *hf11 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X);
	device_list = new FXList(new FXHorizontalFrame(hf11,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0), this, ID_DEVLIST, LISTBOX_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,300,200);
	FXVerticalFrame *buttonVF11 = new FXVerticalFrame(hf11);
	connect_button = new FXButton(buttonVF11, "Connect", NULL, this, ID_CONNECT, BUTTON_NORMAL|LAYOUT_FILL_X);
	disconnect_button = new FXButton(buttonVF11, "Disconnect", NULL, this, ID_DISCONNECT, BUTTON_NORMAL|LAYOUT_FILL_X);
	disconnect_button->disable();
	rescan_button = new FXButton(buttonVF11, "Re-Scan devices", NULL, this, ID_RESCAN, BUTTON_NORMAL|LAYOUT_FILL_X);
	reboot_button = new FXButton(buttonVF11, "Reboot device", NULL, this, ID_REBOOT, BUTTON_NORMAL|LAYOUT_FILL_X);
	connected_label = new FXLabel(vf1, "Disconnected");
	connected_label2 = new FXLabel(vf1, "Firmware:");
	connected_label3 = new FXLabel(vf1, "Protocols:");
	
	// horizontal frame of group boxes
	FXHorizontalFrame *hf12 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X|PACK_UNIFORM_WIDTH);
	//set Group Box
	FXGroupBox *gb121 = new FXGroupBox(hf12, "set", FRAME_GROOVE|LAYOUT_FILL_X);
	pwakeup_button = new FXButton(gb121, "wakeup", NULL, this, ID_PWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	pmacro_button = new FXButton(gb121, "macro", NULL, this, ID_PMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	//set by remote Group Box
	FXGroupBox *gb122 = new FXGroupBox(hf12, "set by remote", FRAME_GROOVE|LAYOUT_FILL_X);
	prwakeup_button = new FXButton(gb122, "wakeup", NULL, this, ID_PRWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	prmacro_button = new FXButton(gb122, "macro", NULL, this, ID_PRMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	//get Group Box
	FXGroupBox *gb123 = new FXGroupBox(hf12, "get", FRAME_GROOVE|LAYOUT_FILL_X);
	gwakeup_button = new FXButton(gb123, "wakeup", NULL, this, ID_GWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	gmacro_button = new FXButton(gb123, "macro", NULL, this, ID_GMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	gcap_button = new FXButton(gb123, "caps", NULL, this, ID_GCAP, BUTTON_NORMAL|LAYOUT_FILL_X);
	//reset Group Box
	FXGroupBox *gb124 = new FXGroupBox(hf12, "reset", FRAME_GROOVE|LAYOUT_FILL_X);
	rwakeup_button = new FXButton(gb124, "wakeup", NULL, this, ID_RWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	rmacro_button = new FXButton(gb124, "macro", NULL, this, ID_RMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	ralarm_button = new FXButton(gb124, "alarm", NULL, this, ID_RALARM, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame for IR Group Box and firmware upgrade and eeprom reset
	FXHorizontalFrame *hf13 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
	FXSpring *s131 = new FXSpring(hf13, LAYOUT_FILL_X, 300, 0, 0,0,0,0, 4,2,2,2, 2,2);
	FXSpring *s132 = new FXSpring(hf13, LAYOUT_FILL_X, 100, 0, 0,0,0,0, 2,4,2,2, 2,2);

	//IR Group Box
	FXGroupBox *gb131 = new FXGroupBox(s131, "IR (hex)", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,4);
	FXMatrix *m131 = new FXMatrix(gb131, 6, MATRIX_BY_COLUMNS,0,0,0,0);
	new FXLabel(m131, "");
	new FXLabel(m131, "protocol");
	new FXLabel(m131, "address");
	new FXLabel(m131, "command");
	new FXLabel(m131, "flag");
	new FXLabel(m131, "");
	new FXLabel(m131, "set");
	protocol_text = new FXTextField(m131, 4, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	address_text = new FXTextField(m131, 8, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	command_text = new FXTextField(m131, 8, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	flag_text = new FXTextField(m131, 4, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	send_button = new FXButton(m131, "send", NULL, this, ID_SEND, BUTTON_NORMAL|LAYOUT_FILL_X,0,0,0,0,0,0,0,0);
	new FXLabel(m131, "get");
	FXVerticalFrame *innerVF1 = new FXVerticalFrame(m131, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXVerticalFrame *innerVF2 = new FXVerticalFrame(m131, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXVerticalFrame *innerVF3 = new FXVerticalFrame(m131, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXVerticalFrame *innerVF4 = new FXVerticalFrame(m131, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	protocol1_text = new FXText(new FXHorizontalFrame(innerVF1,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X);
	address1_text = new FXText(new FXHorizontalFrame(innerVF2,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X);
	command1_text = new FXText(new FXHorizontalFrame(innerVF3,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X);
	flag1_text = new FXText(new FXHorizontalFrame(innerVF4,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X);
	read_cont_button = new FXButton(m131, " receive mode ", NULL, this, ID_READ_CONT, BUTTON_NORMAL|LAYOUT_FILL_X,0,0,0,0,0,0,0,0);
	protocol1_text->setVisibleRows(1);
	address1_text->setVisibleRows(1);
	command1_text->setVisibleRows(1);
	flag1_text->setVisibleRows(1);

	//firmware Group Box
	FXGroupBox *gb132 = new FXGroupBox(s132, "firmware", FRAME_GROOVE|LAYOUT_FILL_X);
	upgrade_button = new FXButton(gb132, "upgrade", NULL, this, ID_UPGRADE, BUTTON_NORMAL|LAYOUT_FILL_X);

	//eeprom Group Box
	FXGroupBox *gb133 = new FXGroupBox(s132, "eeprom", FRAME_GROOVE|LAYOUT_FILL_X);
	reset_button = new FXButton(gb133, "reset", NULL, this, ID_REEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame for alarm Group Box, select listboxes, Output Group Box and map group box
	FXHorizontalFrame *hf14 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
	// two vertical frames, left for map, right for everything else 
	FXVerticalFrame *vf141 = new FXVerticalFrame(hf14, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf142 = new FXVerticalFrame(hf14, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0/*,0,0,0*/);
	// horizontal frame for alarm Group Box and select listboxes
	FXHorizontalFrame *hf143 = new FXHorizontalFrame(vf141, LAYOUT_FILL_X/*,0,0,0,0, 0,0,0,0*/);
	//alarm Group Box
	FXGroupBox *gb14 = new FXGroupBox(hf143, "alarm (dec)", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,6);
	FXMatrix *m14 = new FXMatrix(gb14, 5, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	new FXLabel(m14, "days");
	new FXLabel(m14, "hours");
	new FXLabel(m14, "minutes");
	new FXLabel(m14, "seconds");
	new FXLabel(m14, "");
	days_text = new FXTextField(m14, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	hours_text = new FXTextField(m14, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	minutes_text = new FXTextField(m14, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	seconds_text = new FXTextField(m14, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	aset_button = new FXButton(m14, "set", NULL, this, ID_ASET, BUTTON_NORMAL|LAYOUT_FILL_X);
	FXVerticalFrame *innerVF5 = new FXVerticalFrame(m14, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXVerticalFrame *innerVF6 = new FXVerticalFrame(m14, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXVerticalFrame *innerVF7 = new FXVerticalFrame(m14, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXVerticalFrame *innerVF8 = new FXVerticalFrame(m14, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	days1_text = new FXText(new FXHorizontalFrame(innerVF5,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X);
	hours1_text = new FXText(new FXHorizontalFrame(innerVF6,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X);
	minutes1_text = new FXText(new FXHorizontalFrame(innerVF7,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X);
	seconds1_text = new FXText(new FXHorizontalFrame(innerVF8,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X);
	aget_button = new FXButton(m14, "get", NULL, this, ID_AGET, BUTTON_NORMAL|LAYOUT_FILL_X);
	days1_text->setVisibleRows(1);
	hours1_text->setVisibleRows(1);
	minutes1_text->setVisibleRows(1);
	seconds1_text->setVisibleRows(1);
	// select listboxes
	FXGroupBox *gb143 = new FXGroupBox(hf143, "select", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	wslistbox=new FXListBox(gb143,this,ID_WSLISTBOX,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	mnlistbox=new FXListBox(gb143,this,ID_MNLISTBOX,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	mslistbox=new FXListBox(gb143,this,ID_MSLISTBOX,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	// map group box
	FXGroupBox *gb142 = new FXGroupBox(vf142, "map", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	open_button = new FXButton(gb142, "open", NULL, this, ID_OPEN, BUTTON_NORMAL|LAYOUT_FILL_X);
	save_button = new FXButton(gb142, "save", NULL, this, ID_SAVE, BUTTON_NORMAL|LAYOUT_FILL_X);
	append_button = new FXButton(gb142, "append", NULL, this, ID_APPEND, BUTTON_NORMAL|LAYOUT_FILL_X);
	//apply_button = new FXButton(gb142, "apply", NULL, this, ID_APPLY, BUTTON_NORMAL|LAYOUT_FILL_X);
	write_ir_button = new FXButton(gb142, "write IR", NULL, this, ID_WRITE_IR, BUTTON_NORMAL|LAYOUT_FILL_X);
	new FXLabel(gb142, "Key");
	FXVerticalFrame *innerVF9 = new FXVerticalFrame(gb142, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	map_text = new FXText(new FXHorizontalFrame(innerVF9,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X);
	map_text->setVisibleRows(1);

	// horizontal frame for Output Group Box
	FXHorizontalFrame *hf15 = new FXHorizontalFrame(vf141, LAYOUT_FILL_X);
	// Output Group Box
	FXGroupBox *gb15 = new FXGroupBox(hf15, "PC->STM32", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0);
	FXMatrix *m3 = new FXMatrix(gb15, 2, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	new FXLabel(m3, "Data");
	new FXLabel(m3, "");
	output_text = new FXTextField(m3, 29, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	output_button = new FXButton(m3, "Send to IRMP Device", NULL, this, ID_SEND_OUTPUT_REPORT, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame for Input Group Box
	FXHorizontalFrame *hf16 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	// Input Group Box
	FXGroupBox *gb16 = new FXGroupBox(hf16, "debug messages", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXHorizontalFrame *innerHF16 = new FXHorizontalFrame(gb16, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	input_text = new FXText(new FXHorizontalFrame(innerHF16,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	input_text->setEditable(false);
	FXVerticalFrame *innerVF16 = new FXVerticalFrame(innerHF16,LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXButton *clear_button = new FXButton(innerVF16, "Clear", NULL, this, ID_CLEAR, BUTTON_NORMAL|LAYOUT_FILL_X);
	FXButton *save_log_button = new FXButton(innerVF16, "Save", NULL, this, ID_SAVE_LOG, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame for Status Bar
	FXHorizontalFrame *hf17 = new FXHorizontalFrame(vf1, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X,0,0,0,0, 1,2,0,3);
	//Status Bar
	statusbar = new FXStatusBar(hf17, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

	// HelpText
	device_list->setHelpText("select one of found IRMP_STM32 devices (doubleclick connects)");
	connect_button->setHelpText("connect to selected device");
	disconnect_button->setHelpText("disconnect device");
	rescan_button->setHelpText("rescan devices");
	reboot_button->setHelpText("reboot device");
	pwakeup_button->setHelpText("set wakeup");
	pmacro_button->setHelpText("set macro");
	prwakeup_button->setHelpText("set wakeup by remote");
	prmacro_button->setHelpText("set macro by remote");
	gwakeup_button->setHelpText("get wakeup");
	gmacro_button->setHelpText("get macro");
	gcap_button->setHelpText("get capabilities");
	rwakeup_button->setHelpText("reset wakeup");
	rmacro_button->setHelpText("reset macro");
	ralarm_button->setHelpText("reset alarm");
	protocol_text->setHelpText("enter IR protocol to be set");
	address_text->setHelpText("enter IR address to be set");
	command_text->setHelpText("enter IR command to be set");
	flag_text->setHelpText("enter IR flags to be set");
	send_button->setHelpText("send IR");
	protocol1_text->setHelpText("received IR protocol");
	address1_text->setHelpText("received IR address");
	command1_text->setHelpText("received IR command");
	flag1_text->setHelpText("received IR flags");
	read_cont_button->setHelpText("receive IR until pressed again");
	upgrade_button->setHelpText("upgrade firmware");
	reset_button->setHelpText("reset eeprom");
	days_text->setHelpText("enter days to be set");
	hours_text->setHelpText("enter hours to be set");
	minutes_text->setHelpText("enter minutes to be set");
	seconds_text->setHelpText("enter seconds to be set");
	aset_button->setHelpText("set alarm");
	days1_text->setHelpText("alarm days read from device");
	hours1_text->setHelpText("alarm hours read from device");
	minutes1_text->setHelpText("alarm minutes read from device");
	seconds1_text->setHelpText("alarm seconds read from device");
	aget_button->setHelpText("get alarm");
	wslistbox->setHelpText("wakeupslot to be set");
	mnlistbox->setHelpText("macronumber to be set");
	mslistbox->setHelpText("macroslot to be set");
	output_text->setHelpText("data to be sent to device (experts only)");
	output_button->setHelpText("send to device");
	input_text->setHelpText("debug messages");
	clear_button->setHelpText("clear debug messages");
	save_log_button->setHelpText("save debug messages");
	open_button->setHelpText("open translation map");
	map_text->setHelpText("key (translated received IR)");
	save_button->setHelpText("save translation map");
	map_text21->setHelpText("editable translation map");
	append_button->setHelpText("add received IR and key to translation map");
	write_ir_button->setHelpText("overwrite IR");

	// disable buttons
	output_button->disable();
	pwakeup_button->disable();
	pmacro_button->disable();
	prwakeup_button->disable();
	prmacro_button->disable();
	gwakeup_button->disable();
	gmacro_button->disable();
	gcap_button->disable();
	aget_button->disable();
	aset_button->disable();
	rwakeup_button->disable();
	rmacro_button->disable();
	ralarm_button->disable();
	send_button->disable();
	read_cont_button->disable();
	reboot_button->disable();

	// save Colors
	storedShadowColor = read_cont_button->getShadowColor();
	storedBaseColor = read_cont_button->getBaseColor();
	storedBackColor = read_cont_button->getBackColor();

	// initialize
	ReadIRcontActive = 0;
	ReadIRActive = 0;
	reduce_timeout = 0;
	RepeatCounter = 0;
	active_lines = 0;
	wakeupslots = 0;
	macrodepth = 0;
	macroslots = 0;
	protocols = "";
	firmware = "";
	firmware1 = "";
	max = 0;
	count = 0;
	out_size = 64; // at first out_size is unknown, so use HID maximum
}

MainWindow::~MainWindow()
{
	if (connected_device)
		hid_close(connected_device);
	hid_exit();
	delete guisignal;
}

long
MainWindow::onCmdQuit(FXObject *sender, FXSelector sel, void *ptr)
{
	if(map_text21->isModified()){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("map was changed"),tr("Discard changes to map?"))==MBOX_CLICKED_NO) return 1;
	}
	getApp()->exit(0);
	return 0;
}

void
MainWindow::create()
{
	FXMainWindow::create();
	show();

	onRescan(NULL, 0, NULL);
	onConnect(NULL, 0, NULL);

#ifdef __APPLE__
	init_apple_message_system();
	
	getApp()->addTimeout(this, ID_MAC_TIMER,
		50 * timeout_scalar /*50ms*/);
#endif
}

long
MainWindow::onConnect(FXObject *sender, FXSelector sel, void *ptr)
{
	if (connected_device)
		return 1;
	
	FXint cur_item = device_list->getCurrentItem();
	if (cur_item < 0)
		return -1;
	FXListItem *item = device_list->getItem(cur_item);
	if (!item)
		return -1;
	struct hid_device_info *device_info = (struct hid_device_info*) item->getData();
	if (!device_info)
		return -1;
	
	connected_device =  hid_open_path(device_info->path);
	
	if (!connected_device) {
		FXMessageBox::error(this, MBOX_OK, "Device Error oC", "Unable To Connect to Device");
		return -1;
	}

	if(onGcaps(NULL, 0, NULL) == -1)
		return -1;
	FXString s;
	s.format("Connected to: %04hx:%04hx -", device_info->vendor_id, device_info->product_id);
	//s += FXString(" ") + device_info->manufacturer_string;
	s += FXString(" ") + device_info->product_string;
	connected_label->setText(s);
	s = "Firmware: ";
	FXint pos = firmware.find("   ", 3);
	firmware1 = firmware.left(pos);
	firmware.substitute("___","   ");
	firmware.substitute(":_",": ");
	s += firmware;
	connected_label2->setText(s);
	s = "Protocols: ";
	s += protocols;
	connected_label3->setText(s);
	for(int i = 0; i < wakeupslots; i++) {
		s = (i < wakeupslots-1) ? "wakeup" : "reboot";
#if (FOX_MINOR >= 7)
		FXString t;
		t.fromInt(i,10);
		s += (i < wakeupslots-1) ? t : "";
#else
		s += (i < wakeupslots-1) ? FXStringVal(i,10) : "";
#endif
		wslistbox->appendItem(s);	
	}
	wslistbox->setNumVisible(wakeupslots);
	for(int i = 0; i < macrodepth; i++) {
		s = "macro";
#if (FOX_MINOR >= 7)
		FXString t;
		t.fromInt(i,10);
		s += t;
#else
		s += FXStringVal(i,10);
#endif
		mnlistbox->appendItem(s);	
	}
	mnlistbox->setNumVisible(macrodepth);
	for(int i = 0; i < macroslots; i++) {
		s = "macroslot";
#if (FOX_MINOR >= 7)
		FXString t;
		t.fromInt(i,10);
		s += t;
#else
		s += FXStringVal(i,10);
#endif
		mslistbox->appendItem(s);	
	}
	mslistbox->setNumVisible(macroslots);
	output_button->enable();
	pwakeup_button->enable();
	pmacro_button->enable();
	prwakeup_button->enable();
	prmacro_button->enable();
	gwakeup_button->enable();
	gmacro_button->enable();
	gcap_button->enable();
	aget_button->enable();
	aset_button->enable();
	rwakeup_button->enable();
	rmacro_button->enable();
	ralarm_button->enable();
	send_button->enable();
	read_cont_button->enable();
	connect_button->disable();
	disconnect_button->enable();
	reboot_button->enable();
	reset_button->enable();

	//list report counts
	FXString w, x;
	w.format("hid in report count: %u\n", in_size);
	x = w;
	w.format("hid out report count: %u\n", out_size);
	x += w;
	//list wakeups and alarm and warn if no STM32
	FXString u;
	for(int i = 0; i < wakeupslots; i++) {
		FXString t, v;
#if (FOX_MINOR >= 7)
		t.fromInt(i,10);
#else
		t = FXStringVal(i,10);
#endif
		s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_WAKE);
		s += t;
		output_text->setText(s);
		Write_and_Check(5, 10);
		s = (i < wakeupslots-1) ? "wakeup: " : "reboot: ";
		t.format("%02x", (unsigned int)buf[4]);
		v = t;
		t.format("%02x", (unsigned int)buf[6]);
		v += t;
		t.format("%02x", (unsigned int)buf[5]);
		v += t;
		t.format("%02x", (unsigned int)buf[8]);
		v += t;
		t.format("%02x", (unsigned int)buf[7]);
		v += t;
		t.format("%02x", (unsigned int)buf[9]);
		v += t;
		s += v;
		s += "\n";
		if(v != "ffffffffffff") {
			u += s;
		}
	}
	s.format("%d %d %d %d", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_ALARM);
	output_text->setText(s);
	Write_and_Check(4, 8);
	unsigned int alarm = *((uint32_t *)&buf[4]);
	FXString t;	
	s = "alarm: ";
	t.format("%u", alarm/60/60/24);
	s += t;
	s += " days, ";
	t.format("%d", (alarm/60/60) % 24);
	s += t;
	s += " hours, ";
	t.format("%d", (alarm/60) % 60);
	s += t;
	s += " minutes, ";
	t.format("%d", alarm % 60);
	s += t;
	s += " seconds\n";
	if(uC != "STM32"){
		s += "WARNING: This device's microcontroller is a ";
		s += uC;
		s += ", NOT a STM32!\n";
	}
	input_text->setText("");
	output_text->setText("");
	input_text->appendText(u);
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);

	hid_set_nonblocking(connected_device, 1);

	return 1;
}

long
MainWindow::onDisconnect(FXObject *sender, FXSelector sel, void *ptr)
{
	if (!connected_device)
		return 1;
	if(ReadIRcontActive)
		onReadIRcont(NULL, 0, NULL);
	hid_close(connected_device);
	connected_device = NULL;
	connected_label->setText("Disconnected");
	connected_label2->setText("Firmware:");
	connected_label3->setText("Protocols:");
	protocols = "";
	firmware = "";
	firmware1 = "";
	max = 0;
	count = 0;
	wslistbox->clearItems();
	mnlistbox->clearItems();
	mslistbox->clearItems();
	output_button->disable();
	pwakeup_button->disable();
	pmacro_button->disable();
	prwakeup_button->disable();
	prmacro_button->disable();
	gwakeup_button->disable();
	gmacro_button->disable();
	gcap_button->disable();
	aget_button->disable();
	aset_button->disable();
	rwakeup_button->disable();
	rmacro_button->disable();
	ralarm_button->disable();
	send_button->disable();
	read_cont_button->disable();
	connect_button->enable();
	disconnect_button->disable();
	reboot_button->disable();
	reset_button->disable();
	getApp()->removeTimeout(this, ID_TIMER);
	getApp()->removeTimeout(this, ID_READIR_TIMER);
	getApp()->removeTimeout(this, ID_RED_TIMER);

	return 1;
}

long
MainWindow::onRescan(FXObject *sender, FXSelector sel, void *ptr)
{
	// the selected device's position in the list may change, so make a new onConnect() mandatory
	onDisconnect(NULL, 0, NULL);

	struct hid_device_info *cur_dev;

	device_list->clearItems();
	
	// List the Devices
	hid_free_enumeration(devices);
	devices = hid_enumerate(0x1209, 0x4444);
	cur_dev = devices;	
	while (cur_dev) {
		// Add it to the List Box.
		FXString s;
		s.format("%04hx:%04hx -", cur_dev->vendor_id, cur_dev->product_id);
		s += FXString(" ") + cur_dev->manufacturer_string;
		s += FXString(" ") + cur_dev->product_string;
		FXListItem *li = new FXListItem(s, NULL, cur_dev);
		device_list->appendItem(li);

		cur_dev = cur_dev->next;
	}

	if (device_list->getNumItems() == 0) {
		device_list->appendItem("*** No Devices Connected ***");
		num_devices_after_rescan = 0;
	}
	else {
		device_list->selectItem(0);
		num_devices_after_rescan = device_list->getNumItems();
	}

	return 1;
}

long
MainWindow::onReboot(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXint success = 1;
	s.format("%d %d %d %d", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_REBOOT);
	output_text->setText(s);

	FXint cur_item = device_list->getCurrentItem();
	FXint num_devices_before_reboot = device_list->getNumItems();
	Write_and_Check(4, 4);
	FXThread::sleep(1500000000); // 1,5 s
	do { // wait for device to reappear
		FXThread::sleep(100000000); // 100 ms
		onRescan(NULL, 0, NULL);
		count++;
		if(count > 30) {
			printf("stopped waiting\n");
			success = 0;
			break;
		}
	} while(num_devices_after_rescan != num_devices_before_reboot);
	if(success) {
		device_list->setCurrentItem(cur_item);
		device_list->deselectItem(0);
		device_list->selectItem(cur_item);
		onConnect(NULL, 0, NULL);
	}

	return 1;
}

size_t
MainWindow::getDataFromTextField(FXTextField *tf, uint8_t *buf, size_t len)
{
	const char *delim = " ,{}\t\r\n";
	FXString data = tf->getText();
	const FXchar *d = data.text();
	size_t i = 0;
	
	// Copy the string from the GUI.
	size_t sz = strlen(d);
	char *str = (char*) malloc(sz+1);
	strcpy(str, d);
	
	// For each token in the string, parse and store in buf[].
	char *token = strtok(str, delim);
	while (token) {
		char *endptr = NULL;
		long int val = strtol(token, &endptr, 16); // hex!
		buf[i++] = val;
		if (i > len) {
			FXMessageBox::error(this, MBOX_OK, "Invalid length", "Data field is too long.");
			break;
		}
		token = strtok(NULL, delim);
	}
	
	free(str);
	return i;
}

long
MainWindow::Read(int show_len)
{
	memset(buf, 0, sizeof(buf));
	FXString s;
	if (!connected_device) {
		FXMessageBox::error(this, MBOX_OK, "Device Error R", "Unable To Connect to Device");
		s = "Unable To Connect to Device R\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	int res = hid_read(connected_device, buf, in_size); // nonblocking, must read full length!
	
	if (res < 0) {
		FXMessageBox::error(this, MBOX_OK, "Error Reading", "Could not read from device. Error reported was: %ls", hid_error(connected_device));
		onRescan(NULL, 0, NULL);
		input_text->appendText("read error\n");
		input_text->setBottomLine(INT_MAX);
		return -1;
	} else {
		if (res == 0)
			return 0;

		s.format("Received %d bytes:\n", res);
		for (int i = 0; i < show_len; i++) {
			FXString t;
			t.format("%02x ", (unsigned int)buf[i]);
			s += t;
		}
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
	}

	return 1;
}

long
MainWindow::onReadIR(FXObject *sender, FXSelector sel, void *ptr)
{
	int read;
	read = Read(7);
	if(read == -1)
		return -1;
	else if (read == 0)
		return 0;
	else {
		if(reduce_timeout) {
			reduce_timeout = 0;
			getApp()->addTimeout(this, ID_READIR_TIMER, 1 * timeout_scalar /*1ms*/);
		}
	}

	FXString s;
	FXString t;

	if (buf[0] == REPORT_ID_IR) {
		// Repeat Counter
		if(ReadIRcontActive) {
			if (!buf[6]) {
				RepeatCounter = 0;
			} else {
				RepeatCounter++;
			}
			FXString u;
			u.format("RepeatCounter: %d \n", RepeatCounter);
			input_text->appendText(u);
			input_text->setBottomLine(INT_MAX);
			read_cont_button->setBackColor(FXRGB(255,23,23));
			g_main_window->repaint();
			getApp()->addTimeout(this, ID_RED_TIMER, 50 * timeout_scalar /*50ms*/); // three refreshes at 60Hz
		}

		// show received IR
		s = "";
		t.format("%02hhx", buf[1]);
		s += t;
		protocol1_text->setText(s);
		
		s = ""; t = "";
		t.format("%02hhx", buf[3]);
		s += t; t = "";
		t.format("%02hhx", buf[2]);
		s += t;
		address1_text->setText(s);

		s = ""; t = "";
		t.format("%02hhx", buf[5]);
		s += t; t = "";
		t.format("%02hhx", buf[4]);
		s += t;
		command1_text->setText(s);

		s = ""; t = "";
		t.format("%02hhx", buf[6]);
		s += t;
		flag1_text->setText(s);

		//translate by map and show
		int k = 0;
		t = protocol1_text->getText();
		t += address1_text->getText();
		t += command1_text->getText();
		t += "00";
		s = "translated:";
		map_text21->killHighlight();
		map_text->killHighlight();
		//map_text->setText("");
		for(int i = 0; i < active_lines; i++) {
			if(map[i*2] == t) {
				s += " ";
				s += map[i*2+1];
				map_text->setText(map[i*2+1]);
				map_text->setHighlight(0, map_text->getLength());
				k++;
				map_text21->setHighlight(mapbeg[i], mapbeg[i+1] - mapbeg[i] - 1);
			}
		}
		if(k > 1)
			s += ", WARNING: multiple entries!";
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);		
	}

	return 1;
}

long
MainWindow::onReadIRcont(FXObject *sender, FXSelector sel, void *ptr)
{
	if (!ReadIRcontActive) {
		output_button->disable();
		pwakeup_button->disable();
		pmacro_button->disable();
		prwakeup_button->disable();
		prmacro_button->disable();
		gwakeup_button->disable();
		gmacro_button->disable();
		gcap_button->disable();
		aget_button->disable();
		aset_button->disable();
		rwakeup_button->disable();
		rmacro_button->disable();
		ralarm_button->disable();
		send_button->disable();
		/* consume IR */
		int read;
		read = Read(7);
		while(read > 0)
			read = Read(7);
		// timer on
		getApp()->addTimeout(this, ID_TIMER, 5 * timeout_scalar /*5ms*/);
		ReadIRcontActive = 1;
		read_cont_button->setBackColor(FXRGB(255,207,207));
		read_cont_button->setBaseColor(FXRGB(0,0,255));
		read_cont_button->setShadowColor(makeShadowColor(FXRGB(0,0,255)));
		g_main_window->repaint();
		FXString s;
		if (!ReadIRActive) {
			s = "receive IR data by pressing buttons on the remote control\n";
			s += "stop receive mode by pressing receive mode button again\n";
		}
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		RepeatCounter = 0;
  	} else {
		// timer off
		getApp()->removeTimeout(this, ID_TIMER);
		getApp()->removeTimeout(this, ID_RED_TIMER);
		output_button->enable();
		pwakeup_button->enable();
		pmacro_button->enable();
		prwakeup_button->enable();
		prmacro_button->enable();
		gwakeup_button->enable();
		gmacro_button->enable();
		gcap_button->enable();
		aget_button->enable();
		aset_button->enable();
		rwakeup_button->enable();
		rmacro_button->enable();
		ralarm_button->enable();
		send_button->enable();
		ReadIRcontActive = 0;
		read_cont_button->setBaseColor(storedBaseColor);
		read_cont_button->setShadowColor(storedShadowColor);
		read_cont_button->setBackColor(storedBackColor);
		g_main_window->repaint();
	}

	return 1;
}

long
MainWindow::Write(int out_len)
{
	FXString s;
	memset(bufw, 0, sizeof(bufw));
	FXint output_text_len = getDataFromTextField(output_text, bufw, sizeof(bufw));
	if(out_len > output_text_len)
		out_len = output_text_len;

	if (!connected_device) {
		FXMessageBox::error(this, MBOX_OK, "Device Error W", "Unable to Connect to Device");
		s = "Unable to Connect to Device W\n";//
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	int res = hid_write(connected_device, (const unsigned char*)bufw, out_len); // may write arbitrary length
	if (res < 0) {
		FXMessageBox::error(this, MBOX_OK, "Error Writing", "Could not write to device. Error reported was: %ls", hid_error(connected_device));
		input_text->appendText("write error\n");
		input_text->setBottomLine(INT_MAX);
		onRescan(NULL, 0, NULL);
		return -1;
	} else {
		s.format("Sent %d bytes:\n", res);
		for (int i = 0; i < out_len; i++) {
			FXString t;
			t.format("%02hhx ", bufw[i]);
			s += t;
		}
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
	}
	
	return 1;
}

long
MainWindow::Write_and_Check(int out_len, int show_len)
{
	FXString s;
	int read, count = 0;
	s = "";
	if(Write(out_len) == -1) {
		s += "W&C Write(): -1\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	FXThread::sleep(3000000); // 3ms

	read = Read(show_len); // nonblocking
	if(read  == -1) {
		s += "W&C first Read(): -1\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	while ((buf[0] == REPORT_ID_IR || read == 0) && count < 5000) { // 5000ms needed in case of "set by remote", this is done by a timer
		read = Read(show_len);
		if(read == -1) {
			s += "W&C loop Read(): -1\n";
			input_text->appendText(s);
			input_text->setBottomLine(INT_MAX);
			return -1;
		}
		count++;
		FXThread::sleep(1000000); // 1ms
	}

	if((buf[0] == REPORT_ID_CONFIG_IN) && (buf[1] == STAT_SUCCESS) && (buf[2] == bufw[2]) && (buf[3] == bufw[3])) {
		s += "************************OK***************************\n";	
	} else {
		s += "**********************ERROR**************************\n";
	}
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onSendOutputReport(FXObject *sender, FXSelector sel, void *ptr)
{
	Write_and_Check(12, 10); // maximum outgoing protocol is 12, maximum incoming protocol is 10

	return 1;
}

long
MainWindow::onPwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
	t.format("%d ", wslistbox->getCurrentItem());
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_WAKE);
	s += t;
	t = protocol_text->getText();
	len = t.length(); // don't put this into the for loop!!!
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	t = address_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = flag_text->getText();
	len = t.length();
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	output_text->setText(s);

	Write_and_Check(11, 4);

	return 1;
}

long
MainWindow::onPmacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
	t.format("%d ", mnlistbox->getCurrentItem());
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_MACRO);
	s += t;
	t.format("%d ", mslistbox->getCurrentItem());
	s += t;
	t = protocol_text->getText();
	len = t.length(); // don't put this into the for loop!!!
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	t = address_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = flag_text->getText();
	len = t.length();
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	output_text->setText(s);

	Write_and_Check(12, 4);

	return 1;
}

long
MainWindow::onPRwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	protocol1_text->setText("");
	address1_text->setText("");
	command1_text->setText("");
	flag1_text->setText("");
	s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);
	ReadIRActive = 1;
	reduce_timeout = 1;
	getApp()->addTimeout(this, ID_READIR_TIMER, 5000 * timeout_scalar /*5s*/);
	onReadIRcont(NULL, 0, NULL);
	ReadIRActive = 0;
	t.format("%d ", wslistbox->getCurrentItem());
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_WAKE);
	s += t;
	output_text->setText(s);

	return 1;
}

long
MainWindow::onPRmacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	protocol1_text->setText("");
	address1_text->setText("");
	command1_text->setText("");
	flag1_text->setText("");
	s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);
	ReadIRActive = 1;
	reduce_timeout = 1;
	getApp()->addTimeout(this, ID_READIR_TIMER, 5000 * timeout_scalar /*5s*/);
	onReadIRcont(NULL, 0, NULL);
	ReadIRActive = 0;
	t.format("%d ", mnlistbox->getCurrentItem());
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_MACRO);
	s += t;
	t.format("%d ", mslistbox->getCurrentItem());
	s += t;
	output_text->setText(s);

	return 1;
}


long
MainWindow::onGwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d", wslistbox->getCurrentItem());
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_WAKE);
	s += t;
	output_text->setText(s);

	Write_and_Check(5, 10);

	s = "";
	t.format("%02x", (unsigned int)buf[4]);
	s += t;
	protocol1_text->setText(s);
		
	s = "";
	t.format("%02x", (unsigned int)buf[6]);
	s += t;
	t.format("%02x", (unsigned int)buf[5]);
	s += t;
	address1_text->setText(s);

	s = "";
	t.format("%02x", (unsigned int)buf[8]);
	s += t;
	t.format("%02x", (unsigned int)buf[7]);
	s += t;
	command1_text->setText(s);

	s = "";
	t.format("%02x", (unsigned int)buf[9]);
	s += t;
	flag1_text->setText(s);

	return 1;
}

long
MainWindow::onGmacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d ", mnlistbox->getCurrentItem());
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_MACRO);
	s += t;
	t.format("%d", mslistbox->getCurrentItem());
	s += t;
	output_text->setText(s);

	Write_and_Check(6, 10);
	
	s = "";
	t.format("%02x", (unsigned int)buf[4]);
	s += t;
	protocol1_text->setText(s);
		
	s = "";
	t.format("%02x", (unsigned int)buf[6]);
	s += t;
	t.format("%02x", (unsigned int)buf[5]);
	s += t;
	address1_text->setText(s);

	s = "";
	t.format("%02x", (unsigned int)buf[8]);
	s += t;
	t.format("%02x", (unsigned int)buf[7]);
	s += t;
	command1_text->setText(s);

	s = "";
	t.format("%02x", (unsigned int)buf[9]);
	s += t;
	flag1_text->setText(s);
	
	return 1;
}

long
MainWindow::onGcaps(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	int jump_to_firmware, romtable;
	jump_to_firmware = 0;
	romtable = 0;
	uC = "";
	for(int i = 0; i < 20; i++) { // for safety stop after 20 loops
		s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_CAPS);
#if (FOX_MINOR >= 7)
		t.fromInt(i,16);
		s += t;
#else
		s += FXStringVal(i,16);
#endif
		s += " ";
		output_text->setText(s);

		Write_and_Check(5, i == 0 ? 9 : in_size);

		if (!i) { // first query for slots and depth
			macroslots = buf[4];
			s.format("macro_slots: %u\n", buf[4]);
			macrodepth = buf[5];
			t.format("macro_depth: %u\n", buf[5]);
			s += t;
			wakeupslots = buf[6];
			t.format("wakeup_slots: %u\n", buf[6]);
			s += t;
			in_size = buf[7] ? buf[7] : 17;
			t.format("hid in report count: %u\n", in_size);
			s += t;
			out_size = buf[8] ? buf[8] : 17;
			t.format("hid out report count: %u\n", out_size);
			s += t;
			if(!buf[7] || ! buf[8])
				s += "old firmware!\n";
		} else {
			if (!jump_to_firmware) { // queries for supported_protocols
				s = "protocols: ";
				for (int k = 4; k < in_size; k++) {
					if (!buf[k]) { // NULL termination
						s += "\n";
						input_text->appendText(s);
						input_text->setBottomLine(INT_MAX);
						jump_to_firmware = 1;
						goto again;
					}
					t.format("%u ", buf[k]);
					protocols += t;
					s += t;
				}
			} else { // queries for firmware
				s = "firmware: ";
				for (int k = 4; k < in_size; k++) {
					if (!buf[k]) { // NULL termination
						s += "\n";
						input_text->appendText(s);
						input_text->setBottomLine(INT_MAX);
						return 1;
					}
					if (buf[k] == 42) { // * separator
						romtable = 1;
						firmware += "   uC: ";
						s += "*";
					} else {
						t.format("%c", buf[k]);
						firmware += t;
						if(romtable)
							uC += t;
						s += t;
					}
				}
			}
		}
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
again:	;
	}
	return 1;
}

long
MainWindow::onAget(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	s.format("%d %d %d %d", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_ALARM);
	output_text->setText(s);

	Write_and_Check(4, 8);

	unsigned int alarm = *((uint32_t *)&buf[4]);

	FXString t;	
	s = "";
	t.format("%u", alarm/60/60/24);
	s += t;
	days1_text->setText(s);
		
	s = "";
	t.format("%d", (alarm/60/60) % 24);
	s += t;
	hours1_text->setText(s);

	s = "";
	t.format("%d", (alarm/60) % 60);
	s += t;
	minutes1_text->setText(s);

	s = "";
	t.format("%d", alarm % 60);
	s += t;
	seconds1_text->setText(s);

	return 1;
}

long
MainWindow::onAset(FXObject *sender, FXSelector sel, void *ptr)
{
	unsigned int setalarm = 0;
	FXString u = "";
#if (FOX_MINOR >= 7)
	setalarm += 60 * 60 * 24 * days_text->getText().toUInt();
	setalarm += 60 * 60 * hours_text->getText().toUInt();
	setalarm += 60 * minutes_text->getText().toUInt();
	setalarm += seconds_text->getText().toUInt();
#else
	setalarm += 60 * 60 * 24 * FXUIntVal(days_text->getText(), 10);
	setalarm += 60 * 60 * FXUIntVal(hours_text->getText(), 10);
	setalarm += 60 * FXUIntVal(minutes_text->getText(), 10);
	setalarm += FXUIntVal(seconds_text->getText(), 10);
#endif
	if(setalarm < 2) {
		setalarm = 2;
		u = "set alarm to 2 in order to prevent device or program hangup\n";
	 }

	FXString s;
	FXString t;
	const char *z = " ";
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_ALARM);
#if (FOX_MINOR >= 7)
	t.fromInt(setalarm,16);
#else
	t = FXStringVal(setalarm, 16);
#endif
	int len = t.length();
	for (int i = 0; i < 8 - len; i++) {
		t.prepend("0");
	}
	t.insert(2, " ");
	t.insert(5, " ");
	t.insert(8, " ");
	s += t.section(z, 3, 1);
	s += " ";
	s += t.section(z, 2, 1);
	s += " ";
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	output_text->setText(s);

	Write_and_Check(8, 4);

	input_text->appendText(u);
	input_text->setBottomLine(INT_MAX);
	
	return 1;
}

long
MainWindow::onRwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d", wslistbox->getCurrentItem());
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_WAKE);
	s += t;
	output_text->setText(s);

	Write_and_Check(5, 4);

	return 1;
}

long
MainWindow::onRmacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d ", mnlistbox->getCurrentItem());
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_MACRO);
	s += t;
	t.format("%d ", mslistbox->getCurrentItem());
	s += t;
	output_text->setText(s);

	Write_and_Check(6, 4);

	return 1;
}

long
MainWindow::onRalarm(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	s.format("%d %d %d %d", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_ALARM);
	output_text->setText(s);

	Write_and_Check(4, 4);

	return 1;
}

long
MainWindow::onSendIR(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_EMIT);
	t = protocol_text->getText();
	len = t.length(); // don't put this into the for loop!!!
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	t = address_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = flag_text->getText();
	len = t.length();
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	output_text->setText(s);

	Write_and_Check(10, 4);

	return 1;
}

long
MainWindow::onUpgrade(FXObject *sender, FXSelector sel, void *ptr)
{
	const FXchar patterns[]="All Files (*)\nFirmware Files (*.bin)";
	FXString s, v, Filename, FilenameText;
	FXFileDialog open(this,"Open a firmware file");
	open.setPatternList(patterns);
	open.setCurrentPattern(1);
	if(open.execute()){
		Filename = open.getFilename();
		FXint pos = Filename.rfind(PATHSEP);
		FXint endpos = Filename.length();
		FXint suffix_length = open.getCurrentPattern() ? 4 : 0;
		FXString Firmwarename = Filename.mid(pos + 1, endpos - pos - 1 - suffix_length);
		if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,"Really upgrade?","Old Firmware: %s\nNew Firmware: %s", firmware1.text(),  Firmwarename.text())) return 1;
		sprintf(printcollect, "%s", "");
#ifndef WIN32
		sprintf(firmwarefile, "%s", Filename.text());
#else
		FXCP1252Codec codec;
		FXString mbstring=codec.utf2mb(Filename); // on Windows file encoding is cp1252, needed for umlaut
		sprintf(firmwarefile, "%s", mbstring.text());
#endif

		doUpgrade.set_firmwarefile(firmwarefile);
		doUpgrade.set_print(print);
		doUpgrade.set_printcollect(printcollect);
		doUpgrade.set_signal(guisignal);
		doUpgrade.start();

		cur_item = device_list->getCurrentItem();
		num_devices_before_upgrade = device_list->getNumItems();
		s.format("%d %d %d %d", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_REBOOT);
		output_text->setText(s);
		if(connected_device)
			Write_and_Check(4, 4);
		onDisconnect(NULL, 0, NULL);
	}

	return 1;
}

long
MainWindow::onPrint(FXObject *sender, FXSelector sel, void *ptr)
{
		FXint success = 1;
		FXString t = print;
		input_text->appendText(t);
		input_text->setBottomLine(INT_MAX);
		if(t == "=== Firmware Upgrade successful! ===\n"){
			int count = 0;
			do { // wait for device to reappear
				FXThread::sleep(100000000); // 100 ms
				onRescan(NULL, 0, NULL);
				count++;
				if(count > 20) {
					printf("stopped waiting\n");
					success = 0;
					break;
				}
			} while(num_devices_after_rescan != num_devices_before_upgrade);
			if(success){
				device_list->setCurrentItem(cur_item);
				device_list->deselectItem(0);
				device_list->selectItem(cur_item);
				onConnect(NULL, 0, NULL);
			} else
				input_text->setText("");
			FXString u = printcollect;
			input_text->appendText(u);
			input_text->setBottomLine(INT_MAX);
		}

	return 1;
}

long
MainWindow::onOpen(FXObject *sender, FXSelector sel, void *ptr)
{
	if(map_text21->isModified()){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("map was changed"),tr("Discard changes to map?"))==MBOX_CLICKED_NO) return 1;
	}
	const FXchar patterns[]="All Files (*)\nmap Files (*.map)";
	long loaded = 0;
	FXint size = 0;
	FXint n;
	FXFileDialog open(this,"Open a map file");
	open.setPatternList(patterns);
	open.setCurrentPattern(1);
	if(open.execute()){
		map_text21->setText(NULL,0);
		FXString file=open.getFilename();
		FXFile textfile(file,FXFile::Reading);
		// Opened file?
		if(textfile.isOpen()){
			FXchar *text;

			// Get file size
			size=textfile.size();

			// Make buffer to load file
			if(allocElms(text,size)){

				// Set wait cursor
				getApp()->beginWaitCursor();

				// Read the file
				n=textfile.readBlock(text,size);
				if(0<=n){

					// Set text
					map_text21->setText(text,n);

					// Success
					loaded=1;
				}

				// Kill wait cursor
				getApp()->endWaitCursor();

				// Free buffer
				freeElms(text);
			}
		}

		map_text21->setModified(0);
		FXString u;
		FXString v;
		u = "opened: ";
		v = open.getFilename().text();
		u += v;
		u += "\n";
		u += "opened size: ";
#if (FOX_MINOR >= 7)
		v.fromInt(size, 10);
#else
		v = FXStringVal(size, 10);
#endif
		u += v;
		u += "\n";
		input_text->appendText(u);
		input_text->setBottomLine(INT_MAX);
		onApply(NULL, 0, NULL);
    }

	return loaded;
}

long
MainWindow::onSave(FXObject *sender, FXSelector sel, void *ptr){
	const FXchar patterns[]="All Files (*)\nmap Files (*.map)";
	FXFileDialog save(this,"save the map file");
	FXString file;
	save.setPatternList(patterns);
	save.setCurrentPattern(1);
	if(save.execute()){
		file=save.getFilename();
		if(compare(file.right(4), ".map") && (save.getCurrentPattern() == 1))
			file += ".map";
		if(FXStat::exists(file)){
			if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,tr("Overwrite Document"),tr("Overwrite existing document: %s?"),file.text())) return 1;
		}
		if(!saveFile(file)){
			getApp()->beep();
			FXMessageBox::error(this,MBOX_OK,tr("Error Saving File"),tr("Unable to save file: %s"),file.text());
			return 1;
		}
		map_text21->setModified(0);
		FXString u;
		u = "save map to ";
		u += file;
		u += "\n";
		input_text->appendText(u);
		input_text->setBottomLine(INT_MAX);
    }

	return 1;
}

long
MainWindow::onSaveLog(FXObject *sender, FXSelector sel, void *ptr){
	const FXchar patterns[]="All Files (*)\nlog Files (*.log)";
	FXFileDialog save(this,"save the log file");
	FXString file;
	save.setPatternList(patterns);
	save.setCurrentPattern(1);
	if(save.execute()){
		file=save.getFilename();
		if(compare(file.right(4), ".log") && (save.getCurrentPattern() == 1))
			file += ".log";
		if(FXStat::exists(file)){
			if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,tr("Overwrite Document"),tr("Overwrite existing document: %s?"),file.text())) return 1;
		}
		if(!saveLogFile(file)){
			getApp()->beep();
			FXMessageBox::error(this,MBOX_OK,tr("Error Saving File"),tr("Unable to save file: %s"),file.text());
			return 1;
		}
		FXString u;
		u = "save debug messages to ";
		u += file;
		u += "\n";
		input_text->appendText(u);
		input_text->setBottomLine(INT_MAX);
    }

	return 1;
}

long 
MainWindow::saveFile(const FXString& file){
	FXFile textfile(file,FXFile::Writing);
	long saved=0;

	// Opened file?
	if(textfile.isOpen()){
		FXchar *text; FXint size,n;

		// Get size
		size=map_text21->getLength();

		// Alloc buffer
		if(allocElms(text,size+1)){

			// Set wait cursor
			getApp()->beginWaitCursor();

			// Get text from editor
			map_text21->getText(text,size);

			// Write the file
			n=textfile.writeBlock(text,size);
			if(n==size){

				// Success
				saved=1;
			}

			// Kill wait cursor
			getApp()->endWaitCursor();

			// Free buffer
			freeElms(text);

			FXString u;
			FXString v;
			u = "saved size: ";
#if (FOX_MINOR >= 7)
			v.fromInt(size, 10);
#else
			v = FXStringVal(size, 10);
#endif
			u += v;
			u += "\n";
			input_text->appendText(u);
			input_text->setBottomLine(INT_MAX);
		}
	}
	return saved;
}

long
MainWindow::saveLogFile(const FXString& file){
	FXFile textfile(file,FXFile::Writing);
	long saved=0;

	// Opened file?
	if(textfile.isOpen()){
		FXchar *text; FXint size,n;

		// Get size
		size=input_text->getLength();

		// Alloc buffer
		if(allocElms(text,size+1)){

			// Set wait cursor
			getApp()->beginWaitCursor();

			// Get text from debug messages field
			input_text->getText(text,size);

			// Write the file
			n=textfile.writeBlock(text,size);
			if(n==size){

				// Success
				saved=1;
			}

			// Kill wait cursor
			getApp()->endWaitCursor();

			// Free buffer
			freeElms(text);

			FXString u;
			FXString v;
			u = "saved size: ";
#if (FOX_MINOR >= 7)
			v.fromInt(size, 10);
#else
			v = FXStringVal(size, 10);
#endif
			u += v;
			u += "\n";
			input_text->appendText(u);
			input_text->setBottomLine(INT_MAX);
		}
	}
	return saved;
}

long
MainWindow::onAppend(FXObject *sender, FXSelector sel, void *ptr){
	FXString s;
	FXString t;
	s = protocol1_text->getText();
	t = address1_text->getText();
	s += t;
	t = command1_text->getText();
	s += t;
	s += "00";
	s += " ";
	s += map_text->getText();
	s += "\n";
	map_text21->appendText(s);
	onApply(NULL, 0, NULL);
	map_text21->setModified(1);

	return 1;
}

long
MainWindow::onReeprom(FXObject *sender, FXSelector sel, void *ptr){
	if(map_text21->isModified()){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("map was changed"),tr("Discard changes to map?"))==MBOX_CLICKED_NO) return 1;
	}
	if(FXMessageBox::question(this,MBOX_YES_NO,tr("reset eeprom"),tr("really reset eeprom?"))==MBOX_CLICKED_NO) return 1;

	FXString s;
	s.format("%d %d %d %d ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_EEPROM_RESET);

	output_text->setText(s);

	Write_and_Check(4, 4);

	return 1;
}

long
MainWindow::onApply(FXObject *sender, FXSelector sel, void *ptr){
	// fill map[] and mapbeg[]
	const char *delim = " \t\r\n"; // Space, Tab, CR and LF
	FXString data = map_text21->getText();
	const FXchar *d = data.text();
	size_t k = 0;
	size_t sz = strlen(d);
	char *str = (char*) malloc(sz+1);
	strcpy(str, d);
	char *token = strtok(str, delim);
	memset(mapbeg, 0, sizeof(mapbeg));
	int count = 0;
	while (token) {
		map[k++] = token;
		count += map[k-1].length() + 1;
		if(!(k%2))
			mapbeg[(k+1)/2] = count;
		token = strtok(NULL, delim);
	}
	free(str);
	active_lines = k / 2;

	FXString u;
	FXString v;
	u = "map mapbeg: \n";
	for(int i = 0; i < active_lines; i++) {
		v = map[i*2];
		u += v;
		u += " ";
		v = map[i*2+1];
		u += v;
		u += " ";
#if (FOX_MINOR >= 7)
		v.fromInt(mapbeg[i], 10);
#else
		v = FXStringVal(mapbeg[i],10);
#endif
		u += v;
		u += "\n";
	}
	input_text->appendText(u);
	input_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onWrite_IR(FXObject *sender, FXSelector sel, void *ptr)
{
	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	map_text21->removeText(mapbeg[i-1], map[(i-1)*2].length());
	FXString s;
	s = protocol1_text->getText();
	s += address1_text->getText();
	s += command1_text->getText();
	s += "00";
	map_text21->insertText(mapbeg[i-1], s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i]);
	map_text21->setModified(1);

	return 1;
}

long
MainWindow::onClear(FXObject *sender, FXSelector sel, void *ptr)
{
	input_text->setText("");

	return 1;
}

long
MainWindow::onTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
	if(onReadIR(NULL, 0, NULL) != -1)
		getApp()->addTimeout(this, ID_TIMER, 5 * timeout_scalar /*5ms*/);

	return 1;
}

long
MainWindow::onReadIRTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	FXString u;
	const char *z = " ";

	// toggle onReadIRcont off
	if (ReadIRcontActive)
		onReadIRcont(NULL, 0, NULL);

	u = "";
	u += output_text->getText();
	u += " ";
	s = "";
	s += protocol1_text->getText();
	s += " ";
	t = address1_text->getText();
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command1_text->getText();
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " 00 ";
	if (s=="     00 "){
		FXMessageBox::error(this, MBOX_OK, "IR Read Error", "No IR received");
		return 0;
	}
	u += s;
	output_text->setText(u);

	Write_and_Check(12, 4);

	return 1;
}

long
MainWindow::onRedTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
	read_cont_button->setBackColor(FXRGB(255,207,207));
	g_main_window->repaint();

	return 1;
}

long MainWindow::onCmdwsListBox(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,wslistbox->getCurrentItem()));
	return 1;
  }

long MainWindow::onCmdmnListBox(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,mnlistbox->getCurrentItem()));
	return 1;
  }

long MainWindow::onCmdmsListBox(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,mslistbox->getCurrentItem()));
	return 1;
  }

long MainWindow::onDevDClicked(FXObject *sender, FXSelector sel, void *ptr){
	onDisconnect(NULL, 0, NULL);
	onConnect(NULL, 0, NULL);
	return 1;
}

long
MainWindow::onMacTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
#ifdef __APPLE__
	check_apple_events();
	
	getApp()->addTimeout(this, ID_MAC_TIMER,
		50 * timeout_scalar /*50ms*/);
#endif

	return 1;
}

int main(int argc, char **argv)
{
	FXApp app("IRMP STM32 Configuration", "");
	app.init(argc, argv);
	g_main_window = new MainWindow(&app);
	app.create();
	app.run();
	return 0;
}

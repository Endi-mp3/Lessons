#pragma once


struct MenuNetwork
{
	int Port;
	int IP;
	MyLibMenu *Network;
    MyLibMenu *WIFI;
    MyLibMenu *WIFIconnect;
    int WifiName;
    int WifiSid;
};

struct MenuButtonSlot
{
	MyLibMenu *ButtonSlot;
	int SlotName;
	int SlotBtnStart;
};

struct MenuTrigerSlot
{
	MyLibMenu *TrigerSlot;
	int TrigerBtn;
	int TrigerBtnStart;
};


struct MenuTrigerSlotCheckBox
{
	int mTrgCbCount;
	int mTrgCbArray[256];
};

struct MenuResetingDevice
{
	MyLibMenu *ResetDevice;
	int FullReset;
	int CleanSlot;
};

struct MenuSettingsPayload
{
	MyLibMenu *SettingsPayload;
	int SettingCmd;
	int SettingData;
	int SettingSend;
};


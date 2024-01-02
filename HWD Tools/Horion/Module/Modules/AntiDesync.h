#pragma once
#include "Module.h"
class AntiDesync : public IModule {
private:
	//bool noFunni = false;
	bool keyPressed = false;

public:
	SettingEnum mode = this;

	AntiDesync();
	~AntiDesync();

	virtual void onSendPacket(C_Packet* packet) override;
	virtual const char* getRawModuleName() override;
	virtual const char* getModuleName() override;
};

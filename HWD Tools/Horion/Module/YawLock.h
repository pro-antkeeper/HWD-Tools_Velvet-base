#pragma once
#include "../ModuleManager.h"
#include "Module.h"
class yawLock : public IModule {
public:
	SettingEnum Direction;
	SettingEnum Mode;
	yawLock();
	~yawLock();
	virtual const char* getModuleName() override;
	virtual void onPlayerTick(C_Player* plr);
};
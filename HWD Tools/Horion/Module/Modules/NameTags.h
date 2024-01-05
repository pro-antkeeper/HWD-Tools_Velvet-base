#pragma once
#include "../../../Utils/TargetUtil.h"
#include "../ModuleManager.h"
#include "../../../Horion/DrawUtils.h"
#include "Module.h"

class NameTags : public IModule {
public:
	std::set<std::string> nameTags;
	bool displayArmor = true;
	bool displayDurability = true;
	bool drawhealth = true;
	//bool underline = true;
	bool hideTags = false;
	bool fsync = false;
	float scalen = 0.6f;
	float fred = 0.f;
	float fgreen = 255.f;
	float fblue = 255.f;
	int opacity = 150;

	bool* ingameNametagSetting = nullptr;
	bool gotPrevSetting = false;
	bool lastSetting = true;

	SettingEnum renderMode = this;

	virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx);
	virtual const char* getModuleName();
	virtual void onDisable();
	NameTags();
};

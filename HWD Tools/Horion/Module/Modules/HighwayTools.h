#pragma once
#include <vector>
#include "../../../Utils/HMath.h"
#include "../../DrawUtils.h"
#include "../ModuleManager.h"
#include "Module.h"
class HighwayTools : public IModule {
private:
	vec3_ti startBuildPos;
	SettingEnum block = SettingEnum(this);
	SettingEnum axis = SettingEnum(this);
	std::vector<vec3_ti> currentSchematic;
	std::vector<vec3_ti> baseSchematic;
	int oldAutoBuildSlot = 0;
	bool setAutoBuildSlot = false;
	int visualrange = 15;
	int delayticks = 0;
	bool replenish = true;
	float totalrange = 8;
	float buildrange = 4.5;
	bool autoswitch = true;
	bool fpsmode = false;
	bool visualize = true;
	bool pressed;
	bool autocenter = false;
	bool rotationlock = false;

public:
	static std::unordered_map<int, std::string> initializeModeNames();
	void initializeSchematic();
	std::vector<vec3_ti> getCurrentSchematic(vec3_ti currentPos);
	HighwayTools();
	~HighwayTools();
	virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx) override;
	virtual void onEnable() override;
	virtual void onDisable() override;
	void onLevelRender() override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onMove(C_MoveInputHandler* input) override;
	virtual void onPlayerTick(C_Player* plr);
	virtual const char* getModuleName() override;
	void setStartBuildPos(const vec3_ti& startPos) {
		startBuildPos = startPos;
	}
};

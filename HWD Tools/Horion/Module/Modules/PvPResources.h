#pragma once
#include "Module.h"
class pvpResources : public IModule {
public:
	pvpResources();
	~pvpResources();

	float scale = 1.f;
	float pvpX = 0.00f;
	float pvpY = 340.84f;
	bool text = true;

	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx) override;
	virtual bool isThereCrystal();
	virtual bool isThereGapples();
	virtual bool isThereTotem();
	virtual bool isThereObsidian();
	virtual bool isThereEChest();
	virtual bool isThereTNT();
	virtual bool isThereMelons();
	int pullSlot(int itemIDPog);
	virtual int countItem(int itemIDCount);
};

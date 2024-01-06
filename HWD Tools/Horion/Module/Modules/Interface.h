#pragma once
#include "../ModuleManager.h"
#include "../../Horion/DrawUtils.h"
#include "Module.h"

class Interface : public IModule
{
public:
	std::string versionStr = "0.4.2";

public:
	// Settings
	bool keystrokes = false;
	float saturation = 1.f;
	float brightness = 1.f;
	bool tabGUI = false;
	int opacity = 150;
	float coloropacity = 255.f;
	int glowlayers = 12;
	float glowopacity = 0.75;
	int spacing = 70;

	float scale = 1.f;

	//new shit
	//static MC_Color waveColor(int red, int green, int blue, int red2, int green2, int blue2, long index);
	static MC_Color rainbowColor(float seconds, float saturation, float brightness, long index);
	static MC_Color astolfoRainbow(int yOffset, int yTotal);
	static MC_Color interfaceColor(int index);

	// Positions
	vec2_t windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;

	// Colors
	SettingEnum color = this;
	SettingEnum Fonts = this;
	float r = 255.f, g = 0.f, b = 0.f;
	float r2 = 150.f, g2 = 0.f, b2 = 0.f;

	virtual void onPostRender(C_MinecraftUIRenderContext *renderCtx);
	virtual void onPreRender(C_MinecraftUIRenderContext *renderCtx);
	virtual const char *getModuleName();
	// virtual void onTick(C_GameMode* gm);
	virtual void onDisable();
	Interface();
};
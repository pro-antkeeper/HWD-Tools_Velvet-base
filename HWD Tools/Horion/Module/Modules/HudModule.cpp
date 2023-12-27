#include "HudModule.h"
#include "../../DrawUtils.h"
#include "../../Scripting/ScriptManager.h"

HudModule::HudModule() : IModule(0, Category::HUD, "Displays ArrayList/TabGUI") {
	registerBoolSetting("TabGui", &tabgui, tabgui);
	//registerBoolSetting("ArrayList", &arraylist, arraylist);
	registerBoolSetting("ClickToggle", &clickToggle, clickToggle);
	//registerBoolSetting("Watermark", &watermark, watermark);
	//registerBoolSetting("Coordinates", &coordinates, coordinates);
	registerBoolSetting("Always show", &alwaysShow, alwaysShow);
	//registerFloatSetting("Scale", &scale, scale, 0.5f, 1.5f);
}

HudModule::~HudModule() {
}

const char* HudModule::getModuleName() {
	return ("HUD");
}

void HudModule::onPostRender(C_MinecraftUIRenderContext* renderCtx) {

	vec2_t windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;
	float f = 10.f * scale;
	std::string tempStr("Movement");
	float len = DrawUtils::getTextWidth(&tempStr, scale) + 7.f;
	float startY = tabgui ? 6 * f : 0.f;
	if(tabgui && scriptMgr.getNumEnabledScripts() > 0)
		startY += f;
}
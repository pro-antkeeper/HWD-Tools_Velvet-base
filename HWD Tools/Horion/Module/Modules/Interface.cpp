#include "Interface.h"
#include "../../../pch.h"
#include "../../../Utils/TimerUtil.h"

Interface::Interface() : IModule(0, Category::HUD, "Change the color of the Client ~by #Team GameSense")
{
	registerEnumSetting("Mode", &color, 2);
	color.addEntry(EnumEntry("RGB", 0));
	color.addEntry(EnumEntry("Astolfo", 1));
//	color.addEntry(EnumEntry("Wave", 2)); //broken for now (code cant see wich option is best so it shits itself smh)
	//color.addEntry("RGBWave", 3);
	//registerFloatSetting("Brightness", &brightness, brightness, 0.f, 1.f);
	registerFloatSetting("Saturation", &saturation, saturation, 0.f, 1.f);
	registerIntSetting("Spacing", &spacing, spacing, 1, 200);
	registerFloatSetting("Opacity", &coloropacity, coloropacity, 0.f, 255.f);
	registerFloatSetting("R", &r, r, 0.f, 255.f);
	registerFloatSetting("G", &g, g, 0.f, 255.f);
	registerFloatSetting("B", &b, b, 0.f, 255.f);
	registerFloatSetting("R2", &r2, r2, 0.f, 255.f);
	registerFloatSetting("G2", &g2, g2, 0.f, 255.f);
	registerFloatSetting("B2", &b2, b2, 0.f, 255.f);
	//shouldHide = true;
}

const char *Interface::getModuleName()
{
	return ("Colors");
}

void Interface::onPreRender(C_MinecraftUIRenderContext *renderCtx)
{
	auto clickGUIh = moduleMgr->getModule<ClickGuiMod>();
	auto playerh = g_Data.getLocalPlayer();
	if (playerh == nullptr)
		return;
}

void Interface::onPostRender(C_MinecraftUIRenderContext *renderCtx)
{
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr)
		return;

	auto interfaceCL = Interface::interfaceColor(1);
	auto clickGUI = moduleMgr->getModule<ClickGuiMod>();

	static int index = 0;
	int curIndex = -index * spacing;
	auto hudColor = Interface::interfaceColor(curIndex);
	windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;
}

void Interface::onDisable()
{
	setEnabled(true);
}

MC_Color Interface::interfaceColor(int index)
{
	auto i = moduleMgr->getModule<Interface>();
	auto color = Interface::rainbowColor(8, 1.F, 1.F, i->coloropacity);

	switch (i->color.selected)
	{
	case 0:
		color = Interface::rainbowColor(8, i->saturation, 1.F, -index * 4.f);
		break; /* Rainbow */
	case 1:
		color = Interface::astolfoRainbow(index / 5, 1000);
		break; /* Astolfo */
/*	case 2:
		color = Interface::waveColor(i->r, i->g, i->b, i->r2, i->g2, i->b2, index * 3.f);
		break; /* Wave */
	}
	return MC_Color(color);
}

MC_Color Interface::rainbowColor(float seconds, float saturation, float brightness, long index)
{
	auto interface = moduleMgr->getModule<Interface>();
	float hue6 = (((TimerUtil::getCurrentMs() + index) % (int)(6000)) / (float)(6000));
	float r, g, b = 0;
	Utils::ColorConvertHSVtoRGB(hue6, saturation, brightness, r, g, b);

	return MC_Color(r * 255, g * 255, b * 255, interface->coloropacity);
}

/*MC_Color Interface::waveColor(int red, int green, int blue, int red2, int green2, int blue2, long index)
{
	auto interface = moduleMgr->getModule<Interface>();
	double offset = ((TimerUtil::getCurrentMs() - index) / 8) / (double)120;
	double aids123 = ((TimerUtil::getCurrentMs() - index) % 1000 / 1000.000);
	int aids1234 = ((TimerUtil::getCurrentMs() - index) % 2000 / 2000.000) * 2;
	aids123 = aids1234 % 2 == 0 ? aids123 : 1 - aids123;
	double inverse_percent = 1 - aids123;
	int redPart = (int)(red * inverse_percent + red2 * aids123);
	int greenPart = (int)(green * inverse_percent + green2 * aids123);
	int bluePart = (int)(blue * inverse_percent + blue2 * aids123);

	return MC_Color(redPart, greenPart, bluePart, interface->coloropacity);
} */

MC_Color Interface::astolfoRainbow(int yOffset, int yTotal)
{
	auto interface = moduleMgr->getModule<Interface>();
	float speed = 3000;
	float hue = (float)(TimerUtil::getCurrentMs() % (int)speed) + ((yTotal - yOffset) * 9);
	while (hue > speed)
	{
		hue -= speed;
	}
	hue /= speed;
	if (hue > 0.5)
	{
		hue = 0.5F - (hue - 0.5f);
	}
	hue += 0.5f;
	float r, g, b = 0;
	Utils::ColorConvertHSVtoRGB(hue, 0.5, 1.f, r, g, b);

	return MC_Color(r * 255, g * 255, b * 255, interface->coloropacity);
}
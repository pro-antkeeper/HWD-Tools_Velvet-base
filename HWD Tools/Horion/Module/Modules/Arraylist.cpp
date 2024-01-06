#include "ArrayList.h"

float ArrayList::opacity{ 0 };

ArrayList::ArrayList() : IModule(0x0, Category::HUD, "draws the array list on the right of your screen") {
	registerBoolSetting("Rainbow Array", &this->rainbowArrayList, this->rainbowArrayList);
	registerBoolSetting("Bottom", &this->bottom, this->bottom);
	registerBoolSetting("Show Keybinds", &this->keybinds, this->keybinds);
	registerBoolSetting("Outline", &this->outline, this->outline);
	registerBoolSetting("Underline", &this->underline, this->underline);
	registerFloatSetting("ArrayList Opacity", &opacity, opacity, 0.f, 1.f);
	registerFloatSetting("Scale", &this->scale, this->scale, 0.5f, 1.5f);
	registerIntSetting("ColorOpacity", &arraycoloropa, arraycoloropa, 0, 255);
}

ArrayList::~ArrayList() {
}

const char* ArrayList::getModuleName() {
	return ("Arraylist");
}

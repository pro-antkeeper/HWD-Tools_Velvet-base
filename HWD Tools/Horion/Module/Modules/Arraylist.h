#pragma once
#include "Module.h"

class ArrayList : public IModule {
public:
	SettingEnum Modes;
	ArrayList();
	~ArrayList();

	bool bottom = true;
	float opacity = 0.2f;
	bool bugs = false;
	bool rgba = false;
	bool Cool = false;
	float scale = 1.0f;
	int arraycoloropa = 150;

	virtual const char* getModuleName() override;
};

#include "AntiBot.h"
#include "../../pch.h"

using namespace std;
AntiBot::AntiBot() : IModule(0, Category::PLAYER, "Ignores anticheat generated NPCs")
{
	registerEnumSetting("Mode", &mode, 0);
	mode.addEntry(EnumEntry("Basic", 0));
	mode.addEntry(EnumEntry("Advanced", 1));
	registerBoolSetting("Invisible Check", &invisibleCheck, invisibleCheck);
	registerBoolSetting("EntityID Check", &entityIdCheck, entityIdCheck);
	registerBoolSetting("Hitbox Check", &hitboxCheck, hitboxCheck);
	registerBoolSetting("Other Check", &otherCheck, otherCheck);
	registerBoolSetting("Extra Check", &extraCheck, extraCheck);
}

const char* AntiBot::getRawModuleName()
{
	return "AntiBot";
}

const char* AntiBot::getModuleName()
{
	name = string("AntiBot ") + string(GRAY) + string("Advanced") + string(GRAY);
	return name.c_str();
}

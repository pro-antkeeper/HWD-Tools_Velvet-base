#include "Target.h"

#include <regex>

#include "../Horion/Module/ModuleManager.h"

C_LocalPlayer** localPlayer;

void Target::init(C_LocalPlayer** cl) {
	localPlayer = cl;
}

bool Target::isValidTarget(C_Entity* ent, bool checkFriends) {
	if (ent == NULL)
		return false;

	if (ent == nullptr)
		return false;

	auto localPlayer = g_Data.getLocalPlayer();

	if (!localPlayer->isAlive()) {
		//g_Hooks.entityList.clear();
		return false;
	}

	if (ent == localPlayer)
		return false;

	if (!ent->isAlive())
		return false;

	if (!ent->isAlive())
		return false;

	auto entityTypeId = ent->getEntityTypeId();

	if (entityTypeId == 63) {
			auto targetName = ent->getNameTag();
			auto localName = localPlayer->getNameTag();
			if (targetName->getTextLength() > 2 && localName->getTextLength() > 2) {
				auto colorTargetName = std::regex_replace(targetName->getText(), std::regex("\\§r"), "");
				auto colorLocalName = std::regex_replace(localName->getText(), std::regex("\\§r"), "");
				if (colorTargetName.at(0) == colorLocalName.at(0))
					return false;
			}
	}

	// Temporarily removed from gui, tons of false negatives

	if (!localPlayer->canAttack(ent, false))
		return false;

	if (!(localPlayer)->canAttack(ent, false))
		return false;

	return true;
}

bool Target::containsOnlyASCII(const std::string& string) {
	for (auto c : string) {
		int n = static_cast<unsigned char>(c);
		if (n > 127 && n != -89) {
			return false;
		}
	}
	return true;
}

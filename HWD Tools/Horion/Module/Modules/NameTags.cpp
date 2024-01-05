#include "NameTags.h"
#include "../../../pch.h"

NameTags::NameTags() : IModule(0, Category::VISUAL, "Displays a more detailed name")
{
	registerFloatSetting("Scale", &scalen, scalen, 0.3f, 1.5f);
	registerEnumSetting("Render", &renderMode, 1);
	renderMode.addEntry(EnumEntry("None", 0));
	renderMode.addEntry(EnumEntry("Underline", 1));
	renderMode.addEntry(EnumEntry("Rectangle", 2));
	registerBoolSetting("Show Armor", &displayArmor, displayArmor);
	registerBoolSetting("Health", &drawhealth, drawhealth);
	registerIntSetting("Opacity", &opacity, opacity, 0, 255);
}

const char* NameTags::getModuleName()
{
	return ("NameTags");
}

void drawNameTags(C_Entity* ent, bool)
{
	static auto nametags = moduleMgr->getModule<NameTags>();

	if (ent->timeSinceDeath > 0)
		return;
	if (ent->getNameTag()->getTextLength() < 1)
		return;
	if (TargetUtil::isValidTarget(ent) && nametags->isEnabled() || (FriendList::findPlayer(ent->getNameTag()->getText()) && ent->getNameTag()->getTextLength() >= 1 && ent->getEntityTypeId() == 319))
	{
		nametags->nameTags.insert(Utils::sanitize(ent->getNameTag()->getText()));
		float dist = ent->getPos()->dist(*g_Data.getLocalPlayer()->getPos());
		// DrawUtils::drawNameTags(ent, fmax(0.6f, 3.f / dist));
		DrawUtils::drawNameTags(ent, fmax(nametags->scalen, 3.f / dist));
		DrawUtils::flush();
	}
}

void NameTags::onPreRender(C_MinecraftUIRenderContext* renderCtx)
{
	if (!g_Data.canUseMoveKeys())
		return;

	// retarded
	if (ingameNametagSetting && hideTags)
		if (!gotPrevSetting)
		{
			lastSetting = *ingameNametagSetting;
			gotPrevSetting = true;
			*ingameNametagSetting = false;
		}
		else
			*ingameNametagSetting = false; // disable other ppl's nametags

	g_Data.forEachEntity(drawNameTags);
}

void NameTags::onDisable()
{
	if (ingameNametagSetting && gotPrevSetting && hideTags)
	{
		*ingameNametagSetting = lastSetting;
		gotPrevSetting = false;
	}
}

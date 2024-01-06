#define NOMINMAX // fixes da std::min eror
#include "CrystalUtilsJTWD.h"
#include "../../Utils/Target.h"
#include "../ModuleManager.h"
#include "../../../Utils/ColorUtil.h"

using namespace std;
// EZ CA
CrystalPlace::CrystalPlace() : IModule(0, Category::COMBAT, "Automatically places and breaks end crystals. Rewrite by Merci")
{

	// Rotations
	rotate = SettingEnum(this)
		.addEntry(EnumEntry("None", 0))
		.addEntry(EnumEntry("Normal", 1))
		.addEntry(EnumEntry("Silent", 2))
		.addEntry(EnumEntry("PitchUp", 3));
	registerEnumSetting("Rotations", &this->rotate, 0);

	// Safety
	registerBoolSetting("Safety", &this->safetyFirst, this->safetyFirst);
	registerFloatSetting("SafetyBalance", &this->minHealth, this->minHealth, 0.f, 20.f);
	registerFloatSetting("Self Damage", &this->maxSelfDmg, this->maxSelfDmg, 0.f, 20.f);
	registerBoolSetting("Ignore Eat", &ignoreEat, ignoreEat);
	registerBoolSetting("Ignore Tools", &ignoreTools, ignoreTools);

	// Delays and ranges
	registerIntSetting("Place Delay", &this->delay, this->delay, 0, 25);
	registerIntSetting("Timeout", &this->placetimes, this->placetimes, 1, 5);

	registerIntSetting("Proximity", &this->maxProximity, this->maxProximity, 1, 8);
	registerFloatSetting("Target Range", &this->range, this->range, 5, 15);
	registerFloatSetting("Target WallRange", &this->postWallsR, this->postWallsR, 0.f, 10.f);
	registerFloatSetting("Place Range", &this->placeRange, this->placeRange, 0.f, 12.f);
	registerFloatSetting("Place WallRange", &this->thruWallsR, this->thruWallsR, 0, 10);

	// Placement
	registerBoolSetting("Sequential", &this->attackMulti, this->attackMulti);

	calcDmgType = SettingEnum(this)
		.addEntry(EnumEntry("Java", 0))
		.addEntry(EnumEntry("Vanilla", 1));
	registerEnumSetting("Protocol", &this->calcDmgType, 0);

	registerBoolSetting("1.13+", &this->noCheckUpper, this->noCheckUpper);
	registerFloatSetting("Min Enemy Damage", &this->minEnemDmg, this->minEnemDmg, 0.f, 20.f);

	priority = SettingEnum(this)
		.addEntry(EnumEntry("Distance", 0))
		.addEntry(EnumEntry("Health", 1));
	registerEnumSetting("Priority", &this->priority, 0);

	// Swap
	switchType = SettingEnum(this)
		.addEntry(EnumEntry("None", 0))
		.addEntry(EnumEntry("Regular", 1))
		.addEntry(EnumEntry("SwitchBack", 2))
		//.addEntry(EnumEntry("Mob-Equipment", 3))
		.addEntry(EnumEntry("NukkitSpoof", 4));
	registerEnumSetting("Swap", &this->switchType, 0);

	// FacePlace
	facePlaceType = SettingEnum(this)
		.addEntry(EnumEntry("True", 0))
		.addEntry(EnumEntry("False", 1));
	registerEnumSetting("Faceplace", &this->facePlaceType, 0);
	registerFloatSetting("FacePlace HP", &this->fpThresh, this->fpThresh, 0.f, 20.f);
	registerFloatSetting("FacePlace Dmg", &this->dmgAtThresh, this->dmgAtThresh, 0.f, 20.f);

	// Render
	renderType = SettingEnum(this)
		.addEntry(EnumEntry("Both", 0))
		.addEntry(EnumEntry("Fill", 1))
		.addEntry(EnumEntry("Box", 2))
		.addEntry(EnumEntry("None", 3));
	registerEnumSetting("Render", &this->renderType, 0);
	registerFloatSetting("Outline Opacity", &this->OutOpacity, 0.1f, 0.f, 1.f);
	registerFloatSetting("Fill Opacity", &this->Opacity, 0.1f, 0.f, 1.f);
	registerBoolSetting("Show Damage", &this->drawEnemDmg, this->drawEnemDmg);
	registerBoolSetting("Show SelfDamage", &this->drawSelfDmg, this->drawSelfDmg);

	// Funni stuff // Misc
	registerBoolSetting("Gaming Mode", &this->sayGaming, this->sayGaming);

	// End
}

CrystalPlace::~CrystalPlace() {};

const char* CrystalPlace::getRawModuleName()
{
	return ("AutoCrystalRewrite");
}

const char* CrystalPlace::getModuleName()
{
	std::string name = "AutoCrystalRewrite";
	switch (calcDmgType.selected)
	{
	case 0:
		name = string("AutoCrystalRewrite ") + string(GRAY) + string("Java");
		break;
	case 1:
		name = string("AutoCrystalRewrite ") + string(GRAY) + string("Vanilla");
		break;
	}
	return name.c_str();
}

void getCrystal()
{
	auto supplies = g_Data.getLocalPlayer()->getSupplies();
	auto inv = supplies->inventory; // g_Data.getLocalPlayer()->getSupplies()->inventory->getItemStack(g_Data.getLocalPlayer()->getSupplies())->getItem()->itemID

	for (int n = 0; n < 9; n++)
	{
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr)
		{
			if (stack->getItem()->itemId == 637)
			{ // select crystal
				supplies->selectedHotbarSlot = n;
				return;
			}
		}
	}
}
void findSomeEndCrystals() // NukkitSpof
{
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	for (int n = 0; n < 9; n++)
	{
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr)
		{
			if (stack->getItem()->itemId == 637)
			{ // select crystal
				supplies->selectedHotbarSlot = n;
				return;
			}
		}
	}
}

bool isthereCrystalsInInventory()
{
	auto supplies = g_Data.getLocalPlayer()->getSupplies();
	auto inv = supplies->inventory; // g_Data.getLocalPlayer()->getSupplies()->inventory->getItemStack(g_Data.getLocalPlayer()->getSupplies())->getItem()->itemID

	for (int n = 0; n < 9; n++)
	{
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr)
		{
			if (stack->getItem()->itemId == 637)
			{ // theres a crystal in inventory
				return true;
			}
		}
	}
	return false;
}

static std::vector<C_Entity*> tgtList;
bool locateEntCrPl(C_Entity* curEnt, bool isRegularEntity)
{
	if (curEnt == nullptr)
		return false;
	if (curEnt == g_Data.getLocalPlayer())
		return false; // Skip Local player
	if (!curEnt->isAlive())
		return false;
	if (!g_Data.getLocalPlayer()->isAlive())
		return false;
	if (curEnt->getEntityTypeId() == 71)
		return false; // endcrystal
	if (curEnt->getEntityTypeId() == 66)
		return false; // falling block
	if (curEnt->getEntityTypeId() == 64)
		return false; // item
	if (curEnt->getEntityTypeId() == 69)
		return false; // xp orb
	if (curEnt->width <= 0.01f || curEnt->height <= 0.01f)
		return false; // Don't hit this pesky antibot on 2b2e.org
	if (!Target::isValidTarget(curEnt))
		return false;

	float dist = (curEnt->getHumanPos()).dist(g_Data.getLocalPlayer()->getHumanPos());
	if (dist <= moduleMgr->getModule<CrystalPlace>()->range)
	{
		tgtList.push_back(curEnt);
		return true;
	}
	return false;
}

static std::vector<C_Entity*> entLocator120;
bool processEntForentLocator120(C_Entity* curEnt, bool isRegularEntity)
{
	if (curEnt == nullptr)
		return false;
	if (!curEnt->isAlive())
		return false;
	if (!g_Data.getLocalPlayer()->isAlive())
		return false;
	if (curEnt->getEntityTypeId() == 71)
		return false; // endcrystal
	if (curEnt->getEntityTypeId() == 69)
		return false; // xp orb
	if (curEnt->width <= 0.01f || curEnt->height <= 0.01f)
		return false; // Don't hit this pesky antibot on 2b2e.org

	float dist = (curEnt->getHumanPos()).dist(g_Data.getLocalPlayer()->getHumanPos());
	if (dist <= moduleMgr->getModule<CrystalPlace>()->range)
	{
		entLocator120.push_back(curEnt);
		return true;
	}
	return false;
}

void CrystalPlace::onEnable()
{
	if (g_Data.getLocalPlayer() == nullptr)
		return;

	if (switchType.GetSelectedEntry().GetValue() == 1)
	{ // if switchType == regular
		origSlot = g_Data.getLocalPlayer()->getSupplies()->selectedHotbarSlot;
		getCrystal();
	}
	CJTWDPlaceArr.clear();
	tgtList.clear();
	entLocator120.clear();

	auto CPlace = moduleMgr->getModule<CrystalPlace>();
	auto CBreak = moduleMgr->getModule<CrystalBreak>();
	if (CPlace->isEnabled())
		cbWasEnabled = true;
	if (cbWasEnabled)
	{
		CBreak->isEnabled(); //doea this work?? lmfao
	}
	if (sayGaming)
	{
		g_Data.getClientInstance()->getGuiData()->displayClientMessageF("<%sHighway%sDepartment%s> we %sgaming", BLUE, RED, WHITE, DARK_GREEN);
	}
	return;
}

bool CrystalPlace::isPlaceValid(vec3_t location, C_Entity* atkObj)
{ // 100% WORKING (real)

	using getSeenPercent_t = float(__fastcall*)(C_BlockSource*, vec3_t const&, AABB const&);
	static getSeenPercent_t getSeenPercent = reinterpret_cast<getSeenPercent_t>(FindSignature("40 53 55 41 56 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4"));

	// place location obstructed; do wall range
	if (getSeenPercent(g_Data.getLocalPlayer()->region, location.add(0.5f, 0.f, 0.5f), g_Data.getLocalPlayer()->aabb) == 0.f &&
		thruWallsR != 10)
	{

		if (thruWallsR == 0) // walls completely disabled, might as well save on processing time by not going through
			return false;

		lineResults lineRes = countBlksAlongLine(g_Data.getLocalPlayer()->eyePos0, location.add(0.5f));
		float amtOfShitInBtwn = lineRes.blockCount;

		if (amtOfShitInBtwn >= thruWallsR) // theres too much shit in between
			return false;				   // works

		vec3_t lastKnownSolid = lineRes.lastSolidBlock;
		if (lastKnownSolid.dist(location) > postWallsR) // shit is too far away from wall
			return false;								// works
	}

	if (g_Data.getLocalPlayer()->region->getBlock(location)->toLegacy()->blockId != 0)								 // location is not air
		return false;																								 // works
	if (!noCheckUpper && g_Data.getLocalPlayer()->region->getBlock(location.add(0, 1, 0))->toLegacy()->blockId != 0) // location+(0,1,0) is not air when 1.12 on
		return false;																								 // works
	if (!noCheckUpper && checkCornerHitboxCollision(&location.add(0, 1, 0), atkObj))								 // make sure opponent does not step on the crystal, thereby blocking placements
		return false;
	if (checkCornerHitboxCollision(&location, atkObj))												   // opponent's hitbox intersects location
		return false;																				   // works
	if (g_Data.getLocalPlayer()->region->getBlock(location.sub(0, 1, 0))->toLegacy()->blockId != 49 && // floor is not obsidian or bedrock;
		g_Data.getLocalPlayer()->region->getBlock(location.sub(0, 1, 0))->toLegacy()->blockId != 7)	   // - cannot place crystal : return
		return false;																				   // works

	float distToLoc = (location).dist(g_Data.getLocalPlayer()->getHumanPos());
	if (distToLoc > placeRange) // place location is too far
		return false;			// works
	auto friendly = g_Data.getLocalPlayer();

	// if player.health < projected damage, return false
	if (friendly->getHealth() < minHealth) // playerHealth less than minHealth
		return false;					   // works

	float possibleDmg = computeExplosionDamage(location, friendly, friendly->region);
	float enemyDmg = computeExplosionDamage(location, atkObj, atkObj->region);

	if (possibleDmg > maxSelfDmg && maxSelfDmg != 20.f) // crystalDamage > maxSelf
		return false;

	if (atkObj->getHumanPos().floor().y >= location.y)
	{ // not faceplacing
		if (enemyDmg < minEnemDmg && minEnemDmg != 0.f)
			return false;
	}

	if ((atkObj->getHumanPos().floor().y + 1) <= location.y && facePlaceType.GetSelectedEntry().GetValue() == 0)
	{ // Determine if faceplacing
		if ((fpThresh < atkObj->getHealth()) && fpThresh != 20.f)
		{				  // target.health is higher than threshold
			return false; // works
		}
		if (enemyDmg < dmgAtThresh && enemyDmg != 0.f)
		{				  // damage from crystal is lower than minimum damage required
			return false; // works
		}
	}
	else if ((atkObj->getHumanPos().floor().y + 1) <= location.y && facePlaceType.GetSelectedEntry().GetValue() == 1) // faceplacing disabled by user
		return false;																								  // works

	g_Data.forEachEntity(processEntForentLocator120);
	for (auto i : entLocator120)
	{
		if (i == atkObj)
			continue;
		if (i->getHumanPos().floor() == location) // entities on [location]; return false because unable to place
			return false;						  // works
		/*
		if (moduleMgr->getModule<CrystalBreak>()->funPlace) {
			if (checkCornerHitboxCollision(&location, i, true)) {  // an entities hitbox intersect this particular location
				return false;
			}
		}
		*/
	}
	if (g_Data.getLocalPlayer()->getHumanPos().floor() == location) // localplayer on [location]; unable to place because fatty is there
		return false;												// works

	return true;
}

float CrystalPlace::computeExplosionDamage(vec3_t crystalPos, C_Entity* target, C_BlockSource* reg)
{
	float explosionRadius = 12.f; // 6 * 2

	vec3_t pos = target->getHumanPos();

	float dist = pos.dist(crystalPos) / explosionRadius;

	if (dist <= 1.f)
	{
		float exposure = 0;
		float impact;

		int armorPoints = 0;
		int epf = 0;

		using getSeenPercent_t = float(__fastcall*)(C_BlockSource*, vec3_t const&, AABB const&);
		static getSeenPercent_t getSeenPercent = reinterpret_cast<getSeenPercent_t>(FindSignature("40 53 55 41 56 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4"));

		if (calcDmgType.GetSelectedEntry().GetValue() == 0)
		{ // calcDmgType == JAVA
			float toughness = 0.f;

			float distedsize = target->getHumanPos().dist(crystalPos) / explosionRadius;
			double blockDensity = getSeenPercent(reg, crystalPos.add(0.5f, 0.f, 0.5f), target->aabb);
			double v = (1.0 - distedsize) * blockDensity;

			float damage = (float)(int)((v * v + v) / 2.0 * 7.0 * explosionRadius + 1.0); // raw damage
			damage = damage * 3 / 2;													  // assume that difficulty is set to hard (yes im serious)

			bool hasArmor = false;
			for (int i = 0; i < 4; i++)
			{ // process armor
				C_ItemStack* armor = target->getArmor(i);

				if (armor->item == nullptr)
					continue;

				int armorId = armor->getItem()->itemId;

				/*	GEAR		TURT	LEAT	GOLD	CHML	IRON	DIAM	NETH
				 *	Helm		573		335		351		339		343		347		609
				 *	Tunic		null	336		352		340		344		348		610
				 *	Pants		null	337		353		341		345		349		611
				 *	Boots		null	338		354		342		346		350		612
				 */

				switch (armorId)
				{
				case 573: // turt helm
					armorPoints += 2;
					break;

				case 335: // leat helm
					armorPoints += 1;
					break;
				case 336: // leat ches
					armorPoints += 3;
					break;
				case 337: // leat pant
					armorPoints += 2;
					break;
				case 338: // leat boot
					armorPoints += 1;
					break;

				case 351: // gold helm
					armorPoints += 2;
					break;
				case 352: // gold ches
					armorPoints += 5;
					break;
				case 353: // gold pant
					armorPoints += 3;
					break;
				case 354: // gold boot
					armorPoints += 1;
					break;

				case 339: // chml helm
					armorPoints += 2;
					break;
				case 340: // chml ches
					armorPoints += 5;
					break;
				case 341: // chml pant
					armorPoints += 4;
					break;
				case 342: // chml boot
					armorPoints += 1;
					break;

				case 343: // iron helm
					armorPoints += 2;
					break;
				case 344: // iron ches
					armorPoints += 6;
					break;
				case 345: // iron pant
					armorPoints += 5;
					break;
				case 346: // iron boot
					armorPoints += 2;
					break;
				default:
					if (armorId == 347 || armorId == 609 || armorId == 350 || armorId == 612)
					{ // neth/diam helm/boot
						armorPoints += 3;
					}
					else if (armorId == 348 || armorId == 610)
					{ // neth/diam ches
						armorPoints += 8;
					}
					else if (armorId == 349 || armorId == 611)
					{ // neth/diam pant
						armorPoints += 6;
					}

					if (347 <= armorId && armorId <= 350)
					{ // armor diam
						toughness += 2.f;
					}
					else if (609 <= armorId && armorId <= 612)
					{ // armor neth
						toughness += 3.f;
					}
				}
				hasArmor = true;
				epf += (int)getBlastDamageEnchantReduction(armor);
			}

			if (hasArmor)
			{
				float F_epf = fminf((float)epf, 15.f) / 25.f;

				// damage, with armor, no ench
				// damage = damage * (1 / ( fmaxf(((float)armorPoints/5), (armorPoints - (4*damage)/(toughness+8))))); // seems reasonable
				damage = damage * fminf(((float)armorPoints / 5), (1 / ((float)armorPoints / 5)));

				// damage, with ench
				damage = damage * (1 - F_epf);
			}
			return damage;
		}
		else if ((calcDmgType.GetSelectedEntry().GetValue() == 1))
		{
			exposure = getSeenPercent(reg, crystalPos.add(0.5f, 0.f, 0.5f), target->aabb);
			impact = (1 - dist) * exposure;
			int damage = (int)((impact * impact * 3.5f + impact * 0.5f * 7.f) * explosionRadius + 1.f);

			float finalDamage = (float)damage;

			// int toughness = 0;

			for (int i = 0; i < 4; i++)
			{
				C_ItemStack* armor = target->getArmor(i);

				if (armor->item == nullptr)
					continue;

				armorPoints += (*armor->item)->getArmorValue();
				epf += (int)getBlastDamageEnchantReduction(armor);
			}

			finalDamage -= finalDamage * armorPoints * 0.04f; // basic armor reduction

			finalDamage -= finalDamage * std::min(ceilf(std::min(epf, 25) * ((float)75 /* it's unneccessary to simulate randomness */ / 100)), 20.f) * 0.04f; // enchant reduction

			// absorption and resistance are impossible to predict client side

			int targetY = (int)floorf(target->aabb.lower.y);
			int crystalY = (int)floorf(crystalPos.y);

			return finalDamage;
		}
	}
	else
	{
		return 0.f;
	}
}

float CrystalPlace::getBlastDamageEnchantReduction(C_ItemStack* armor)
{
	float epf = 0.f;

	if (calcDmgType.GetSelectedEntry().GetValue() == 0)
	{
		if (armor->getEnchantValue(0) != 4)
		{
			epf += armor->getEnchantValue(0); // protection
		}
		else
			epf += 5;							//  yeah prot in BE is OP AF
		epf += armor->getEnchantValue(3) * 2.f; // blast protection
	}

	else if (calcDmgType.GetSelectedEntry().GetValue() == 1)
	{ // java getBlastReduction
		// float rprot_level = armor->getEnchantValue(0);
		// float blast_level = armor->getEnchantValue(3) * 2.f;

		epf += armor->getEnchantValue(0);
		epf += armor->getEnchantValue(3) * 2.f;
	}

	return epf;
}

std::vector<CrystalPlacements> CrystalPlace::generateValidPlacements(C_Entity* target, int yOffset)
{ // damn look at the length of this shit
	vec3_t targetPos = target->getHumanPos().floor();
	targetPos = vec3_t(targetPos.x, targetPos.y + (float)yOffset, targetPos.z);

	// just search a flat 2d plane for now -> detect valid placements -> filter out the bests ones later on
	std::vector<CrystalPlacements> bunchashit;
	C_Entity* ent;
	if (doMultiple)
	{
		{
			auto targetPosi = rad;
			for (int x = targetPosi - 2; x <= targetPosi + 2; x++)
			{
				for (int z = targetPosi - 2; z <= targetPosi + 2; z++)
				{
					vec3_t search = targetPos.add(x, 0, z);
					if (search.dist(target->getHumanPos()) < maxProximity && target->getHumanPos().floor() != search)
					{
						if (isPlaceValid(search, target))
						{
							CrystalPlacements me;
							me.toPlace = search;
							me.enemyDmg = computeExplosionDamage(search, target, target->region);
							me.selfDmg = computeExplosionDamage(search, g_Data.getLocalPlayer(), g_Data.getLocalPlayer()->region);

							bunchashit.push_back(me);
						}
					}
				}
			}
		}
		return bunchashit;
	}
	{
		for (int x = -maxProximity; x < maxProximity; x++)
		{
			for (int z = -maxProximity; z < maxProximity; z++)
			{
				vec3_t search = targetPos.add(x, 0, z);
				if (search.dist(target->getHumanPos()) < maxProximity && target->getHumanPos().floor() != search)
				{
					if (isPlaceValid(search, target))
					{
						CrystalPlacements me;
						me.toPlace = search;
						me.enemyDmg = computeExplosionDamage(search, target, target->region);
						me.selfDmg = computeExplosionDamage(search, g_Data.getLocalPlayer(), g_Data.getLocalPlayer()->region);

						bunchashit.push_back(me);
					}
				}
			}
		}
	}
	return bunchashit;
}

bool cmpPlacements(CrystalPlacements E1, CrystalPlacements E2)
{
	bool cmpType = moduleMgr->getModule<CrystalPlace>()->safetyFirst;

	if (!cmpType)
	{ // Enem Dmg->high to low -> safetyFirst off
		return E1.enemyDmg > E2.enemyDmg;
	}

	if (cmpType)
	{ // Self Dmg->low to high -> safetyFirst on
		return E1.selfDmg < E2.selfDmg;
	}
}

bool cmpDup(CrystalPlacements E1, CrystalPlacements E2)
{
	bool cmpType = !(moduleMgr->getModule<CrystalPlace>()->safetyFirst);

	if (!cmpType)
	{
		return E1.enemyDmg > E2.enemyDmg;
	}

	if (cmpType)
	{
		return E1.selfDmg < E2.selfDmg;
	}
}

CrystalPlacements CrystalPlace::bestPlaceOnPlane(C_Entity* targ, int yLevel)
{

	std::vector<CrystalPlacements> validShit = generateValidPlacements(targ, yLevel);

	if (validShit.empty())
	{
		CrystalPlacements anEmptyValue;
		anEmptyValue.enemyDmg = -42069; // ima use this as the empty value
		return anEmptyValue;
	}

	std::sort(validShit.begin(), validShit.end(), cmpPlacements);

	// get duplicated values @ top

	float hl = 0.f;
	std::vector<CrystalPlacements> dups;

	if (safetyFirst)
	{
		for (const CrystalPlacements& i : validShit)
		{
			hl = i.selfDmg;

			if (i.selfDmg == hl)
			{
				dups.push_back(i);
			}
			else
				break;
		}
	}
	else if (!safetyFirst)
	{
		for (const CrystalPlacements& i : validShit)
		{
			hl = i.enemyDmg;

			if (i.enemyDmg == hl)
			{
				dups.push_back(i);
			}
			else
				break;
		}
	}

	if (dups.size() == 1)
		return dups[0];

	// sort dups
	std::sort(dups.begin(), dups.end(), cmpDup);
	return dups[0];
}

bool cmpAgain(CrystalPlacements E1, CrystalPlacements E2)
{
	return E1.enemyDmg > E2.enemyDmg;
}

CrystalPlacements CrystalPlace::CrystalAuraJTWD(C_Entity* target)
{
	std::vector<CrystalPlacements> on3d;

	int maxVOff = (facePlaceType.GetSelectedEntry().GetValue() == 1 ? 1 : 3); // faceplace disabled-> maxOffset = 1

	for (int vOffset = -5; vOffset < maxVOff; vOffset++)
	{

		CrystalPlacements PN; // totally not a reference to an edgily named hack client that i made a yr ago ;D
		PN = bestPlaceOnPlane(target, vOffset);

		if (PN.enemyDmg != -42069)
		{
			on3d.push_back(PN);
		}
	}

	if (on3d.empty())
	{
		CrystalPlacements anEmptyValue;
		anEmptyValue.enemyDmg = -42069; // ima use this as the empty value
		return anEmptyValue;
	}

	// force sorting on3d into the highest enemy damage
	std::sort(on3d.begin(), on3d.end(), cmpAgain);
	return on3d[0];
}

bool compareTargListVec(C_Entity* E1, C_Entity* E2)
{
	int whatToCompare = moduleMgr->getModule<CrystalPlace>()->priority.GetSelectedEntry().GetValue();

	if (whatToCompare == 0)
	{ // distance comparison -> low to high
		return g_Data.getLocalPlayer()->getHumanPos().dist(E1->getHumanPos()) < g_Data.getLocalPlayer()->getHumanPos().dist(E2->getHumanPos());
	}
	else if (whatToCompare == 1)
	{ // health comparison -> low to high
		return E1->getHealth() < E2->getHealth();
	}
}

void stopSpoofing()
{
	__int64 id = *g_Data.getLocalPlayer()->getUniqueId();
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	C_MobEquipmentPacket a(id, *g_Data.getLocalPlayer()->getSelectedItem(), supplies->selectedHotbarSlot, supplies->selectedHotbarSlot);
	g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a);
	return;
}

int switchSlot;
bool grabNow = false;

bool hasRotten = false;
bool packetSent = false;
bool isPlayerAuthInput = false;

void CrystalPlace::onTick(C_GameMode* gm)
{
	auto cp = moduleMgr->getModule<CrystalPlace>();
	if (g_Data.getLocalPlayer() == nullptr || !g_Data.canUseMoveKeys() || (!isthereCrystalsInInventory() && switchType.GetSelectedEntry().GetValue() == 3))
		return;

	auto antiDesync = moduleMgr->getModule<AntiDesync>();
	if (antiDesync->mode.selected == 0 && antiDesync->isEnabled())
	{
		auto player = g_Data.getLocalPlayer();
		if (player->getSelectedItemId() == 259 && g_Data.isRightClickDown())
			return;
		if (player->getSelectedItemId() == 300 && g_Data.isRightClickDown())
			return;
	}

	if (!ignoreEat)
	{
		auto player = g_Data.getLocalPlayer();
		if (player->getSelectedItemId() == 259 && g_Data.isRightClickDown())
			return;
		if (player->getSelectedItemId() == 300 && g_Data.isRightClickDown())
			return;
	}

	auto supplies = g_Data.getLocalPlayer()->getSupplies();
	auto inv = supplies->inventory;
	slotMC = supplies->selectedHotbarSlot;

	hasRotten = false;
	rotUpNow = true;
	packetSent = false;
	isPlayerAuthInput = false;

	placeArrEmpty = CJTWDPlaceArr.empty();

	entLocator120.clear();
	tgtList.clear();
	CJTWDPlaceArr.clear();

	g_Data.forEachEntity(locateEntCrPl);

	if (tgtList.empty())
		return;

	if (!ignoreTools && g_Data.getLocalPlayer()->getSelectedItemId() == 300 || g_Data.getLocalPlayer()->getSelectedItemId() == 316 || g_Data.getLocalPlayer()->getSelectedItemId() == 318 || g_Data.getLocalPlayer()->getSelectedItemId() == 319 || g_Data.getLocalPlayer()->getSelectedItemId() == 604 || g_Data.getLocalPlayer()->getSelectedItemId() == 607 || g_Data.getLocalPlayer()->getSelectedItemId() == 606)
		return;

	std::sort(tgtList.begin(), tgtList.end(), compareTargListVec);

	if (!attackMulti)
	{ // attack single enemy
		bool canAttack = false;
		while (!canAttack)
		{
			CrystalPlacements placeInfo = CrystalAuraJTWD(tgtList[0]);
			if (placeInfo.enemyDmg != -42069)
			{
				CrystalInfo a2vec;
				a2vec.CPlacements = placeInfo;
				a2vec.ent = tgtList[0];
				CJTWDPlaceArr.push_back(a2vec);
				canAttack = true;
			}
			else
			{
				if (tgtList.size() != 1)
				{
					tgtList.erase(tgtList.begin()); // try again with a different person
				}
				else
					canAttack = true;
			}
		}
	}
	else if (attackMulti)
	{
		for (auto& enemy : tgtList)
		{

			CrystalPlacements placeInfo = CrystalAuraJTWD(enemy);
			if (placeInfo.enemyDmg != -42069)
			{
				CrystalInfo a2vec;
				a2vec.CPlacements = placeInfo;
				a2vec.ent = enemy;
				CJTWDPlaceArr.push_back(a2vec);
			}
		}
	}

	// THE D M Z \\

	ctr++;

	if (ctr >= delay)
	{
		if (hasRotten && !packetSent && !isPlayerAuthInput && rotate.GetSelectedEntry().GetValue() == 2)
		{
			C_MovePlayerPacket pkt(g_Data.getLocalPlayer(), *gm->player->getPos());
			g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&pkt);

			hasRotten = false;
			packetSent = false;
		}

		if (switchType.GetSelectedEntry().GetValue() == 2 && !grabNow)
		{
			// clientMessageF("SHB: %i" , g_Data.getLocalPlayer()->getSupplies()->selectedHotbarSlot);
			switchSlot = g_Data.getLocalPlayer()->getSupplies()->selectedHotbarSlot;
			getCrystal();
			grabNow = true;
			return;
		}

		if (g_Data.getLocalPlayer()->getSelectedItemId() == 637 && !currentlySwitchingHotbar || switchType.GetSelectedEntry().GetValue() == 3 || switchType.GetSelectedEntry().GetValue() == 4)
		{

			if (switchType.GetSelectedEntry().GetValue() == 3)
				getCrystalSpoof();

			if (switchType.selected == 4)
				findSomeEndCrystals();

			for (CrystalInfo& Place : CJTWDPlaceArr)
			{

				vec3_t placeMe = Place.CPlacements.toPlace;

				if (rotate.GetSelectedEntry().GetValue() == 1)
				{ // normal rotations
					rotAngle = g_Data.getLocalPlayer()->getHumanPos().CalcAngle(placeMe);
				}

				else if (rotate.GetSelectedEntry().GetValue() == 2)
				{ // silent rotations
					C_MovePlayerPacket pkt(g_Data.getLocalPlayer(), *gm->player->getPos());

					rotAngle = gm->player->getPos()->CalcAngle((Place.CPlacements.toPlace).add(0.5f, 0.5f, 0.5f));

					pkt.pitch = rotAngle.x;
					pkt.yaw = rotAngle.y;
					pkt.headYaw = rotAngle.y;

					g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&pkt);
				}

				else if (rotate.GetSelectedEntry().GetValue() == 3)
					rotUpNow = true;

				g_Data.getCGameMode()->buildBlock(&vec3_ti(placeMe).sub(0, 1, 0), 0, true); //last one is true or false??
			}
			if (switchType.GetSelectedEntry().GetValue() == 3)
				stopSpoofing();
		}
		else if (g_Data.getLocalPlayer()->getSelectedItemId() == 637 && currentlySwitchingHotbar)
		{
			currentlySwitchingHotbar = false;
			return;
		}
		else if (g_Data.getLocalPlayer()->getSelectedItemId() != 637)
		{
			currentlySwitchingHotbar = true;
			return;
		}

		// this part of the code breaks the item switcher; you cant attack and switch items simultaneously
		// so does buildBlock, apparently
		/* this is just a temporary code just to show that yes, i can place crystals
		if (g_Data.getLocalPlayer()->getSelectedItemId() == 637) {
			g_Data.forEachEntity([](C_Entity* ent, bool b) {
				int id = ent->getEntityTypeId();
				if (id == 71) {  // kill crystal
					g_Data.getCGameMode()->attack(ent);
				}
			});
		}
		/* end temp code */

		if (switchType.GetSelectedEntry().GetValue() == 2 && grabNow)
		{
			g_Data.getLocalPlayer()->getSupplies()->selectedHotbarSlot = switchSlot;
			grabNow = false;
		}

		if (switchType.selected == 4)
			supplies->selectedHotbarSlot = slotMC;

		ctr = 0;
		return;
	}

	return;
}

bool CrystalPlace::getCrystalSpoof()
{
	__int64 id = *g_Data.getLocalPlayer()->getUniqueId();
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	for (int n = 0; n < 9; n++)
	{
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr)
		{
			if ((*stack->item)->itemId == 637)
			{
				C_MobEquipmentPacket a(id, *stack, n, n);
				g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a);
				return true;
			}
		}
	}
	C_MobEquipmentPacket a(id, *g_Data.getLocalPlayer()->getSelectedItem(), supplies->selectedHotbarSlot, supplies->selectedHotbarSlot);
	g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a);
	return false;
}

void CrystalPlace::onWorldTick(C_GameMode* gm)
{
	if (g_Data.getLocalPlayer() == nullptr)
		return;

	if (switchType.GetSelectedEntry().GetValue() == 1) // if switchType == regular; always grab crystals
		getCrystal();

	return;
}

void CrystalPlace::onDisable()
{
	if (switchType.GetSelectedEntry().GetValue() == 1)
	{ // if switchType == regular; always grab crystals
		auto supplies = g_Data.getLocalPlayer()->getSupplies();
		supplies->selectedHotbarSlot = origSlot;
		return;
	}
	auto CBreak = moduleMgr->getModule<CrystalBreak>();
	if (cbWasEnabled)
	{
		CBreak->setEnabled(false);
		cbWasEnabled = false;
	}
	if (sayGaming)
	{
		g_Data.getClientInstance()->getGuiData()->displayClientMessageF("<%sHighway%sDepartment%s> we aint %sgaming %sno more", BLUE, RED, WHITE, RED, RESET);
	}
}

float roundoff(float value, unsigned char prec)
{
	float pow_10 = pow(10.0f, (float)prec);
	return round(value * pow_10) / pow_10;
}

std::string chopOffDigits(std::string STUFF, int digits)
{
	bool afterDecimalPt = false;
	int dp = 0;
	std::string toOutput;

	for (int i = 0; i < STUFF.length(); ++i)
	{
		if (STUFF.at(i) == '.')
		{
			afterDecimalPt = true;
		}

		toOutput.append(std::string(1, STUFF.at(i)));

		if (afterDecimalPt)
		{
			dp++;

			if (dp > digits)
				break;
		}
	}

	return toOutput;
}

void CrystalPlace::onPreRender(C_MinecraftUIRenderContext* renderCtx)
{
	if (g_Data.getLocalPlayer() == nullptr)
		return;
	if (!g_Data.getLocalPlayer()->isAlive())
	{
		moduleMgr->getModule<CrystalBreak>()->setEnabled(false);
		return;
	}

	if (!g_Data.canUseMoveKeys() || tgtList.empty() || !g_Data.isInGame() || tgtList[0] == NULL || tgtList.size() == 0 || tgtList[0] == nullptr || !g_Data.getLocalPlayer()->isAlive())
	{
		tgtList.clear();
		return;
	}

	if (CJTWDPlaceArr.empty())
		return;

	if (renderPlacing)
	{
		auto interfacrColor = ColorUtil::interfaceColor(1);
		for (CrystalInfo& indivCrystal : CJTWDPlaceArr)
		{
			if (CJTWDPlaceArr.size() == 0 || tgtList.size() == 0)
				return;
			vec3_t placeLoc = indivCrystal.CPlacements.toPlace;
			float enemyDmg = indivCrystal.CPlacements.enemyDmg;
			float selfDmg = indivCrystal.CPlacements.selfDmg;

			if (renderType.selected == 0 || renderType.selected == 2)
			{
				DrawUtils::setColor(interfacrColor.r, interfacrColor.g, interfacrColor.b, OutOpacity);
				DrawUtils::drawBox(placeLoc.sub(0, 1, 0), placeLoc.add(1, 0, 1), .4f, false);
			}

			std::string self_dmg = std::to_string(roundoff(selfDmg, 2));
			self_dmg = chopOffDigits(self_dmg, 2);

			if (drawSelfDmg && !syncSelfDmg)
			{
				DrawUtils::drawTextInWorld(&self_dmg, placeLoc.sub(0, 1.4f, 0), 0.6f, vec3_ti(sred, sgreen, sblue), vec3_ti(12, 12, 12), .1f);
			}

			if (drawSelfDmg && syncSelfDmg)
			{
				auto interfaceColor = ColorUtil::interfaceColor(1);
				DrawUtils::drawTextInWorld(&self_dmg, placeLoc.sub(0, 1.4f, 0), 0.6f, vec3_ti(interfaceColor.r, interfaceColor.g, interfaceColor.b), vec3_ti(12, 12, 12), .1f);
			}

			vec3_t posplace = indivCrystal.ent->getHumanPos().floor();

			std::string enem_dmg = std::to_string(roundoff(enemyDmg, 2));
			enem_dmg = chopOffDigits(enem_dmg, 2);

			if (drawEnemDmg && !syncEnemDmg)
			{
				DrawUtils::drawTextInWorld(&enem_dmg, placeLoc.sub(0, 1.2f, 0), 0.6f, vec3_ti(ered, egreen, eblue), vec3_ti(12, 12, 12), .1f);
			}

			if (drawEnemDmg && syncEnemDmg)
			{
				auto interfaceColor = ColorUtil::interfaceColor(1);
				DrawUtils::drawTextInWorld(&enem_dmg, placeLoc.sub(0, 1.2f, 0), 0.6f, vec3_ti(interfaceColor.r, interfaceColor.g, interfaceColor.b), vec3_ti(12, 12, 12), .1f);
			}
		}
		if (renderType.selected == 0 || renderType.selected == 1)
		{
			auto interfacrColor = ColorUtil::interfaceColor(1);
			for (CrystalInfo& indivCrystal : CJTWDPlaceArr)
			{
				if (CJTWDPlaceArr.size() == 0 || tgtList.size() == 0)
					return;
				vec3_t placeLoc = indivCrystal.CPlacements.toPlace;
				vec3_t block = placeLoc.add(0.5, -0.5, 0.5);
				float enemyDmg = indivCrystal.CPlacements.enemyDmg;
				float selfDmg = indivCrystal.CPlacements.selfDmg;
				float rentimer = 1;
				float zero = rentimer / 2;
				DrawUtils::setColor(interfacrColor.r, interfacrColor.g, interfacrColor.b, Opacity);
				{
					vec2_t fill1 = DrawUtils::worldToScreen(vec3_t(block).add(zero, zero, zero));
					vec2_t fill2 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, zero, zero));
					vec2_t fill3 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, zero, -zero));
					vec2_t fill4 = DrawUtils::worldToScreen(vec3_t(block).add(zero, zero, -zero));
					DrawUtils::drawQuad(vec2_t(fill3), vec2_t(fill4), vec2_t(fill1), vec2_t(fill2));
				}
				{

					vec2_t fill1 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, -zero, -zero));
					vec2_t fill2 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, -zero, zero));
					vec2_t fill3 = DrawUtils::worldToScreen(vec3_t(block).add(zero, -zero, zero));
					vec2_t fill4 = DrawUtils::worldToScreen(vec3_t(block).add(zero, -zero, -zero));
					DrawUtils::drawQuad(vec2_t(fill3), vec2_t(fill4), vec2_t(fill1), vec2_t(fill2));
				}
				{

					vec2_t fill1 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, -zero, zero));
					vec2_t fill2 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, zero, zero));
					vec2_t fill3 = DrawUtils::worldToScreen(vec3_t(block).add(zero, zero, zero));
					vec2_t fill4 = DrawUtils::worldToScreen(vec3_t(block).add(zero, -zero, zero));
					DrawUtils::drawQuad(vec2_t(fill3), vec2_t(fill4), vec2_t(fill1), vec2_t(fill2));
				}
				{

					vec2_t fill1 = DrawUtils::worldToScreen(vec3_t(block).add(zero, zero, -zero));
					vec2_t fill2 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, zero, -zero));
					vec2_t fill3 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, -zero, -zero));
					vec2_t fill4 = DrawUtils::worldToScreen(vec3_t(block).add(zero, -zero, -zero));
					DrawUtils::drawQuad(vec2_t(fill3), vec2_t(fill4), vec2_t(fill1), vec2_t(fill2));
				}
				{

					vec2_t fill1 = DrawUtils::worldToScreen(vec3_t(block).add(zero, -zero, zero));
					vec2_t fill2 = DrawUtils::worldToScreen(vec3_t(block).add(zero, zero, zero));
					vec2_t fill3 = DrawUtils::worldToScreen(vec3_t(block).add(zero, zero, -zero));
					vec2_t fill4 = DrawUtils::worldToScreen(vec3_t(block).add(zero, -zero, -zero));
					DrawUtils::drawQuad(vec2_t(fill3), vec2_t(fill4), vec2_t(fill1), vec2_t(fill2));
				}
				{

					vec2_t fill1 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, zero, -zero));
					vec2_t fill2 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, zero, zero));
					vec2_t fill3 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, -zero, zero));
					vec2_t fill4 = DrawUtils::worldToScreen(vec3_t(block).add(-zero, -zero, -zero));
					DrawUtils::drawQuad(vec2_t(fill3), vec2_t(fill4), vec2_t(fill1), vec2_t(fill2));
				}
			}
		}
	}
	return;
}

void CrystalPlace::onSendPacket(C_Packet* pkt)
{
	if (pkt->isInstanceOf<PlayerAuthInputPacket>())
	{
		isPlayerAuthInput = true;

		if (hasRotten)
		{
			PlayerAuthInputPacket* packet = (PlayerAuthInputPacket*)pkt;

			packet->pitch = rotAngle.x;
			packet->yaw = rotAngle.y;
			packet->yawUnused = rotAngle.y;
		}
	}
	if (pkt->isInstanceOf<C_MovePlayerPacket>())
	{
		packetSent = true;

		if (hasRotten)
		{
			auto* packet = (C_MovePlayerPacket*)pkt;

			packet->pitch = rotAngle.x;
			packet->yaw = rotAngle.y;
			packet->headYaw = rotAngle.y;
		}
	}
}

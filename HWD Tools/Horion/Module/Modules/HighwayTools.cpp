/*#include "HighwayTools.h"

HighwayTools::HighwayTools() : IModule(0, Category::PLAYER, "Builds highways automatically. Made for HWD by J") {
	registerBoolSetting("Auto Switch", &autoswitch, &autoswitch);
	registerBoolSetting("Auto Center", &autocenter, &autocenter);
	registerBoolSetting("Stable Yaw", &rotationlock, &rotationlock);
	registerBoolSetting("Replenish", &replenish, &replenish);
	registerBoolSetting("Visuals", &visualize, &visualize);
	registerFloatSetting("Place Range", &buildrange, buildrange, 0.0f, 5.5f);
	registerFloatSetting("Visual Range", &totalrange, totalrange, 0.0f, 20.0f);
	block.addEntry(EnumEntry("Normal", 0))
		.addEntry(EnumEntry("Diagonal", 1));
	registerEnumSetting("Design", &block, 0);
	axis.addEntry(EnumEntry("+X", 0))
		.addEntry(EnumEntry("-X", 1))
		.addEntry(EnumEntry("+Z", 2))
		.addEntry(EnumEntry("-Z", 3))
		.addEntry(EnumEntry("+X/+Z", 4))
		.addEntry(EnumEntry("-X/-Z", 5))
		.addEntry(EnumEntry("+X/-Z", 6))
		.addEntry(EnumEntry("-X/+Z", 7));
	registerEnumSetting("Highway", &axis, 0);
};

HighwayTools::~HighwayTools() {
}

const char* HighwayTools::getModuleName() {
	return ("HighwayTools");
}

std::unordered_map<int, std::string> HighwayTools::initializeModeNames() {  // mode initialization for message
	std::unordered_map<int, std::string> names;
	names[0] = "Normal";
	names[1] = "Diagonal";
	return names;
}

bool isHighwayBlock(C_ItemStack* C_itemStack) { // allowed highway block substrings
	std::string itemName = C_itemStack->getItem()->name.getText();
	return itemName.find("obsidian") != std::string::npos;
}

void HighwayTools::onEnable() { // sweet message & initialization
	initializeSchematic();
	setAutoBuildSlot = false;
	C_LocalPlayer* C_player = g_Data.getLocalPlayer();
	if (C_player != nullptr) {

		std::unordered_map<int, std::string> modeNames = initializeModeNames();
		std::string modeName = modeNames[block.selected];

		std::string holdingIdMsg = "%sHighway Design:%s %s";
		C_GuiData* C_guiData = g_Data.getGuiData();
		if (C_guiData != nullptr) {
			C_guiData->displayClientMessageF(holdingIdMsg.c_str(), WHITE, GOLD, modeName.c_str());
		}

		setAutoBuildSlot = false;
		oldAutoBuildSlot = C_player->getSupplies()->selectedHotbarSlot;
	}
}

std::vector<vec3_ti> HighwayTools::getCurrentSchematic(vec3_ti currentPos) { // initialization
	std::vector<vec3_ti> currentSchematic;
	for (const auto& relPos : baseSchematic) {
		currentSchematic.push_back(currentPos.add(relPos));
	}
	return currentSchematic;
}

void HighwayTools::onTick(C_GameMode* gm) { // dynamic highway schem position & autoswitching & replenishing & centering
	C_LocalPlayer* Player = g_Data.getLocalPlayer();
	if (Player != nullptr) {
		vec3_ti myPos = Player->getPos()->sub(0, 1, 0);
		startBuildPos = vec3_ti(floorf(myPos.x), floorf(myPos.y), floorf(myPos.z));

		C_PlayerInventoryProxy* supplies = Player->getSupplies();
		C_Inventory* inv = supplies->inventory;
		C_ItemStack* handItem = supplies->inventory->getItemStack(supplies->selectedHotbarSlot);

		bool highwayBlockInHotbar = false;
		for (int i = 0; i < 9; i++) {
			C_ItemStack* currentItemStack = inv->getItemStack(i);
			if (currentItemStack != nullptr && currentItemStack->item != nullptr && isHighwayBlock(currentItemStack)) {
				highwayBlockInHotbar = true;
				break;
			}
		}

		if (autoswitch && highwayBlockInHotbar) {
			C_ItemStack* handItem = inv->getItemStack(supplies->selectedHotbarSlot);
			if (handItem == nullptr || handItem->item == nullptr || !isHighwayBlock(handItem)) {
				for (int i = 0; i < 9; i++) {
					C_ItemStack* currentItemStack = inv->getItemStack(i);
					if (currentItemStack != nullptr && currentItemStack->item != nullptr && currentItemStack->getItem()->isBlock() && isHighwayBlock(currentItemStack)) {
						supplies->selectedHotbarSlot = i;
						break;
					}
				}
			}
		}

		if (replenish && !highwayBlockInHotbar && handItem != nullptr) {  // crashes when leaving server idk why cant be arsed to fix sorry <3
			C_ItemStack* selectedStack = inv->getItemStack(supplies->selectedHotbarSlot);
			if (selectedStack == nullptr || selectedStack->count == 0 || !isHighwayBlock(selectedStack)) {
				for (int i = 9; i < 36; i++) {
					C_ItemStack* stack = inv->getItemStack(i);
					if (stack != nullptr && stack->item != nullptr && isHighwayBlock(stack)) {
						inv->moveItem(i, supplies->selectedHotbarSlot);
						break;
					}
				}
			}
		}

		if (autocenter) {
			if (!Player->isJumping() && moduleMgr->getModule<HighwayTools>()->pressed == false) {
				vec3_t yR = vec3_t(floor(g_Data.getLocalPlayer()->getPos()->x), g_Data.getLocalPlayer()->getPos()->y, floor(g_Data.getLocalPlayer()->getPos()->z));
				yR.x += 0.5f;
				yR.z += 0.5f;
				g_Data.getLocalPlayer()->setPos(yR);
			}
		}
	}
	++delayticks;
}

void HighwayTools::onMove(C_MoveInputHandler* input) { // checks for auto center
	C_LocalPlayer* player = g_Data.getLocalPlayer();
	if (player == nullptr)
		return;
	if (player->isInLava() || player->isInWater())
		return;
	pressed = input->forward || input->backward || input->right || input->left;
}

void HighwayTools::onPlayerTick(C_Player* plr) { // locks yaw to closest highway
	if (rotationlock) {
		auto player = g_Data.getLocalPlayer();
		if (player != nullptr) {
			float currentYaw = player->yaw;
			if (currentYaw < 0.0f) {
				currentYaw += 360.0f;
			}

			float closestDirection = 180.0f;
			float minDifference = 180.0f;

			for (int i = 0; i < 8; i++) {
				float directionYaw = i * 45.0f;
				float difference = std::abs(currentYaw - directionYaw);

				if (difference > 180.0f) {
					difference = 360.0f - difference;
				}

				if (difference < minDifference) {
					minDifference = difference;
					closestDirection = directionYaw;
				}
			}

			player->yaw = closestDirection;
		}
	}
}

void HighwayTools::onDisable() { // set old slot
	delayticks = 0;

	C_LocalPlayer* Player = g_Data.getLocalPlayer();

	if (Player != nullptr) {
		if (autoswitch && setAutoBuildSlot == true) {
			Player->getSupplies()->selectedHotbarSlot = oldAutoBuildSlot;
		}
	}
}

void HighwayTools::onLevelRender() { // placing blocks
	if (delayticks <= 5) {
		return;
	}
	C_LocalPlayer* Player = g_Data.getLocalPlayer();
	if (Player != nullptr) {
		vec3_t myPos = *Player->getPos();
		C_PlayerInventoryProxy* supplies = Player->getSupplies();
		C_ItemStack* handItem = supplies->inventory->getItemStack(supplies->selectedHotbarSlot);

		if (handItem == nullptr || handItem->item == nullptr || !isHighwayBlock(handItem)) {
			return;
		}

		currentSchematic = getCurrentSchematic(startBuildPos);

		for (int i = 0; i < currentSchematic.size(); i++) {
			vec3_ti buildPos = currentSchematic.at(i);
			if (myPos.dist(vec3_t(buildPos.x + 0.5f, buildPos.y + 0.5f, buildPos.z + 0.5f)) <= totalrange) {
				C_Block* block = Player->region->getBlock(buildPos);
				C_BlockLegacy* blockLegacy = block->toLegacy();

				if (blockLegacy->material->isReplaceable) {
					vec3_ti blok(buildPos);
					static std::vector<vec3_ti*> checklist = { new vec3_ti(0, -1, 0), new vec3_ti(0, 1, 0), new vec3_ti(0, 0, -1), new vec3_ti(0, 0, 1), new vec3_ti(-1, 0, 0), new vec3_ti(1, 0, 0) };
					bool foundCandidate = false;
					int faceIndex = 0;

					for (auto current : checklist) {
						vec3_ti calc = blok.sub(*current);
						if (!((Player->region->getBlock(calc)->blockLegacy))->material->isReplaceable) {
							foundCandidate = true;
							blok = calc;
							break;
						}
						faceIndex++;
					}

					if (foundCandidate) {
						vec3_ti beforethingy = vec3_ti(blok.x, blok.y, blok.z);
						vec3_ti* thingy = &beforethingy;
						g_Data.getCGameMode()->buildBlock(&blok, faceIndex, thingy);
						break;
					}
				}
				else {
					currentSchematic.erase(currentSchematic.begin() + i--);
				}
			}
		}
	}
}

void HighwayTools::onPostRender(C_MinecraftUIRenderContext* renderCtx) { // visuals
	C_LocalPlayer* Player = g_Data.getLocalPlayer();
	if (Player != nullptr && g_Data.isInGame() && GameData::canUseMoveKeys()) {
		vec3_t myPos = *Player->getPos();
		C_PlayerInventoryProxy* supplies = Player->getSupplies();
		C_ItemStack* handItem = supplies->inventory->getItemStack(supplies->selectedHotbarSlot);

		bool isHighwayBlockSelected = handItem != nullptr && handItem->item != nullptr && isHighwayBlock(handItem);

		currentSchematic = getCurrentSchematic(startBuildPos);

		for (vec3_ti p : currentSchematic) {
			vec3_t bp(p.x, p.y, p.z);
			float dist = myPos.dist(bp.add(0.5f, 0.5f, 0.5f));

			if (dist <= visualrange) {
				if (!isHighwayBlockSelected || dist > buildrange + 1.0) {
					DrawUtils::setColor(1.0, 0.0, 0.0, 0.8f);
				}
				else {
					DrawUtils::setColor(0.1, 0.6, 0.7, 0.8f);
				}

				if (visualize) {
					DrawUtils::drawBox(bp, bp.add(1.f, 1.f, 1.f), 0.3f, false);
				}
			}
		}
	}
}

void HighwayTools::initializeSchematic() { // schematics for different types of highways 
	baseSchematic.clear();

	if (block.selected == 0) {  // Normal
		if (axis.selected == 0) { // +X
			baseSchematic.push_back(vec3_ti(0, -1, -2));
			baseSchematic.push_back(vec3_ti(0, -1, -1));
			baseSchematic.push_back(vec3_ti(6, 0, -3));
			baseSchematic.push_back(vec3_ti(6, -1, 0));
			baseSchematic.push_back(vec3_ti(0, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, -1, 2));
			baseSchematic.push_back(vec3_ti(6, -1, 3));
			baseSchematic.push_back(vec3_ti(6, 0, 3));
			baseSchematic.push_back(vec3_ti(1, -1, 1));
			baseSchematic.push_back(vec3_ti(1, -1, -2));
			baseSchematic.push_back(vec3_ti(1, -1, -1));
			baseSchematic.push_back(vec3_ti(4, -1, -3));
			baseSchematic.push_back(vec3_ti(2, -1, -2));
			baseSchematic.push_back(vec3_ti(2, -1, -1));
			baseSchematic.push_back(vec3_ti(4, -1, 0));
			baseSchematic.push_back(vec3_ti(2, -1, 1));
			baseSchematic.push_back(vec3_ti(4, -1, 3));
			baseSchematic.push_back(vec3_ti(5, -1, 0));
			baseSchematic.push_back(vec3_ti(3, -1, -3));
			baseSchematic.push_back(vec3_ti(5, -1, -3));
			baseSchematic.push_back(vec3_ti(5, -1, 3));
			baseSchematic.push_back(vec3_ti(3, -1, 0));
			baseSchematic.push_back(vec3_ti(3, 0, -3));
			baseSchematic.push_back(vec3_ti(3, 0, 3));
			baseSchematic.push_back(vec3_ti(3, -1, 3));
			baseSchematic.push_back(vec3_ti(-1, -1, -2));
			baseSchematic.push_back(vec3_ti(-1, -1, -1));
			baseSchematic.push_back(vec3_ti(4, 0, -3));
			baseSchematic.push_back(vec3_ti(0, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, 1));
			baseSchematic.push_back(vec3_ti(6, -1, 2));
			baseSchematic.push_back(vec3_ti(0, -1, -3));
			baseSchematic.push_back(vec3_ti(0, -1, 3));
			baseSchematic.push_back(vec3_ti(4, 0, 3));
			baseSchematic.push_back(vec3_ti(1, -1, -3));
			baseSchematic.push_back(vec3_ti(1, -1, 3));
			baseSchematic.push_back(vec3_ti(5, 0, -3));
			baseSchematic.push_back(vec3_ti(1, -1, 0));
			baseSchematic.push_back(vec3_ti(5, 0, 3));
			baseSchematic.push_back(vec3_ti(2, -1, -3));
			baseSchematic.push_back(vec3_ti(2, -1, 0));
			baseSchematic.push_back(vec3_ti(4, -1, 2));
			baseSchematic.push_back(vec3_ti(2, -1, 3));
			baseSchematic.push_back(vec3_ti(3, -1, -1));
			baseSchematic.push_back(vec3_ti(0, 0, -3));
			baseSchematic.push_back(vec3_ti(0, 0, 3));
			baseSchematic.push_back(vec3_ti(5, -1, 2));
			baseSchematic.push_back(vec3_ti(1, 0, -3));
			baseSchematic.push_back(vec3_ti(3, -1, 2));
			baseSchematic.push_back(vec3_ti(2, 0, -3));
			baseSchematic.push_back(vec3_ti(1, 0, 3));
			baseSchematic.push_back(vec3_ti(6, -1, -2));
			baseSchematic.push_back(vec3_ti(-1, -1, -3));
			baseSchematic.push_back(vec3_ti(-1, -1, 3));
			baseSchematic.push_back(vec3_ti(6, -1, -1));
			baseSchematic.push_back(vec3_ti(6, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, -1, 0));
			baseSchematic.push_back(vec3_ti(2, 0, 3));
			baseSchematic.push_back(vec3_ti(0, -1, 2));
			baseSchematic.push_back(vec3_ti(1, -1, 2));
			baseSchematic.push_back(vec3_ti(4, -1, 1));
			baseSchematic.push_back(vec3_ti(2, -1, 2));
			baseSchematic.push_back(vec3_ti(4, -1, -2));
			baseSchematic.push_back(vec3_ti(-1, 0, -3));
			baseSchematic.push_back(vec3_ti(-1, 0, 3));
			baseSchematic.push_back(vec3_ti(4, -1, -1));
			baseSchematic.push_back(vec3_ti(5, -1, -2));
			baseSchematic.push_back(vec3_ti(5, -1, -1));
			baseSchematic.push_back(vec3_ti(3, -1, 1));
			baseSchematic.push_back(vec3_ti(5, -1, 1));
			baseSchematic.push_back(vec3_ti(3, -1, -2));
			baseSchematic.push_back(vec3_ti(6, -1, -3));

		}
		if (axis.selected == 1) {  // -X
			baseSchematic.push_back(vec3_ti(0, -1, -2));
			baseSchematic.push_back(vec3_ti(-6, 0, -3));
			baseSchematic.push_back(vec3_ti(0, -1, -1));
			baseSchematic.push_back(vec3_ti(0, -1, 1));
			baseSchematic.push_back(vec3_ti(-3, -1, -3));
			baseSchematic.push_back(vec3_ti(-2, -1, 2));
			baseSchematic.push_back(vec3_ti(-1, -1, 2));
			baseSchematic.push_back(vec3_ti(-6, 0, 3));
			baseSchematic.push_back(vec3_ti(-3, 0, -3));
			baseSchematic.push_back(vec3_ti(-3, -1, 0));
			baseSchematic.push_back(vec3_ti(1, -1, 1));
			baseSchematic.push_back(vec3_ti(-3, -1, 3));
			baseSchematic.push_back(vec3_ti(1, -1, -2));
			baseSchematic.push_back(vec3_ti(-5, 0, -3));
			baseSchematic.push_back(vec3_ti(-5, 0, 3));
			baseSchematic.push_back(vec3_ti(1, -1, -1));
			baseSchematic.push_back(vec3_ti(-3, 0, 3));
			baseSchematic.push_back(vec3_ti(-4, -1, 1));
			baseSchematic.push_back(vec3_ti(-6, -1, 2));
			baseSchematic.push_back(vec3_ti(-4, -1, -2));
			baseSchematic.push_back(vec3_ti(-4, -1, -1));
			baseSchematic.push_back(vec3_ti(-5, -1, 2));
			baseSchematic.push_back(vec3_ti(-2, -1, -2));
			baseSchematic.push_back(vec3_ti(-2, -1, -1));
			baseSchematic.push_back(vec3_ti(-1, -1, -2));
			baseSchematic.push_back(vec3_ti(-1, -1, -1));
			baseSchematic.push_back(vec3_ti(-2, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, -1, 1));
			baseSchematic.push_back(vec3_ti(0, -1, -3));
			baseSchematic.push_back(vec3_ti(0, -1, 0));
			baseSchematic.push_back(vec3_ti(0, -1, 3));
			baseSchematic.push_back(vec3_ti(-3, -1, 2));
			baseSchematic.push_back(vec3_ti(1, -1, -3));
			baseSchematic.push_back(vec3_ti(1, -1, 3));
			baseSchematic.push_back(vec3_ti(1, -1, 0));
			baseSchematic.push_back(vec3_ti(-4, -1, -3));
			baseSchematic.push_back(vec3_ti(-6, -1, -2));
			baseSchematic.push_back(vec3_ti(-6, -1, -1));
			baseSchematic.push_back(vec3_ti(0, 0, -3));
			baseSchematic.push_back(vec3_ti(-4, -1, 0));
			baseSchematic.push_back(vec3_ti(-6, -1, 1));
			baseSchematic.push_back(vec3_ti(0, 0, 3));
			baseSchematic.push_back(vec3_ti(-5, -1, -2));
			baseSchematic.push_back(vec3_ti(-5, -1, -1));
			baseSchematic.push_back(vec3_ti(-4, -1, 3));
			baseSchematic.push_back(vec3_ti(1, 0, -3));
			baseSchematic.push_back(vec3_ti(-5, -1, 1));
			baseSchematic.push_back(vec3_ti(1, 0, 3));
			baseSchematic.push_back(vec3_ti(-2, -1, -3));
			baseSchematic.push_back(vec3_ti(-2, -1, 3));
			baseSchematic.push_back(vec3_ti(-1, -1, -3));
			baseSchematic.push_back(vec3_ti(-4, 0, -3));
			baseSchematic.push_back(vec3_ti(-2, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, 3));
			baseSchematic.push_back(vec3_ti(0, -1, 2));
			baseSchematic.push_back(vec3_ti(-3, -1, -2));
			baseSchematic.push_back(vec3_ti(-3, -1, -1));
			baseSchematic.push_back(vec3_ti(-4, 0, 3));
			baseSchematic.push_back(vec3_ti(-3, -1, 1));
			baseSchematic.push_back(vec3_ti(1, -1, 2));
			baseSchematic.push_back(vec3_ti(-6, -1, -3));
			baseSchematic.push_back(vec3_ti(-2, 0, -3));
			baseSchematic.push_back(vec3_ti(-6, -1, 0));
			baseSchematic.push_back(vec3_ti(-2, 0, 3));
			baseSchematic.push_back(vec3_ti(-1, 0, -3));
			baseSchematic.push_back(vec3_ti(-4, -1, 2));
			baseSchematic.push_back(vec3_ti(-6, -1, 3));
			baseSchematic.push_back(vec3_ti(-1, 0, 3));
			baseSchematic.push_back(vec3_ti(-5, -1, 0));
			baseSchematic.push_back(vec3_ti(-5, -1, -3));
			baseSchematic.push_back(vec3_ti(-5, -1, 3));
		}
		if (axis.selected == 2) {  // +Z
			baseSchematic.push_back(vec3_ti(0, -1, -1));
			baseSchematic.push_back(vec3_ti(-2, -1, 5));
			baseSchematic.push_back(vec3_ti(-1, -1, 5));
			baseSchematic.push_back(vec3_ti(0, -1, 1));
			baseSchematic.push_back(vec3_ti(-2, -1, 2));
			baseSchematic.push_back(vec3_ti(-1, -1, 2));
			baseSchematic.push_back(vec3_ti(0, -1, 4));
			baseSchematic.push_back(vec3_ti(-3, -1, 0));
			baseSchematic.push_back(vec3_ti(1, -1, 1));
			baseSchematic.push_back(vec3_ti(-3, -1, 3));
			baseSchematic.push_back(vec3_ti(-3, 0, 0));
			baseSchematic.push_back(vec3_ti(-3, 0, 6));
			baseSchematic.push_back(vec3_ti(1, -1, -1));
			baseSchematic.push_back(vec3_ti(-3, -1, 6));
			baseSchematic.push_back(vec3_ti(-3, 0, 3));
			baseSchematic.push_back(vec3_ti(1, -1, 4));
			baseSchematic.push_back(vec3_ti(2, -1, -1));
			baseSchematic.push_back(vec3_ti(2, -1, 4));
			baseSchematic.push_back(vec3_ti(2, -1, 1));
			baseSchematic.push_back(vec3_ti(3, -1, 0));
			baseSchematic.push_back(vec3_ti(3, -1, 6));
			baseSchematic.push_back(vec3_ti(3, 0, 3));
			baseSchematic.push_back(vec3_ti(3, -1, 3));
			baseSchematic.push_back(vec3_ti(3, 0, 0));
			baseSchematic.push_back(vec3_ti(-2, -1, -1));
			baseSchematic.push_back(vec3_ti(-1, -1, -1));
			baseSchematic.push_back(vec3_ti(3, 0, 6));
			baseSchematic.push_back(vec3_ti(0, -1, 0));
			baseSchematic.push_back(vec3_ti(-2, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, -1, 1));
			baseSchematic.push_back(vec3_ti(0, -1, 3));
			baseSchematic.push_back(vec3_ti(-2, -1, 4));
			baseSchematic.push_back(vec3_ti(-1, -1, 4));
			baseSchematic.push_back(vec3_ti(0, -1, 6));
			baseSchematic.push_back(vec3_ti(-3, -1, 2));
			baseSchematic.push_back(vec3_ti(1, -1, 3));
			baseSchematic.push_back(vec3_ti(-3, -1, 5));
			baseSchematic.push_back(vec3_ti(-3, 0, 2));
			baseSchematic.push_back(vec3_ti(1, -1, 0));
			baseSchematic.push_back(vec3_ti(1, -1, 6));
			baseSchematic.push_back(vec3_ti(-3, 0, 5));
			baseSchematic.push_back(vec3_ti(2, -1, 0));
			baseSchematic.push_back(vec3_ti(2, -1, 3));
			baseSchematic.push_back(vec3_ti(2, -1, 6));
			baseSchematic.push_back(vec3_ti(3, -1, 2));
			baseSchematic.push_back(vec3_ti(3, -1, 5));
			baseSchematic.push_back(vec3_ti(3, 0, 2));
			baseSchematic.push_back(vec3_ti(3, 0, 5));
			baseSchematic.push_back(vec3_ti(-2, -1, 3));
			baseSchematic.push_back(vec3_ti(-1, -1, 3));
			baseSchematic.push_back(vec3_ti(-2, -1, 0));
			baseSchematic.push_back(vec3_ti(-2, -1, 6));
			baseSchematic.push_back(vec3_ti(-1, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, 6));
			baseSchematic.push_back(vec3_ti(-3, -1, -1));
			baseSchematic.push_back(vec3_ti(-3, -1, 4));
			baseSchematic.push_back(vec3_ti(0, -1, 2));
			baseSchematic.push_back(vec3_ti(0, -1, 5));
			baseSchematic.push_back(vec3_ti(-3, -1, 1));
			baseSchematic.push_back(vec3_ti(-3, 0, -1));
			baseSchematic.push_back(vec3_ti(-3, 0, 4));
			baseSchematic.push_back(vec3_ti(1, -1, 2));
			baseSchematic.push_back(vec3_ti(-3, 0, 1));
			baseSchematic.push_back(vec3_ti(1, -1, 5));
			baseSchematic.push_back(vec3_ti(2, -1, 2));
			baseSchematic.push_back(vec3_ti(2, -1, 5));
			baseSchematic.push_back(vec3_ti(3, -1, 1));
			baseSchematic.push_back(vec3_ti(3, -1, -1));
			baseSchematic.push_back(vec3_ti(3, -1, 4));
			baseSchematic.push_back(vec3_ti(3, 0, 1));
			baseSchematic.push_back(vec3_ti(3, 0, -1));
			baseSchematic.push_back(vec3_ti(3, 0, 4));
		}
		if (axis.selected == 3) {  // -Z
			baseSchematic.push_back(vec3_ti(0, -1, -2));
			baseSchematic.push_back(vec3_ti(0, -1, -1));
			baseSchematic.push_back(vec3_ti(0, -1, 1));
			baseSchematic.push_back(vec3_ti(-3, -1, -3));
			baseSchematic.push_back(vec3_ti(-3, -1, -6));
			baseSchematic.push_back(vec3_ti(-3, -1, 0));
			baseSchematic.push_back(vec3_ti(-3, 0, -3));
			baseSchematic.push_back(vec3_ti(1, -1, -5));
			baseSchematic.push_back(vec3_ti(1, -1, 1));
			baseSchematic.push_back(vec3_ti(-3, 0, -6));
			baseSchematic.push_back(vec3_ti(-3, 0, 0));
			baseSchematic.push_back(vec3_ti(1, -1, -2));
			baseSchematic.push_back(vec3_ti(1, -1, -1));
			baseSchematic.push_back(vec3_ti(2, -1, -5));
			baseSchematic.push_back(vec3_ti(2, -1, -2));
			baseSchematic.push_back(vec3_ti(2, -1, -1));
			baseSchematic.push_back(vec3_ti(2, -1, 1));
			baseSchematic.push_back(vec3_ti(3, -1, -6));
			baseSchematic.push_back(vec3_ti(3, -1, -3));
			baseSchematic.push_back(vec3_ti(3, 0, -6));
			baseSchematic.push_back(vec3_ti(3, -1, 0));
			baseSchematic.push_back(vec3_ti(3, 0, -3));
			baseSchematic.push_back(vec3_ti(3, 0, 0));
			baseSchematic.push_back(vec3_ti(-2, -1, -2));
			baseSchematic.push_back(vec3_ti(-2, -1, -1));
			baseSchematic.push_back(vec3_ti(-1, -1, -2));
			baseSchematic.push_back(vec3_ti(-2, -1, -5));
			baseSchematic.push_back(vec3_ti(-2, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, -1, -5));
			baseSchematic.push_back(vec3_ti(-1, -1, -1));
			baseSchematic.push_back(vec3_ti(-1, -1, 1));
			baseSchematic.push_back(vec3_ti(0, -1, -6));
			baseSchematic.push_back(vec3_ti(0, -1, -3));
			baseSchematic.push_back(vec3_ti(0, -1, 0));
			baseSchematic.push_back(vec3_ti(-3, -1, -4));
			baseSchematic.push_back(vec3_ti(1, -1, -6));
			baseSchematic.push_back(vec3_ti(1, -1, -3));
			baseSchematic.push_back(vec3_ti(-3, 0, -4));
			baseSchematic.push_back(vec3_ti(1, -1, 0));
			baseSchematic.push_back(vec3_ti(2, -1, -3));
			baseSchematic.push_back(vec3_ti(2, -1, -6));
			baseSchematic.push_back(vec3_ti(2, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, -4));
			baseSchematic.push_back(vec3_ti(3, -1, -1));
			baseSchematic.push_back(vec3_ti(3, -1, -4));
			baseSchematic.push_back(vec3_ti(3, 0, -2));
			baseSchematic.push_back(vec3_ti(3, 0, -1));
			baseSchematic.push_back(vec3_ti(3, 0, -4));
			baseSchematic.push_back(vec3_ti(-2, -1, -6));
			baseSchematic.push_back(vec3_ti(-1, -1, -6));
			baseSchematic.push_back(vec3_ti(0, -1, -4));
			baseSchematic.push_back(vec3_ti(-2, -1, -3));
			baseSchematic.push_back(vec3_ti(-1, -1, -3));
			baseSchematic.push_back(vec3_ti(-3, -1, -5));
			baseSchematic.push_back(vec3_ti(-2, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, 0));
			baseSchematic.push_back(vec3_ti(1, -1, -4));
			baseSchematic.push_back(vec3_ti(-3, -1, -1));
			baseSchematic.push_back(vec3_ti(-3, 0, -5));
			baseSchematic.push_back(vec3_ti(-3, -1, -2));
			baseSchematic.push_back(vec3_ti(-3, -1, 1));
			baseSchematic.push_back(vec3_ti(-3, 0, -2));
			baseSchematic.push_back(vec3_ti(-3, 0, -1));
			baseSchematic.push_back(vec3_ti(-3, 0, 1));
			baseSchematic.push_back(vec3_ti(2, -1, -4));
			baseSchematic.push_back(vec3_ti(3, -1, -5));
			baseSchematic.push_back(vec3_ti(3, -1, 1));
			baseSchematic.push_back(vec3_ti(3, -1, -2));
			baseSchematic.push_back(vec3_ti(3, 0, -5));
			baseSchematic.push_back(vec3_ti(3, 0, 1));
			baseSchematic.push_back(vec3_ti(0, -1, -5));
			baseSchematic.push_back(vec3_ti(-2, -1, -4));
		}
	}
	if (block.selected == 1) {  // Diagonal
		if (axis.selected == 4) {  // +X +Z
			baseSchematic.push_back(vec3_ti(6, -1, 3));
			baseSchematic.push_back(vec3_ti(1, -1, 1));
			baseSchematic.push_back(vec3_ti(2, -1, 1));
			baseSchematic.push_back(vec3_ti(4, -1, 3));
			baseSchematic.push_back(vec3_ti(3, -1, 6));
			baseSchematic.push_back(vec3_ti(6, -1, 5));
			baseSchematic.push_back(vec3_ti(6, 0, 2));
			baseSchematic.push_back(vec3_ti(8, 0, 4));
			baseSchematic.push_back(vec3_ti(1, -1, 3));
			baseSchematic.push_back(vec3_ti(2, -1, 3));
			baseSchematic.push_back(vec3_ti(4, -1, 5));
			baseSchematic.push_back(vec3_ti(-1, -1, 0));
			baseSchematic.push_back(vec3_ti(-2, -1, 0));
			baseSchematic.push_back(vec3_ti(0, -1, 2));
			baseSchematic.push_back(vec3_ti(2, -1, 5));
			baseSchematic.push_back(vec3_ti(4, -1, 7));
			baseSchematic.push_back(vec3_ti(3, -1, 1));
			baseSchematic.push_back(vec3_ti(5, -1, 4));
			baseSchematic.push_back(vec3_ti(-2, -1, 2));
			baseSchematic.push_back(vec3_ti(-1, -1, 2));
			baseSchematic.push_back(vec3_ti(0, -1, 4));
			baseSchematic.push_back(vec3_ti(2, -1, -2));
			baseSchematic.push_back(vec3_ti(2, -1, -1));
			baseSchematic.push_back(vec3_ti(4, -1, 0));
			baseSchematic.push_back(vec3_ti(-2, 0, 2));
			baseSchematic.push_back(vec3_ti(0, 0, 4));
			baseSchematic.push_back(vec3_ti(5, -1, 6));
			baseSchematic.push_back(vec3_ti(2, 0, -2));
			baseSchematic.push_back(vec3_ti(6, -1, 2));
			baseSchematic.push_back(vec3_ti(4, 0, 0));
			baseSchematic.push_back(vec3_ti(8, -1, 4));
			baseSchematic.push_back(vec3_ti(2, -1, 0));
			baseSchematic.push_back(vec3_ti(4, -1, 2));
			baseSchematic.push_back(vec3_ti(6, -1, 4));
			baseSchematic.push_back(vec3_ti(-3, 0, 1));
			baseSchematic.push_back(vec3_ti(1, -1, 5));
			baseSchematic.push_back(vec3_ti(2, -1, 2));
			baseSchematic.push_back(vec3_ti(4, -1, 4));
			baseSchematic.push_back(vec3_ti(5, -1, 1));
			baseSchematic.push_back(vec3_ti(7, -1, 3));
			baseSchematic.push_back(vec3_ti(3, 0, -1));
			baseSchematic.push_back(vec3_ti(1, 0, 5));
			baseSchematic.push_back(vec3_ti(3, 0, 7));
			baseSchematic.push_back(vec3_ti(6, -1, 6));
			baseSchematic.push_back(vec3_ti(0, -1, 1));
			baseSchematic.push_back(vec3_ti(1, -1, -2));
			baseSchematic.push_back(vec3_ti(1, -1, -1));
			baseSchematic.push_back(vec3_ti(5, 0, 1));
			baseSchematic.push_back(vec3_ti(7, 0, 3));
			baseSchematic.push_back(vec3_ti(2, -1, 4));
			baseSchematic.push_back(vec3_ti(4, -1, 6));
			baseSchematic.push_back(vec3_ti(5, -1, 3));
			baseSchematic.push_back(vec3_ti(7, -1, 5));
			baseSchematic.push_back(vec3_ti(3, -1, 3));
			baseSchematic.push_back(vec3_ti(-2, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, -1, 1));
			baseSchematic.push_back(vec3_ti(0, -1, 3));
			baseSchematic.push_back(vec3_ti(1, -1, 0));
			baseSchematic.push_back(vec3_ti(5, -1, 5));
			baseSchematic.push_back(vec3_ti(3, -1, 5));
			baseSchematic.push_back(vec3_ti(-1, -1, 3));
			baseSchematic.push_back(vec3_ti(-3, -1, 1));
			baseSchematic.push_back(vec3_ti(1, -1, 2));
			baseSchematic.push_back(vec3_ti(4, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, 0, 3));
			baseSchematic.push_back(vec3_ti(5, -1, 7));
			baseSchematic.push_back(vec3_ti(3, -1, -1));
			baseSchematic.push_back(vec3_ti(3, -1, 7));
			baseSchematic.push_back(vec3_ti(0, -1, -2));
			baseSchematic.push_back(vec3_ti(0, -1, -1));
			baseSchematic.push_back(vec3_ti(1, -1, 4));
			baseSchematic.push_back(vec3_ti(3, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, -1));
			baseSchematic.push_back(vec3_ti(0, -1, 0));
			baseSchematic.push_back(vec3_ti(1, -1, -3));
			baseSchematic.push_back(vec3_ti(2, -1, 6));
			baseSchematic.push_back(vec3_ti(5, -1, 2));
			baseSchematic.push_back(vec3_ti(4, -1, 8));
			baseSchematic.push_back(vec3_ti(1, 0, -3));
			baseSchematic.push_back(vec3_ti(7, -1, 4));
			baseSchematic.push_back(vec3_ti(3, -1, 2));
			baseSchematic.push_back(vec3_ti(2, 0, 6));
			baseSchematic.push_back(vec3_ti(4, 0, 8));
			baseSchematic.push_back(vec3_ti(3, -1, 4));
		}
		if (axis.selected == 5) {  // -X -Z
			baseSchematic.push_back(vec3_ti(-3, -1, 0));
			baseSchematic.push_back(vec3_ti(1, -1, 1));
			baseSchematic.push_back(vec3_ti(-4, -1, -8));
			baseSchematic.push_back(vec3_ti(-7, -1, -3));
			baseSchematic.push_back(vec3_ti(-4, 0, -8));
			baseSchematic.push_back(vec3_ti(-3, -1, -7));
			baseSchematic.push_back(vec3_ti(-4, -1, -6));
			baseSchematic.push_back(vec3_ti(-3, -1, -5));
			baseSchematic.push_back(vec3_ti(-2, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, 0));
			baseSchematic.push_back(vec3_ti(0, -1, 2));
			baseSchematic.push_back(vec3_ti(-4, -1, -4));
			baseSchematic.push_back(vec3_ti(-5, -1, -6));
			baseSchematic.push_back(vec3_ti(-3, -1, -3));
			baseSchematic.push_back(vec3_ti(-2, -1, 2));
			baseSchematic.push_back(vec3_ti(-1, -1, 2));
			baseSchematic.push_back(vec3_ti(2, -1, -2));
			baseSchematic.push_back(vec3_ti(2, -1, -1));
			baseSchematic.push_back(vec3_ti(-2, 0, 2));
			baseSchematic.push_back(vec3_ti(-5, -1, -4));
			baseSchematic.push_back(vec3_ti(-2, -1, -5));
			baseSchematic.push_back(vec3_ti(-1, -1, -5));
			baseSchematic.push_back(vec3_ti(2, 0, -2));
			baseSchematic.push_back(vec3_ti(0, -1, -3));
			baseSchematic.push_back(vec3_ti(2, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, 0, -5));
			baseSchematic.push_back(vec3_ti(-7, -1, -4));
			baseSchematic.push_back(vec3_ti(-5, -1, -2));
			baseSchematic.push_back(vec3_ti(-5, -1, -1));
			baseSchematic.push_back(vec3_ti(-2, -1, -3));
			baseSchematic.push_back(vec3_ti(-1, -1, -3));
			baseSchematic.push_back(vec3_ti(-5, 0, -1));
			baseSchematic.push_back(vec3_ti(-3, 0, 1));
			baseSchematic.push_back(vec3_ti(-6, -1, -6));
			baseSchematic.push_back(vec3_ti(3, 0, -1));
			baseSchematic.push_back(vec3_ti(0, -1, 1));
			baseSchematic.push_back(vec3_ti(1, -1, -2));
			baseSchematic.push_back(vec3_ti(1, -1, -1));
			baseSchematic.push_back(vec3_ti(-6, -1, -4));
			baseSchematic.push_back(vec3_ti(-5, -1, -7));
			baseSchematic.push_back(vec3_ti(-4, -1, -2));
			baseSchematic.push_back(vec3_ti(-4, -1, -1));
			baseSchematic.push_back(vec3_ti(-2, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, -1, 1));
			baseSchematic.push_back(vec3_ti(1, -1, 0));
			baseSchematic.push_back(vec3_ti(-6, -1, -2));
			baseSchematic.push_back(vec3_ti(-8, 0, -4));
			baseSchematic.push_back(vec3_ti(-5, -1, -5));
			baseSchematic.push_back(vec3_ti(-4, -1, 0));
			baseSchematic.push_back(vec3_ti(-2, -1, -6));
			baseSchematic.push_back(vec3_ti(0, -1, -4));
			baseSchematic.push_back(vec3_ti(-1, -1, 3));
			baseSchematic.push_back(vec3_ti(-6, 0, -2));
			baseSchematic.push_back(vec3_ti(-4, 0, 0));
			baseSchematic.push_back(vec3_ti(-3, -1, 1));
			baseSchematic.push_back(vec3_ti(-2, 0, -6));
			baseSchematic.push_back(vec3_ti(-4, -1, -7));
			baseSchematic.push_back(vec3_ti(0, 0, -4));
			baseSchematic.push_back(vec3_ti(-1, 0, 3));
			baseSchematic.push_back(vec3_ti(3, -1, -1));
			baseSchematic.push_back(vec3_ti(-1, -1, -4));
			baseSchematic.push_back(vec3_ti(-2, -1, -4));
			baseSchematic.push_back(vec3_ti(0, -1, -2));
			baseSchematic.push_back(vec3_ti(0, -1, -1));
			baseSchematic.push_back(vec3_ti(-3, -1, -6));
			baseSchematic.push_back(vec3_ti(-4, -1, -5));
			baseSchematic.push_back(vec3_ti(-7, 0, -3));
			baseSchematic.push_back(vec3_ti(-2, -1, -2));
			baseSchematic.push_back(vec3_ti(-2, -1, -1));
			baseSchematic.push_back(vec3_ti(-1, -1, -2));
			baseSchematic.push_back(vec3_ti(0, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, -1));
			baseSchematic.push_back(vec3_ti(-3, -1, -4));
			baseSchematic.push_back(vec3_ti(-3, 0, -7));
			baseSchematic.push_back(vec3_ti(1, -1, -3));
			baseSchematic.push_back(vec3_ti(-6, -1, -5));
			baseSchematic.push_back(vec3_ti(-8, -1, -4));
			baseSchematic.push_back(vec3_ti(-4, -1, -3));
			baseSchematic.push_back(vec3_ti(1, 0, -3));
			baseSchematic.push_back(vec3_ti(-3, -1, -2));
			baseSchematic.push_back(vec3_ti(-3, -1, -1));
			baseSchematic.push_back(vec3_ti(-6, -1, -3));
			baseSchematic.push_back(vec3_ti(-7, -1, -5));
			baseSchematic.push_back(vec3_ti(-5, -1, -3));
		}
		if (axis.selected == 6) {  // +X -Z
			baseSchematic.push_back(vec3_ti(6, -1, -6));
			baseSchematic.push_back(vec3_ti(1, -1, 1));
			baseSchematic.push_back(vec3_ti(2, -1, 1));
			baseSchematic.push_back(vec3_ti(3, -1, -3));
			baseSchematic.push_back(vec3_ti(6, -1, -4));
			baseSchematic.push_back(vec3_ti(1, -1, 3));
			baseSchematic.push_back(vec3_ti(2, -1, -6));
			baseSchematic.push_back(vec3_ti(4, -1, -4));
			baseSchematic.push_back(vec3_ti(-2, 0, -2));
			baseSchematic.push_back(vec3_ti(2, 0, -6));
			baseSchematic.push_back(vec3_ti(6, -1, -2));
			baseSchematic.push_back(vec3_ti(-1, -1, 0));
			baseSchematic.push_back(vec3_ti(-2, -1, 0));
			baseSchematic.push_back(vec3_ti(1, -1, -4));
			baseSchematic.push_back(vec3_ti(0, -1, 2));
			baseSchematic.push_back(vec3_ti(2, -1, -4));
			baseSchematic.push_back(vec3_ti(4, -1, -2));
			baseSchematic.push_back(vec3_ti(4, -1, -1));
			baseSchematic.push_back(vec3_ti(3, -1, 1));
			baseSchematic.push_back(vec3_ti(7, 0, -3));
			baseSchematic.push_back(vec3_ti(2, -1, -2));
			baseSchematic.push_back(vec3_ti(2, -1, -1));
			baseSchematic.push_back(vec3_ti(4, -1, 0));
			baseSchematic.push_back(vec3_ti(3, -1, -6));
			baseSchematic.push_back(vec3_ti(5, -1, -3));
			baseSchematic.push_back(vec3_ti(0, -1, -3));
			baseSchematic.push_back(vec3_ti(4, 0, 0));
			baseSchematic.push_back(vec3_ti(4, -1, -7));
			baseSchematic.push_back(vec3_ti(2, -1, 0));
			baseSchematic.push_back(vec3_ti(3, -1, -4));
			baseSchematic.push_back(vec3_ti(6, -1, -5));
			baseSchematic.push_back(vec3_ti(1, 0, 3));
			baseSchematic.push_back(vec3_ti(-1, -1, -3));
			baseSchematic.push_back(vec3_ti(4, -1, -5));
			baseSchematic.push_back(vec3_ti(2, -1, 2));
			baseSchematic.push_back(vec3_ti(-1, 0, -3));
			baseSchematic.push_back(vec3_ti(6, -1, -3));
			baseSchematic.push_back(vec3_ti(8, 0, -4));
			baseSchematic.push_back(vec3_ti(2, 0, 2));
			baseSchematic.push_back(vec3_ti(0, -1, 1));
			baseSchematic.push_back(vec3_ti(1, -1, -2));
			baseSchematic.push_back(vec3_ti(1, -1, -1));
			baseSchematic.push_back(vec3_ti(2, -1, -5));
			baseSchematic.push_back(vec3_ti(4, -1, -3));
			baseSchematic.push_back(vec3_ti(5, -1, -6));
			baseSchematic.push_back(vec3_ti(7, -1, -4));
			baseSchematic.push_back(vec3_ti(-1, -1, 1));
			baseSchematic.push_back(vec3_ti(1, -1, 0));
			baseSchematic.push_back(vec3_ti(2, -1, -3));
			baseSchematic.push_back(vec3_ti(5, -1, -4));
			baseSchematic.push_back(vec3_ti(3, 0, -7));
			baseSchematic.push_back(vec3_ti(0, -1, -4));
			baseSchematic.push_back(vec3_ti(-3, 0, -1));
			baseSchematic.push_back(vec3_ti(1, -1, 2));
			baseSchematic.push_back(vec3_ti(4, -1, -8));
			baseSchematic.push_back(vec3_ti(0, 0, -4));
			baseSchematic.push_back(vec3_ti(5, -1, -2));
			baseSchematic.push_back(vec3_ti(5, -1, -1));
			baseSchematic.push_back(vec3_ti(3, -1, -1));
			baseSchematic.push_back(vec3_ti(3, -1, -2));
			baseSchematic.push_back(vec3_ti(4, 0, -8));
			baseSchematic.push_back(vec3_ti(8, -1, -4));
			baseSchematic.push_back(vec3_ti(0, -1, -2));
			baseSchematic.push_back(vec3_ti(0, -1, -1));
			baseSchematic.push_back(vec3_ti(1, -1, -5));
			baseSchematic.push_back(vec3_ti(5, 0, -1));
			baseSchematic.push_back(vec3_ti(4, -1, -6));
			baseSchematic.push_back(vec3_ti(1, 0, -5));
			baseSchematic.push_back(vec3_ti(3, -1, 0));
			baseSchematic.push_back(vec3_ti(-2, -1, -2));
			baseSchematic.push_back(vec3_ti(-2, -1, -1));
			baseSchematic.push_back(vec3_ti(-1, -1, -2));
			baseSchematic.push_back(vec3_ti(-1, -1, -1));
			baseSchematic.push_back(vec3_ti(0, -1, 0));
			baseSchematic.push_back(vec3_ti(1, -1, -3));
			baseSchematic.push_back(vec3_ti(5, -1, -7));
			baseSchematic.push_back(vec3_ti(7, -1, -5));
			baseSchematic.push_back(vec3_ti(3, -1, -7));
			baseSchematic.push_back(vec3_ti(6, 0, -2));
			baseSchematic.push_back(vec3_ti(-3, -1, -1));
			baseSchematic.push_back(vec3_ti(5, -1, -5));
			baseSchematic.push_back(vec3_ti(7, -1, -3));
			baseSchematic.push_back(vec3_ti(3, -1, -5));
			baseSchematic.push_back(vec3_ti(3, 0, 1));
		}
		if (axis.selected == 7) {  // -X +Z
			baseSchematic.push_back(vec3_ti(-3, -1, 0));
			baseSchematic.push_back(vec3_ti(1, -1, 1));
			baseSchematic.push_back(vec3_ti(-4, -1, 1));
			baseSchematic.push_back(vec3_ti(2, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, 0, 5));
			baseSchematic.push_back(vec3_ti(-7, 0, 3));
			baseSchematic.push_back(vec3_ti(-3, -1, 2));
			baseSchematic.push_back(vec3_ti(1, -1, 3));
			baseSchematic.push_back(vec3_ti(-2, 0, -2));
			baseSchematic.push_back(vec3_ti(-5, -1, 1));
			baseSchematic.push_back(vec3_ti(-2, -1, 0));
			baseSchematic.push_back(vec3_ti(-1, -1, 0));
			baseSchematic.push_back(vec3_ti(0, -1, 2));
			baseSchematic.push_back(vec3_ti(-3, -1, 4));
			baseSchematic.push_back(vec3_ti(-5, 0, 1));
			baseSchematic.push_back(vec3_ti(-8, -1, 4));
			baseSchematic.push_back(vec3_ti(3, -1, 1));
			baseSchematic.push_back(vec3_ti(-5, -1, 3));
			baseSchematic.push_back(vec3_ti(-2, -1, 2));
			baseSchematic.push_back(vec3_ti(-1, -1, 2));
			baseSchematic.push_back(vec3_ti(0, -1, 4));
			baseSchematic.push_back(vec3_ti(-4, 0, 8));
			baseSchematic.push_back(vec3_ti(0, 0, 4));
			baseSchematic.push_back(vec3_ti(-7, -1, 3));
			baseSchematic.push_back(vec3_ti(-5, -1, 5));
			baseSchematic.push_back(vec3_ti(-2, -1, 4));
			baseSchematic.push_back(vec3_ti(-1, -1, 4));
			baseSchematic.push_back(vec3_ti(2, -1, 0));
			baseSchematic.push_back(vec3_ti(-4, -1, 3));
			baseSchematic.push_back(vec3_ti(-7, -1, 5));
			baseSchematic.push_back(vec3_ti(-5, -1, 7));
			baseSchematic.push_back(vec3_ti(1, 0, 3));
			baseSchematic.push_back(vec3_ti(-1, -1, -3));
			baseSchematic.push_back(vec3_ti(-2, -1, 6));
			baseSchematic.push_back(vec3_ti(2, -1, 2));
			baseSchematic.push_back(vec3_ti(-1, 0, -3));
			baseSchematic.push_back(vec3_ti(-6, -1, 3));
			baseSchematic.push_back(vec3_ti(-2, 0, 6));
			baseSchematic.push_back(vec3_ti(-4, -1, 5));
			baseSchematic.push_back(vec3_ti(2, 0, 2));
			baseSchematic.push_back(vec3_ti(0, -1, 1));
			baseSchematic.push_back(vec3_ti(1, -1, -1));
			baseSchematic.push_back(vec3_ti(-3, -1, 6));
			baseSchematic.push_back(vec3_ti(-6, -1, 5));
			baseSchematic.push_back(vec3_ti(-5, -1, 2));
			baseSchematic.push_back(vec3_ti(-4, -1, 7));
			baseSchematic.push_back(vec3_ti(-2, -1, 1));
			baseSchematic.push_back(vec3_ti(-1, -1, 1));
			baseSchematic.push_back(vec3_ti(0, -1, 3));
			baseSchematic.push_back(vec3_ti(1, -1, 0));
			baseSchematic.push_back(vec3_ti(-4, -1, 0));
			baseSchematic.push_back(vec3_ti(-2, -1, 3));
			baseSchematic.push_back(vec3_ti(-1, -1, 3));
			baseSchematic.push_back(vec3_ti(-4, 0, 0));
			baseSchematic.push_back(vec3_ti(-3, -1, 1));
			baseSchematic.push_back(vec3_ti(-3, 0, -1));
			baseSchematic.push_back(vec3_ti(1, -1, 2));
			baseSchematic.push_back(vec3_ti(-3, 0, 7));
			baseSchematic.push_back(vec3_ti(-4, -1, 2));
			baseSchematic.push_back(vec3_ti(0, -1, -2));
			baseSchematic.push_back(vec3_ti(0, -1, -1));
			baseSchematic.push_back(vec3_ti(-2, -1, 5));
			baseSchematic.push_back(vec3_ti(-1, -1, 5));
			baseSchematic.push_back(vec3_ti(-3, -1, 3));
			baseSchematic.push_back(vec3_ti(-6, -1, 2));
			baseSchematic.push_back(vec3_ti(-4, -1, 4));
			baseSchematic.push_back(vec3_ti(-2, -1, -2));
			baseSchematic.push_back(vec3_ti(-2, -1, -1));
			baseSchematic.push_back(vec3_ti(-1, -1, -2));
			baseSchematic.push_back(vec3_ti(-1, -1, -1));
			baseSchematic.push_back(vec3_ti(0, -1, 0));
			baseSchematic.push_back(vec3_ti(-6, 0, 2));
			baseSchematic.push_back(vec3_ti(-3, -1, 5));
			baseSchematic.push_back(vec3_ti(-6, -1, 4));
			baseSchematic.push_back(vec3_ti(-4, -1, 6));
			baseSchematic.push_back(vec3_ti(-5, -1, 4));
			baseSchematic.push_back(vec3_ti(-3, -1, -1));
			baseSchematic.push_back(vec3_ti(-3, -1, 7));
			baseSchematic.push_back(vec3_ti(-6, -1, 6));
			baseSchematic.push_back(vec3_ti(-8, 0, 4));
			baseSchematic.push_back(vec3_ti(-4, -1, 8));
			baseSchematic.push_back(vec3_ti(3, 0, 1));
			baseSchematic.push_back(vec3_ti(-7, -1, 4));
			baseSchematic.push_back(vec3_ti(-5, -1, 6));
		}
	}
}*/








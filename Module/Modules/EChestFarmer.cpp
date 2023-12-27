#include "EChestFarmer.h"
bool echest = false;

EChestFarmer::EChestFarmer() : IModule(0x0, Category::MISC, "Destroys all Ender chests around you") {
	registerBoolSetting("AutoPickaxe", &autopickaxe, true);
	registerBoolSetting("Echests", &echest, true);
	registerIntSetting("Range", &range, range, 1, 10);
}

EChestFarmer::~EChestFarmer() {
}

const char* EChestFarmer::getModuleName() {
	return "E-Chest Farmer";
}

void EChestFarmer::onTick(C_GameMode* gm) {
	vec3_t* pos = gm->player->getPos();
	for (int x = (int)pos->x - range; x < pos->x + range; x++) {
		for (int z = (int)pos->z - range; z < pos->z + range; z++) {
			for (int y = (int)pos->y - range; y < pos->y + range; y++) {
				vec3_ti blockPos = vec3_ti(x, y, z);
				bool destroy = false;
				bool eat = false;
				auto id = gm->player->region->getBlock(blockPos)->toLegacy()->blockId;

				if (id == 130 && echest) destroy = true;

				if (destroy) {
					gm->destroyBlock(&blockPos, 0);
					g_Data.getLocalPlayer()->swingArm();
					return;
				}

				if (eat) {
					bool idk = true;
					gm->buildBlock(&blockPos, 0, idk);
					g_Data.getLocalPlayer()->swingArm();
					return;
				}
			}
		}
	}
}
void EChestFarmer::onEnable() {
	if (autopickaxe) {
		auto inv = g_Data.getLocalPlayer()->getSupplies()->inventory;
		for (int i = 0; i < 9; i++) {
			C_ItemStack* stack = inv->getItemStack(i);
			if (stack->item != nullptr) {
				if (stack->getItem()->itemId == 318, 606) {
					g_Data.getLocalPlayer()->getSupplies()->selectedHotbarSlot = i;
					return;
				}
			}
		}
	}
}
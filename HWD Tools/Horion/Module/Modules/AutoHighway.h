#pragma once
#include <vector>
#include "../../../Utils/HMath.h" //change the locations depending on the client
#include "../ModuleManager.h"
#include "Module.h"
#include "../../DrawUtils.h"
class AutoHighway : public IModule {
private:
	vec3_t startBuildPos;
	std::vector<vec3_t> schematic;
	std::vector<vec3_t> currentSchematic;
	int ogAutoHighwaySlot = 0;
	int ogAutoBuildSlot = 0;
	bool setAutoBuildSlot = false;
	bool setAutoHighwaySlot = false;
	int noMultiRenderSoLessLag = 0;
	int Odelay = 0;
	bool xplus = false;
	bool zplus = false;
	bool xminus = false;
	bool zminus = false;
	bool railings = true;
	int visualrange2 = 3;
	bool visualize = true;
	int length = 10000;
	int width = 5;
public:
	AutoHighway() : IModule(0x0, Category::PLAYER, "Automaticly builds a highway") {
		registerBoolSetting("Visuals", &visualize, visualize);
		registerBoolSetting("Railings", &railings, railings);
		registerIntSetting("Width", &width, width, 3, 5);
		registerIntSetting("Length", &length, length, 1, 10000);
		registerBoolSetting("X+", &xplus, xplus);
		registerBoolSetting("X-", &xminus, xminus);
		registerBoolSetting("Z+", &zplus, zplus);
		registerBoolSetting("Z-", &zminus, zminus);

	};
	~AutoHighway() {};

	virtual const char* getModuleName() override {
		return "AutoHighway";
	}
	void getObby6() {
		auto supplies = g_Data.getLocalPlayer()->getSupplies();
		auto inv = supplies->inventory;  // g_Data.getLocalPlayer()->getSupplies()->inventory->getItemStack(g_Data.getLocalPlayer()->getSupplies())->getItem()->itemID
		for (int n = 0; n < 9; n++) {
			C_ItemStack* stack = inv->getItemStack(n);
			if (stack->item != nullptr) {
				if (stack->getItem()->itemId == 49) {  // select obsid
					supplies->selectedHotbarSlot = n;
					return;
				}
			}
		}
	}
	void onEnable() {
		getObby6();
		setAutoBuildSlot = false;
		schematic.clear();
		if (xplus && zplus) {
			for (int ii = 0; ii < length; ii++) {
				if (width == 5) {
					schematic.push_back(vec3_t(ii, -1, ii - 2));
					schematic.push_back(vec3_t(ii, -1, ii - 1));
					schematic.push_back(vec3_t(ii, -1, ii));
					schematic.push_back(vec3_t(ii, -1, ii + 2));
					schematic.push_back(vec3_t(ii, -1, ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(ii, 0, ii - 3));
						schematic.push_back(vec3_t(ii, 0, ii + 3));
						schematic.push_back(vec3_t(ii, -1, ii + 3));
						schematic.push_back(vec3_t(ii, -1, ii - 3));
					}
				}
				if (width == 4) {
					schematic.push_back(vec3_t(ii, -1, ii - 1));
					schematic.push_back(vec3_t(ii, -1, ii));
					schematic.push_back(vec3_t(ii, -1, ii + 2));
					schematic.push_back(vec3_t(ii, -1, ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(ii, 0, ii - 2));
						schematic.push_back(vec3_t(ii, 0, ii + 3));
						schematic.push_back(vec3_t(ii, -1, ii + 3));
						schematic.push_back(vec3_t(ii, -1, ii - 2));
					}
				}
				if (width == 3) {
					schematic.push_back(vec3_t(ii, -1, ii - 1));
					schematic.push_back(vec3_t(ii, -1, ii));
					schematic.push_back(vec3_t(ii, -1, ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(ii, 0, ii - 2));
						schematic.push_back(vec3_t(ii, 0, ii + 2));
						schematic.push_back(vec3_t(ii, -1, ii + 2));
						schematic.push_back(vec3_t(ii, -1, ii - 2));
					}
				}
			}
		}
		else if (xplus && zminus) {
			for (int ii = 0; ii < length; ii++) {
				if (width == 5) {
					schematic.push_back(vec3_t(ii, -1, -ii - 2));
					schematic.push_back(vec3_t(ii, -1, -ii - 1));
					schematic.push_back(vec3_t(ii, -1, -ii));
					schematic.push_back(vec3_t(ii, -1, -ii + 2));
					schematic.push_back(vec3_t(ii, -1, -ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(ii, 0, -ii - 3));
						schematic.push_back(vec3_t(ii, 0, -ii + 3));
						schematic.push_back(vec3_t(ii, -1, -ii + 3));
						schematic.push_back(vec3_t(ii, -1, -ii - 3));
					}
				}
				if (width == 4) {
					schematic.push_back(vec3_t(ii, -1, -ii - 1));
					schematic.push_back(vec3_t(ii, -1, -ii));
					schematic.push_back(vec3_t(ii, -1, -ii + 2));
					schematic.push_back(vec3_t(ii, -1, -ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(ii, 0, -ii - 2));
						schematic.push_back(vec3_t(ii, 0, -ii + 3));
						schematic.push_back(vec3_t(ii, -1, -ii + 3));
						schematic.push_back(vec3_t(ii, -1, -ii - 2));
					}
				}
				if (width == 3) {
					schematic.push_back(vec3_t(ii, -1, -ii - 1));
					schematic.push_back(vec3_t(ii, -1, -ii));
					schematic.push_back(vec3_t(ii, -1, -ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(ii, 0, -ii - 2));
						schematic.push_back(vec3_t(ii, 0, -ii + 2));
						schematic.push_back(vec3_t(ii, -1, -ii + 2));
						schematic.push_back(vec3_t(ii, -1, -ii - 2));
					}
				}
			}
		}
		else if (xminus && zplus) {
			for (int ii = 0; ii < length; ii++) {
				if (width == 5) {
					schematic.push_back(vec3_t(-ii, -1, ii - 2));
					schematic.push_back(vec3_t(-ii, -1, ii - 1));
					schematic.push_back(vec3_t(-ii, -1, ii));
					schematic.push_back(vec3_t(-ii, -1, ii + 2));
					schematic.push_back(vec3_t(-ii, -1, ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(-ii, 0, ii - 3));
						schematic.push_back(vec3_t(-ii, 0, ii + 3));
						schematic.push_back(vec3_t(-ii, -1, ii + 3));
						schematic.push_back(vec3_t(-ii, -1, ii - 3));
					}
				}
				if (width == 4) {
					schematic.push_back(vec3_t(-ii, -1, ii - 1));
					schematic.push_back(vec3_t(-ii, -1, ii));
					schematic.push_back(vec3_t(-ii, -1, ii + 2));
					schematic.push_back(vec3_t(-ii, -1, ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(-ii, 0, ii - 2));
						schematic.push_back(vec3_t(-ii, 0, ii + 3));
						schematic.push_back(vec3_t(-ii, -1, ii + 3));
						schematic.push_back(vec3_t(-ii, -1, ii - 2));
					}
				}
				if (width == 3) {
					schematic.push_back(vec3_t(-ii, -1, ii - 1));
					schematic.push_back(vec3_t(-ii, -1, ii));
					schematic.push_back(vec3_t(-ii, -1, ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(-ii, 0, ii - 2));
						schematic.push_back(vec3_t(-ii, 0, ii + 2));
						schematic.push_back(vec3_t(-ii, -1, ii + 2));
						schematic.push_back(vec3_t(-ii, -1, ii - 2));
					}
				}
			}
		}
		else if (xminus && zminus) {
			for (int ii = 0; ii < length; ii++) {
				if (width == 5) {
					schematic.push_back(vec3_t(-ii, -1, -ii - 2));
					schematic.push_back(vec3_t(-ii, -1, -ii - 1));
					schematic.push_back(vec3_t(-ii, -1, -ii));
					schematic.push_back(vec3_t(-ii, -1, -ii + 2));
					schematic.push_back(vec3_t(-ii, -1, -ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(-ii, 0, -ii - 3));
						schematic.push_back(vec3_t(-ii, 0, -ii + 3));
						schematic.push_back(vec3_t(-ii, -1, -ii + 3));
						schematic.push_back(vec3_t(-ii, -1, -ii - 3));
					}
				}
				if (width == 4) {
					schematic.push_back(vec3_t(-ii, -1, -ii - 1));
					schematic.push_back(vec3_t(-ii, -1, -ii));
					schematic.push_back(vec3_t(-ii, -1, -ii + 2));
					schematic.push_back(vec3_t(-ii, -1, -ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(-ii, 0, -ii - 2));
						schematic.push_back(vec3_t(-ii, 0, -ii + 3));
						schematic.push_back(vec3_t(-ii, -1, -ii + 3));
						schematic.push_back(vec3_t(-ii, -1, -ii - 2));
					}
				}
				if (width == 3) {
					schematic.push_back(vec3_t(-ii, -1, -ii - 1));
					schematic.push_back(vec3_t(-ii, -1, -ii));
					schematic.push_back(vec3_t(-ii, -1, -ii + 1));
					if (railings) {
						schematic.push_back(vec3_t(-ii, 0, -ii - 2));
						schematic.push_back(vec3_t(-ii, 0, -ii + 2));
						schematic.push_back(vec3_t(-ii, -1, -ii + 2));
						schematic.push_back(vec3_t(-ii, -1, -ii - 2));
					}
				}
			}
		}
		else if (zplus) {
			for (int i = 0; i < length; i++) {
				if (width == 3) {
					schematic.push_back(vec3_t(-1, -1, i));
					schematic.push_back(vec3_t(0, -1, i));
					schematic.push_back(vec3_t(1, -1, i));
					if (railings) {
						schematic.push_back(vec3_t(-2, 0, i));
						schematic.push_back(vec3_t(2, 0, i));
					}
				}
				if (width == 4) {
					schematic.push_back(vec3_t(-1, -1, i));
					schematic.push_back(vec3_t(0, -1, i));
					schematic.push_back(vec3_t(1, -1, i));
					schematic.push_back(vec3_t(2, -1, i));
					if (railings) {
						schematic.push_back(vec3_t(-2, 0, i));
						schematic.push_back(vec3_t(3, 0, i));
					}
				}
				if (width == 5) {
					schematic.push_back(vec3_t(-1, -1, i));
					schematic.push_back(vec3_t(0, -1, i));
					schematic.push_back(vec3_t(1, -1, i));
					schematic.push_back(vec3_t(2, -1, i));
					schematic.push_back(vec3_t(-2, -1, i));
					if (railings) {
						schematic.push_back(vec3_t(-3, 0, i));
						schematic.push_back(vec3_t(3, 0, i));
					}
				}
			}
		}
		else if (zminus) {
			for (int i = 0; i < length; i++) {
				if (width == 3) {
					schematic.push_back(vec3_t(-1, -1, -i));
					schematic.push_back(vec3_t(0, -1, -i));
					schematic.push_back(vec3_t(1, -1, -i));
					if (railings) {
						schematic.push_back(vec3_t(-2, 0, -i));
						schematic.push_back(vec3_t(2, 0, -i));
					}
				}
				if (width == 4) {
					schematic.push_back(vec3_t(-1, -1, -i));
					schematic.push_back(vec3_t(0, -1, -i));
					schematic.push_back(vec3_t(1, -1, -i));
					schematic.push_back(vec3_t(2, -1, -i));
					if (railings) {
						schematic.push_back(vec3_t(-2, 0, -i));
						schematic.push_back(vec3_t(3, 0, -i));
					}
				}
				if (width == 5) {
					schematic.push_back(vec3_t(-1, -1, -i));
					schematic.push_back(vec3_t(0, -1, -i));
					schematic.push_back(vec3_t(1, -1, -i));
					schematic.push_back(vec3_t(2, -1, -i));
					schematic.push_back(vec3_t(-2, -1, -i));
					if (railings) {
						schematic.push_back(vec3_t(-3, 0, -i));
						schematic.push_back(vec3_t(3, 0, -i));
					}
				}

			}
		}
		else if (xminus) {
			for (int i = 0; i < length; i++) {
				if (width == 3) {
					schematic.push_back(vec3_t(-i, -1, -1));
					schematic.push_back(vec3_t(-i, -1, 0));
					schematic.push_back(vec3_t(-i, -1, 1));
					if (railings) {
						schematic.push_back(vec3_t(-i, 0, -2));
						schematic.push_back(vec3_t(-i, 0, 2));
					}
				}
				if (width == 4) {
					schematic.push_back(vec3_t(-i, -1, -1));
					schematic.push_back(vec3_t(-i, -1, 0));
					schematic.push_back(vec3_t(-i, -1, 1));
					schematic.push_back(vec3_t(-i, -1, 2));
					if (railings) {
						schematic.push_back(vec3_t(-i, 0, -2));
						schematic.push_back(vec3_t(-i, 0, 3));
					}
				}
				if (width == 5) {
					schematic.push_back(vec3_t(-i, -1, -1));
					schematic.push_back(vec3_t(-i, -1, 0));
					schematic.push_back(vec3_t(-i, -1, 1));
					schematic.push_back(vec3_t(-i, -1, 2));
					schematic.push_back(vec3_t(-i, -1, -2));
					if (railings) {
						schematic.push_back(vec3_t(-i, 0, -3));
						schematic.push_back(vec3_t(-i, 0, 3));
					}
				}

			}
		}
		else if (xplus) {
			for (int i = 0; i < length; i++) {
				if (width == 3) {
					schematic.push_back(vec3_t(i, -1, -1));
					schematic.push_back(vec3_t(i, -1, 0));
					schematic.push_back(vec3_t(i, -1, 1));
					if (railings) {
						schematic.push_back(vec3_t(i, 0, -2));
						schematic.push_back(vec3_t(i, 0, 2));
					}
				}
				if (width == 4) {
					schematic.push_back(vec3_t(i, -1, -1));
					schematic.push_back(vec3_t(i, -1, 0));
					schematic.push_back(vec3_t(i, -1, 1));
					schematic.push_back(vec3_t(i, -1, 2));
					if (railings) {
						schematic.push_back(vec3_t(i, 0, -2));
						schematic.push_back(vec3_t(i, 0, 3));
					}
				}
				if (width == 5) {
					schematic.push_back(vec3_t(i, -1, -1));
					schematic.push_back(vec3_t(i, -1, 0));
					schematic.push_back(vec3_t(i, -1, 1));
					schematic.push_back(vec3_t(i, -1, 2));
					schematic.push_back(vec3_t(i, -1, -2));
					if (railings) {
						schematic.push_back(vec3_t(i, 0, -3));
						schematic.push_back(vec3_t(i, 0, 3));
					}
				}
			}
		}

		C_LocalPlayer* Player = g_Data.getLocalPlayer();
		if (Player != nullptr) {
			vec3_t myPos = Player->getPos()->sub(0, 1, 0);
			startBuildPos = vec3_t(floorf(myPos.x), floorf(myPos.y), floorf(myPos.z));
			currentSchematic.clear();
			for (vec3_t p : schematic) {
				currentSchematic.push_back(startBuildPos.add(p));
			}
			setAutoBuildSlot = true;
			ogAutoBuildSlot = Player->getSupplies()->selectedHotbarSlot;
		}
		else {
			setEnabled(false);
		}
	}



	void onDisable() {
		C_LocalPlayer* Player = g_Data.getLocalPlayer();
		if (Player != nullptr) {
			if (setAutoHighwaySlot == true) {
				Player->getSupplies()->selectedHotbarSlot = ogAutoHighwaySlot;
			}
		}
	}

	void onLevelRender() {
		C_LocalPlayer* Player = g_Data.getLocalPlayer();
		if (Player != nullptr) {
			vec3_t myPos = *Player->getPos();
			C_PlayerInventoryProxy* supplies = Player->getSupplies();
			C_Inventory* inv = supplies->inventory;
			C_ItemStack* handItem = inv->getItemStack(supplies->selectedHotbarSlot);

			static std::vector<bool> blockPlaced(currentSchematic.size(), false); //new

			for (int i = 0; i < currentSchematic.size(); i++) {
				if (blockPlaced[i]) {
					// If the block has been placed, move to the next one
					continue;
				}
				vec3_t buildPos = currentSchematic.at(i);
				if (Player->getPos()->dist(vec3_t(buildPos.x + 0.5f, buildPos.y + 0.5f, buildPos.z + 0.5f)) <= 4.5f) {
					C_Block* block = Player->region->getBlock(buildPos);
					C_BlockLegacy* blockLegacy = block->toLegacy();

					if (blockLegacy->material->isReplaceable) {
						vec3_t blok(buildPos);
						vec3_t blockPosition(buildPos);
						static std::vector<vec3_t*> checklist;
						if (checklist.empty()) {
							//add the other list somehow
						}
						bool foundCandidate = false;
						int i = 0;

						for (auto current : checklist) {
							vec3_t calc = blok.sub(*current);
							bool Y = ((Player->region->getBlock(calc)->blockLegacy))->material->isReplaceable;
							if (!((Player->region->getBlock(calc)->blockLegacy))->material->isReplaceable) {
								// Found a solid block to click
								foundCandidate = true;
								blok = calc;
								break;
							}
							i++;
						}

						if (foundCandidate) {
							vec3_ti beforethingy = vec3_ti(blok.x, blok.y, blok.z);
							vec3_ti* thingy = &beforethingy;
							g_Data.getCGameMode()->buildBlock(thingy, i, 0); //what fucking number
							break;
						}
					}
					else {
						currentSchematic.erase(currentSchematic.begin() + i--);
					}
				}

				if (currentSchematic.empty()) {
					g_Data.getGuiData()->displayClientMessageF("%sFinished building!", GREEN);
					setEnabled(false);
				}
			}
		}
	}

	void AutoHighway::onPostRender(C_MinecraftUIRenderContext* renderCtx) { // visuals
		C_LocalPlayer* Player = g_Data.getLocalPlayer();
		if (Player != nullptr && g_Data.isInGame() && GameData::canUseMoveKeys()) {
			vec3_t myPos = Player->getPos();
			C_PlayerInventoryProxy supplies = Player->getSupplies(); //there should not be errors here, but there are... wtf did I fuck up this time smh
			C_ItemStack* handItem = supplies->inventory->getItemStack(supplies->selectedHotbarSlot);

			currentSchematic.push_back(startBuildPos);

			for (vec3_ti p : currentSchematic) {
				vec3_t bp(p.x, p.y, p.z);
				float dist = myPos.dist(bp.add(0.5f, 0.5f, 0.5f));
				if (dist <= visualrange2) {
					DrawUtils::setColor(0.1, 0.6, 0.7, 0.8f);

					if (visualize) {
						DrawUtils::drawBox(bp, bp.add(1.f, 1.f, 1.f), 0.3f, false);
					}
				}
			}
		}
	}
};

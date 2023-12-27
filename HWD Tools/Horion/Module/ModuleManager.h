#pragma once

#include <typeinfo>
#include <vector>
#include <optional>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "../../Memory/GameData.h"

#ifdef _BETA
#include "Modules/Verification.h"
#endif

#include "Modules/PacketMine.h"
#include "Modules/DeathCoords.h"
#include "Modules/PvPResources.h"

#include "Modules/AutoEZ.h"		  // funny    
#include "Modules/Arraylist.h"

#include "Modules/AutoClicker.h"
#include "Modules/AutoGapple.h"
#include "Modules/ClickGuiMod.h"
#include "Modules/Placer.h"
#include "Modules/Excavator.h"
#include "Modules/HudModule.h"
#include "Modules/AutoLog.h"
#include "Modules/ExtendedBlockReach.h"
#include "Modules/InventoryCleaner.h"
#include "Modules/EChestFarmer.h"
#include "Modules/Timer.h"
#include "Modules/ChatSuffix.h"
#include "Modules/FallSave.h"
#include "Modules/Module.h"
#include "Modules/NameTags.h"
#include "Modules/Scaffold.h"
#include "Modules/Spammer.h"
#include "Modules/Waypoints.h"
#include "Modules/Watermark.h"
#include "Modules/Theme.h"
#include "Modules/AutoHighway.h"
#include "Modules/HighwayTools.h"
#include "Modules/YawLock.h"

#ifdef _DEBUG
#include "Modules/PacketLogger.h"
#endif


class ModuleManager {
private:
	GameData* gameData;
	std::vector<std::shared_ptr<IModule>> moduleList;
	bool initialized = false;
	std::shared_mutex moduleListMutex;

public:
	~ModuleManager();
	ModuleManager(GameData* gameData);
	void initModules();
	void disable();
	void onLoadConfig(void* conf);
	void onSaveConfig(void* conf);
	void onTick(C_GameMode* gameMode);
	void onAttack(C_Entity* attackedEnt);
	void onWorldTick(C_GameMode* gameMode);
	void onKeyUpdate(int key, bool isDown);
	void onPreRender(C_MinecraftUIRenderContext* renderCtx);
	void onPostRender(C_MinecraftUIRenderContext* renderCtx);
	void onLevelRender();
	void onMove(C_MoveInputHandler* handler);
	void onPlayerTick(C_Player* plr);
	void onSendPacket(C_Packet*);

	std::shared_lock<std::shared_mutex> lockModuleList() { return std::shared_lock(moduleListMutex); }
	std::unique_lock<std::shared_mutex> lockModuleListExclusive() { return std::unique_lock(moduleListMutex); }
	
	std::shared_mutex* getModuleListLock() { return &moduleListMutex; }

	bool isInitialized() { return initialized; };
	std::vector<std::shared_ptr<IModule>>* getModuleList();

	int getModuleCount();
	int getEnabledModuleCount();

	/*
	 *	Use as follows: 
	 *		IModule* mod = moduleMgr.getModule<NoKnockBack>(); 
	 *	Check for nullptr directly after that call, as Hooks::init is called before ModuleManager::initModules !	
	 */
	template <typename TRet>
	TRet* getModule() {
		if (!isInitialized())
			return nullptr;
		auto lock = lockModuleList();
		for (auto pMod : moduleList) {
			if (auto pRet = dynamic_cast<typename std::remove_pointer<TRet>::type*>(pMod.get())){
				
				return pRet;
			}
		}
		return nullptr;
	};

	// Dont Use this functions unless you absolutely need to. The function above this one works in 99% of cases
	std::optional<std::shared_ptr<IModule>> getModuleByName(const std::string name) {
		if (!isInitialized())
			return nullptr;
		std::string nameCopy = name;
		std::transform(nameCopy.begin(), nameCopy.end(), nameCopy.begin(), ::tolower);
		
		auto lock = lockModuleList();
		for (std::vector<std::shared_ptr<IModule>>::iterator it = moduleList.begin(); it != moduleList.end(); ++it) {
			std::shared_ptr<IModule> mod = *it;
			std::string modNameCopy = mod->getRawModuleName();
			std::transform(modNameCopy.begin(), modNameCopy.end(), modNameCopy.begin(), ::tolower);
			if (modNameCopy == nameCopy)
				return std::optional<std::shared_ptr<IModule>>(mod);
		}
		return std::optional<std::shared_ptr<IModule>>();
	}
};

extern ModuleManager* moduleMgr;

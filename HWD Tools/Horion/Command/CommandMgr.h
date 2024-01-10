#pragma once

#include "../../Memory/GameData.h"
#include "Commands/BindCommand.h"
#include "Commands/EjectCommand.h"
#include "Commands/HelpCommand.h"
#include "Commands/HideCommand.h"
#include "Commands/ICommand.h"
#include "Commands/ModulesCommand.h"
#include "Commands/ServerCommand.h"
#include "Commands/SpammerCommand.h"
#include "Commands/ToggleCommand.h"
#include "Commands/UnbindCommand.h"
#include "Commands/ConfigCommand.h"
#include "Commands/SetPrefixCommand.h"
#include "Commands/WaypointCommand.h"
#include "Commands/ScriptCommand.h"

#include <algorithm>
#include <string>
#include <vector>

class CommandMgr {
private:
	GameData* gameData;
	std::vector<IMCCommand*> commandList;

public:
	CommandMgr(GameData* gm);
	~CommandMgr();

	char prefix = '.';

	void initCommands();
	void disable();
	std::vector<IMCCommand*>* getCommandList();

	void execute(char* message);
};

extern CommandMgr* cmdMgr;

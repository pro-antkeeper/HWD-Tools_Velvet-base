#pragma once
#include <ctime>

#include "../../../Utils/Target.h"
#include "../ModuleManager.h"
#include "Module.h"

class AutoEZ : public IModule {
private:
	bool sayUserDied = true;
	bool sayEnemDied = true;

	std::string deathMsg[19] = { //recount lol
		"Dont be a highway terrorist, you are better than this :(",
		"This guys IQ is in the single digits XD",
		"Tis but a minor setback",
		"JAM, this kid made me lose 0,000000000000000000000000000000000000000000000001% of my stuff >:(",
		"No life?",
		"You have the object permanence of a frog.",
		"yukky",
		"This kid is 100% trans (ew)",
		"Good thing Smokegod lets me use his stash lel",
		"Your brain is paper thin",
		"User fell off of the highway XD",
		"Now I need to regear :P",
		"I WAS AFK LMAO, SO WEAK",
		"Wait, how did I die?? This lag is crazy bruh",
		"A brick has a higher intelligence level than you",
		"You live in detroit",
		"newfag, you need help. The HWD can be that help",
		"My spawn point is set at the HWDs 1000+ dub stash :sunglasses:",
		"OH NO! My stuff I just duped :("
	};

	std::string killMsg[17] = { //recount lol
		"Should of joined the Highway Department smh",
		":skull: EZZd by the Highway Department client :skull:",
		"You just died to a client that makes highways… wow",
		"This client is as strong as the highways :muscle:",
		"This kid is 100% trans (ew)",
		"Join the HWD faqqot",
		"oops sowwyy >-< so silly of me",
		"User fell off of the highway XD",
		"”hole camper” Yeah, Im good with holes LOL ",
		"You have two options: 1) Join the HWD 2) cope harder",
		"I dont kill employees of the Highway Department btw",
		"User was turned into highway materials",
		"User was turned into highway materials",
		"The Highway Department can help you with your skill issue",
		"Become an employee of the HWD or keep dying",
		"yummy",
		"newfag, you need help. The HWD can be that help"
	};

public:
	int kc = 0;
	int dc = 0;
	bool foundPlayer = false;
	bool isDead = false;
	bool antiSpamCheck = true;

	AutoEZ();
	~AutoEZ();

	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
};

// chat suffix!

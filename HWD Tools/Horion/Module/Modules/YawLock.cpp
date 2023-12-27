#include <cmath>
#include "YawLock.h"

bool lockpitchvalue;
bool lockyawvalue;
bool lockaxis;
int pitchvalue = 0;
int yawvalue = 0;
float initialyaw = 0.0f;

yawLock::yawLock() : IModule(0, Category::PLAYER, "Modify your yaw / pitch values") {
	registerBoolSetting("Lock Yaw", &lockaxis, false);
	registerBoolSetting("Lock Pitch", &lockpitchvalue, false);
	registerEnumSetting("Mode", &Mode, 0);
	Mode = SettingEnum(this)
			   .addEntry(EnumEntry("Axis", 0))
			   .addEntry(EnumEntry("Value", 1))
			   .addEntry(EnumEntry("Highway", 2));
	registerEnumSetting("Highways", &Direction, 0);
	Direction = SettingEnum(this)
					.addEntry(EnumEntry("North", 0))
					.addEntry(EnumEntry("NE", 1))
					.addEntry(EnumEntry("East", 2))
					.addEntry(EnumEntry("SE", 3))
					.addEntry(EnumEntry("South", 4))
					.addEntry(EnumEntry("SW", 5))
					.addEntry(EnumEntry("West", 6))
					.addEntry(EnumEntry("NW", 7));
	registerIntSetting("Yaw", &yawvalue, 0, -180, 180);
	registerIntSetting("Pitch", &pitchvalue, 0, -180, 180);
}

yawLock::~yawLock() {
}

const char* yawLock::getModuleName() {
	return "YawLock";
}

void yawLock::onPlayerTick(C_Player* plr) {
	if (lockaxis && Mode.selected == 0) { // lock yaw by the closest axis of the current yaw but not hardset
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

			player->yaw = closestDirection;  // ex, look at 172, locks you at 180
		}
	}

	if (lockaxis && Mode.selected == 1) { // lock yaw by value
		plr->yaw = yawvalue;
	}

	if (lockaxis && Mode.selected == 2) { // lock yaw by hardset highway
		switch (Direction.selected) {
		default:
			plr->yaw = 180;
			break;
		case 1:
			plr->yaw = 225;
			break;
		case 2:
			plr->yaw = 270;
			break;
		case 3:
			plr->yaw = 315;
			break;
		case 4:
			plr->yaw = 0;
			break;
		case 5:
			plr->yaw = 45;
			break;
		case 6:
			plr->yaw = 90;
			break;
		case 7:
			plr->yaw = 135;
			break;
		}
	} 

	if (lockpitchvalue) { // lock pitch
		plr->pitch = (pitchvalue); // lock pitch, do -1 when elytra flying to not lose height
	}
}

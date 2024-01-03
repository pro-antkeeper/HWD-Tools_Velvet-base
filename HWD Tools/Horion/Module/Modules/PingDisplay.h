/*#pragma once
#include "Module.h"

class PingDisplay : public IModule { //not yet converted to velvet base
public:
    bool averagePing = false;
    bool lowercaseMS = false;
    bool space = true;

    PingDisplay() : IModule(0x0, Category::PLAYER, "Displays how laggy you are") {
        registerBoolSetting("display_average", &averagePing, averagePing);
        instance = this;
        //hasTickEvent = true; //idk what this does
    }

    int ping = 0;
    bool hasPing = false;
    virtual std::string getText() override {
        std::string text = hasPing ? std::to_string(ping) : "N/A";
            text += { "" };
        return text;
    }

    void onTick(C_GameMode* gm) override {
        PingStruct* pingStruct = getServerPingStruct();
        if (!pingStruct) {
            hasPing = false;
            return;
        }
        if (averagePing)
            ping = pingStruct->getAveragePing();
        else
            ping = pingStruct->getLastPing();
        hasPing = true;
    }

    inline static PingDisplay* instance = nullptr;
}; */
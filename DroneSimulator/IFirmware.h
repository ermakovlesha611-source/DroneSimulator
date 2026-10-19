#pragma once

#include "SensorData.h"
#include "MotorCommands.h"
#include "RcCommands.h"   // його теж скоро зробимо

class IFirmware
{
public:
    virtual ~IFirmware() = default;

    virtual void init() = 0;

    virtual void update(const SensorData& sensors,
                        const RcCommands& rc,
                        MotorCommands& motors,
                        float dt) = 0;
};
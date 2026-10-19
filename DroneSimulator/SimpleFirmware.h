#pragma once

#include "IFirmware.h"

class SimpleFirmware : public IFirmware
{
public:
    void init() override;

    void update(const SensorData& sensors,
        const RcCommands& rc,
        MotorCommands& motors,
        float dt) override;
};

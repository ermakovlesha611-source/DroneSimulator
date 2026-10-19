#pragma once

#include "DroneState.h"
#include "MotorCommands.h"

class DroneModel
{
public:
    DroneModel();

    void reset();
    void step(const MotorCommands& motors, float dt);

    const DroneState& getState() const;

private:
    DroneState state;
};

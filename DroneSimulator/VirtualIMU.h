#pragma once

#include "DroneState.h"
#include "SensorData.h"

class VirtualIMU
{
public:
    SensorData generate(const DroneState& state) const;
};

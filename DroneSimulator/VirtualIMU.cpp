#include "VirtualIMU.h"
#include "SimulationConfig.h"
#include <cmath>

SensorData VirtualIMU::generate(const DroneState& state) const
{
    SensorData sensors;

    // Гіроскоп — просто кутові швидкості
    sensors.gyro_x = state.wx;
    sensors.gyro_y = state.wy;
    sensors.gyro_z = state.wz;

    // Акселерометр (спрощена модель гравітації)
    const float g = SimulationConfig::gravity;

    float cos_roll = std::cos(state.roll);
    float sin_roll = std::sin(state.roll);
    float cos_pitch = std::cos(state.pitch);
    float sin_pitch = std::sin(state.pitch);

    sensors.acc_x = -g * sin_pitch;
    sensors.acc_y = g * sin_roll * cos_pitch;
    sensors.acc_z = g * cos_roll * cos_pitch;

    return sensors;
}
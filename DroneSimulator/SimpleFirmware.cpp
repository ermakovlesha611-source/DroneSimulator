#include "SimpleFirmware.h"

void SimpleFirmware::init()
{
}

void SimpleFirmware::update(const SensorData& sensors,
    const RcCommands& rc,
    MotorCommands& motors,
    float dt)
{
    // База близька до зависання (~0.31 при max_thrust=8, mass=1)
    float base = 0.31f + rc.throttle * 0.35f;

    if (!rc.arm)
    {
        motors.motor1 = 0.0f;
        motors.motor2 = 0.0f;
        motors.motor3 = 0.0f;
        motors.motor4 = 0.0f;
        return;
    }

    // Простий mixer
    motors.motor1 = base - rc.roll + rc.pitch - rc.yaw;
    motors.motor2 = base + rc.roll + rc.pitch + rc.yaw;
    motors.motor3 = base + rc.roll - rc.pitch - rc.yaw;
    motors.motor4 = base - rc.roll - rc.pitch + rc.yaw;

    // Обмеження 0..1
    if (motors.motor1 < 0.0f) motors.motor1 = 0.0f;
    if (motors.motor1 > 1.0f) motors.motor1 = 1.0f;
    if (motors.motor2 < 0.0f) motors.motor2 = 0.0f;
    if (motors.motor2 > 1.0f) motors.motor2 = 1.0f;
    if (motors.motor3 < 0.0f) motors.motor3 = 0.0f;
    if (motors.motor3 > 1.0f) motors.motor3 = 1.0f;
    if (motors.motor4 < 0.0f) motors.motor4 = 0.0f;
    if (motors.motor4 > 1.0f) motors.motor4 = 1.0f;
}
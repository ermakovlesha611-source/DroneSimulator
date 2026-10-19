#include "DroneModel.h"
#include "SimulationConfig.h"
#include <cmath>

DroneModel::DroneModel()
{
    reset();
}

void DroneModel::reset()
{
    state = DroneState();
    state.z = 0.15f;
}

void DroneModel::step(const MotorCommands& motors, float dt)
{
    using namespace SimulationConfig;

    // Тяга моторів
    float t1 = motors.motor1 * max_thrust;
    float t2 = motors.motor2 * max_thrust;
    float t3 = motors.motor3 * max_thrust;
    float t4 = motors.motor4 * max_thrust;
    float total_thrust = t1 + t2 + t3 + t4;

    // Моменти (X-конфігурація)
    float torque_roll = arm_length * 0.55f * ((t1 + t4) - (t2 + t3));
    float torque_pitch = arm_length * 0.55f * ((t1 + t2) - (t3 + t4));
    float torque_yaw = 0.07f * ((t1 + t3) - (t2 + t4));

    // Кутове прискорення
    state.wx += (torque_roll / inertia_x) * dt;
    state.wy += (torque_pitch / inertia_y) * dt;
    state.wz += (torque_yaw / inertia_z) * dt;

    // Демпфування
    state.wx *= 0.90f;
    state.wy *= 0.90f;
    state.wz *= 0.90f;

    // Обмеження швидкості обертання
    float max_rate = 3.0f;
    if (state.wx > max_rate) state.wx = max_rate;
    if (state.wx < -max_rate) state.wx = -max_rate;
    if (state.wy > max_rate) state.wy = max_rate;
    if (state.wy < -max_rate) state.wy = -max_rate;
    if (state.wz > max_rate) state.wz = max_rate;
    if (state.wz < -max_rate) state.wz = -max_rate;

    // Кути
    state.roll += state.wx * dt;
    state.pitch += state.wy * dt;
    state.yaw += state.wz * dt;

    // Обмеження кутів
    float max_angle = 0.6f; // ~35 deg
    if (state.roll > max_angle) state.roll = max_angle;
    if (state.roll < -max_angle) state.roll = -max_angle;
    if (state.pitch > max_angle) state.pitch = max_angle;
    if (state.pitch < -max_angle) state.pitch = -max_angle;

    // ===== Лінійний рух =====
    // Спрощена модель:
    // нахил pitch → рух по X
    // нахил roll  → рух по Y
    float thrust_acc = total_thrust / mass;

    float ax = sinf(state.pitch) * thrust_acc;
    float ay = -sinf(state.roll) * thrust_acc;
    float az = cosf(state.roll) * cosf(state.pitch) * thrust_acc - gravity;

    // Швидкості
    state.vx += ax * dt;
    state.vy += ay * dt;
    state.vz += az * dt;

    // Легке демпфування
    state.vx *= 0.995f;
    state.vy *= 0.995f;
    state.vz *= 0.995f;

    // Позиція
    state.x += state.vx * dt;
    state.y += state.vy * dt;
    state.z += state.vz * dt;

    // Земля
    if (state.z < 0.0f)
    {
        state.z = 0.0f;
        state.vz = 0.0f;
        state.vx *= 0.5f;
        state.vy *= 0.5f;
        state.wx *= 0.5f;
        state.wy *= 0.5f;
    }
}

const DroneState& DroneModel::getState() const
{
    return state;
}
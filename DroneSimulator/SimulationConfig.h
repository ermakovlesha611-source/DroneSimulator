#pragma once

namespace SimulationConfig
{
    // Крок симуляції (секунди)
    constexpr float dt = 0.002f;          // 500 Гц

    // Маса дрона (кг)
    constexpr float mass = 1.0f;

    // Прискорення вільного падіння
    constexpr float gravity = 9.81f;

    // Максимальна тяга одного мотора (Н)
    constexpr float max_thrust = 8.0f;

    // Відстань від центру до мотора (м)
    constexpr float arm_length = 0.22f;

    // Моменти інерції 
    constexpr float inertia_x = 0.02f;
    constexpr float inertia_y = 0.02f;
    constexpr float inertia_z = 0.04f;
}
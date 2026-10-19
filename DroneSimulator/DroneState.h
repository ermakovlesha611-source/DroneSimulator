#pragma once

struct DroneState {
    // Положення
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    // Лінійна швидкість
    float vx = 0.0f;
    float vy = 0.0f;
    float vz = 0.0f;

    // Орієнтація (радіани)
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;

    // Кутові швидкості
    float wx = 0.0f;
    float wy = 0.0f;
    float wz = 0.0f;
};
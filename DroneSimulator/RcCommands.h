#pragma once

struct RcCommands {
    // Стіки (зазвичай від -1.0 до 1.0)
    float roll = 0.0f;      // крен
    float pitch = 0.0f;     // тангаж
    float yaw = 0.0f;       // рискання
    float throttle = 0.0f;  // газ (0.0 – 1.0)

    // Перемикачі
    bool arm = false;       // армінг
    int mode = 0;           // режим польоту (0 - Angle, 1 - Acro тощо)
};

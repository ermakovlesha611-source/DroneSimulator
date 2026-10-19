#include <raylib.h>
#include <cmath>

#include "DroneModel.h"
#include "SimpleFirmware.h"
#include "VirtualIMU.h"
#include "SimulationConfig.h"
#include "RcCommands.h"
#include "IFirmware.h"

// =====================================================
// МЕНЮ
// =====================================================
IFirmware* ShowMainMenu()
{
    int selected = 0;
    const int itemsCount = 3;

    const char* items[3] = {
        "1. SimpleFirmware (ready)",
        "2. Firmware v2 (empty)",
        "3. Firmware v3 (empty)"
    };
    bool available[3] = { true, false, false };

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        {
            selected++;
            if (selected >= itemsCount) selected = 0;
        }
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        {
            selected--;
            if (selected < 0) selected = itemsCount - 1;
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            if (available[selected])
                return new SimpleFirmware();
        }

        BeginDrawing();
        ClearBackground(Color{ 25, 25, 35, 255 });

        DrawText("DRONE SIMULATOR", 420, 100, 40, RAYWHITE);
        DrawText("Select Firmware", 520, 160, 20, GRAY);

        for (int i = 0; i < itemsCount; i++)
        {
            int y = 260 + i * 60;
            Color color = !available[i] ? Color{ 180, 60, 60, 255 }
            : (i == selected ? SKYBLUE : LIGHTGRAY);

            if (i == selected) DrawText(">", 380, y, 30, color);
            DrawText(items[i], 420, y, 28, color);

            if (!available[i])
            {
                DrawCircle(395, y + 14, 7, RED);
                DrawText("EMPTY", 760, y + 8, 18, RED);
            }
        }

        if (!available[selected])
            DrawText("This firmware slot is empty. Drone will not start.", 300, 480, 20, RED);
        else
            DrawText("W/S or Arrows - select     ENTER - start", 360, 480, 20, DARKGRAY);

        EndDrawing();
    }
    return nullptr;
}

// =====================================================
// СИМУЛЯЦІЯ
// =====================================================
void RunSimulation(IFirmware* firmware)
{
    DroneModel drone;
    VirtualIMU imu;
    firmware->init();

    RcCommands rc{};
    MotorCommands motors{};

    float camX = 0.0f;
    float camY = 0.0f;
    float zoom = 1.2f;

    while (!WindowShouldClose())
    {
        // ----- Пульт -----
        rc.throttle = 0.0f;
        rc.roll = 0.0f;
        rc.pitch = 0.0f;
        rc.yaw = 0.0f;

        // Газ
        if (IsKeyDown(KEY_LEFT_SHIFT)) rc.throttle = 1.0f;
        if (IsKeyDown(KEY_LEFT_CONTROL)) rc.throttle = -0.4f;

        // Крен / тангаж
        if (IsKeyDown(KEY_RIGHT)) rc.roll = 0.35f;
        if (IsKeyDown(KEY_LEFT))  rc.roll = -0.35f;
        if (IsKeyDown(KEY_UP))    rc.pitch = 0.35f;
        if (IsKeyDown(KEY_DOWN))  rc.pitch = -0.35f;

        // Рискання
        if (IsKeyDown(KEY_E)) rc.yaw = 0.35f;
        if (IsKeyDown(KEY_Q)) rc.yaw = -0.35f;

        // Arm
        if (IsKeyPressed(KEY_SPACE))
            rc.arm = !rc.arm;

        // ----- Симуляція -----
        SensorData sensors = imu.generate(drone.getState());
        firmware->update(sensors, rc, motors, SimulationConfig::dt);
        drone.step(motors, SimulationConfig::dt);

        const DroneState& s = drone.getState();

        // ----- Камера (WASD) -----
        float camSpeed = 8.0f / zoom;
        if (IsKeyDown(KEY_W)) camY -= camSpeed;
        if (IsKeyDown(KEY_S)) camY += camSpeed;
        if (IsKeyDown(KEY_A)) camX -= camSpeed;
        if (IsKeyDown(KEY_D)) camX += camSpeed;

        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f)
        {
            zoom += wheel * 0.15f;
            if (zoom < 0.4f) zoom = 0.4f;
            if (zoom > 3.5f) zoom = 3.5f;
        }

        // ----- Малювання -----
        BeginDrawing();
        ClearBackground(Color{ 25, 25, 35, 255 });

        float cx = GetScreenWidth() / 2.0f + camX;
        float cy = GetScreenHeight() / 2.0f + camY;

        // Сітка (прозора)
        int step = (int)(70 * zoom);
        if (step < 20) step = 20;
        Color gridColor = { 45, 45, 55, 45 };

        for (int x = 0; x < GetScreenWidth(); x += step)
            DrawLine(x, 0, x, GetScreenHeight(), gridColor);
        for (int y = 0; y < GetScreenHeight(); y += step)
            DrawLine(0, y, GetScreenWidth(), y, gridColor);

        // Статичні осі (через центр екрану)
        float screenCx = GetScreenWidth() / 2.0f;
        float screenCy = GetScreenHeight() / 2.0f;

        DrawLineEx({ 0, screenCy }, { (float)GetScreenWidth(), screenCy }, 2.0f, Color{ 200, 80, 80, 90 });
        DrawText("X+", GetScreenWidth() - 40, (int)screenCy - 18, 18, Color{ 220, 100, 100, 160 });

        DrawLineEx({ screenCx, (float)GetScreenHeight() }, { screenCx, 0 }, 2.0f, Color{ 80, 200, 80, 90 });
        DrawText("Y+", (int)screenCx + 8, 12, 18, Color{ 100, 220, 100, 160 });

        // Позиція дрона
        float dx = cx + s.x * 45.0f * zoom;
        float dy = cy - s.y * 45.0f * zoom;

        float arm = 38.0f * zoom;
        float motorR = 11.0f * zoom;

        float a[4] = {
            s.yaw + 45 * DEG2RAD,
            s.yaw + 135 * DEG2RAD,
            s.yaw + 225 * DEG2RAD,
            s.yaw + 315 * DEG2RAD
        };

        // Промені
        for (int i = 0; i < 4; i++)
        {
            float ex = dx + cosf(a[i]) * arm;
            float ey = dy + sinf(a[i]) * arm;
            DrawLineEx({ dx, dy }, { ex, ey }, 5.0f * zoom, Color{ 180, 180, 190, 255 });
        }

        // Мотори
        float thrust[4] = { motors.motor1, motors.motor2, motors.motor3, motors.motor4 };
        for (int i = 0; i < 4; i++)
        {
            float mx = dx + cosf(a[i]) * arm;
            float my = dy + sinf(a[i]) * arm;

            DrawCircleLines(mx, my, motorR + 4 * zoom, Color{ 100, 100, 120, 200 });
            Color c = {
                (unsigned char)(60 + thrust[i] * 195),
                (unsigned char)(60 + thrust[i] * 80),
                40, 255
            };
            DrawCircle(mx, my, motorR, c);
            DrawCircleLines(mx, my, motorR, WHITE);
        }

        // Корпус + ніс
        DrawCircle(dx, dy, 14.0f * zoom, Color{ 50, 50, 60, 255 });
        DrawCircleLines(dx, dy, 14.0f * zoom, RAYWHITE);

        float nx = dx + cosf(s.yaw) * 22.0f * zoom;
        float ny = dy + sinf(s.yaw) * 22.0f * zoom;
        DrawLineEx({ dx, dy }, { nx, ny }, 4.0f * zoom, SKYBLUE);
        DrawCircle(nx, ny, 4.0f * zoom, SKYBLUE);

        // Інфо зліва
        DrawRectangle(15, 15, 270, 250, Color{ 0, 0, 0, 200 });
        DrawRectangleLines(15, 15, 270, 250, Color{ 80, 80, 100, 255 });

        DrawText("DRONE SIMULATOR", 30, 28, 20, RAYWHITE);
        DrawText(TextFormat("Alt:    %.2f m", s.z), 30, 65, 18, GREEN);
        DrawText(TextFormat("Roll:   %.1f deg", s.roll * RAD2DEG), 30, 95, 18, LIGHTGRAY);
        DrawText(TextFormat("Pitch:  %.1f deg", s.pitch * RAD2DEG), 30, 120, 18, LIGHTGRAY);
        DrawText(TextFormat("Yaw:    %.1f deg", s.yaw * RAD2DEG), 30, 145, 18, LIGHTGRAY);
        DrawText(TextFormat("Thrust: %.0f %%", motors.motor1 * 100.0f), 30, 175, 18, ORANGE);
        DrawText(TextFormat("Armed:  %s", rc.arm ? "YES" : "NO"), 30, 205, 18, rc.arm ? GREEN : RED);

        // Координати справа (тільки X Y)
        DrawRectangle(GetScreenWidth() - 220, 15, 200, 95, Color{ 0, 0, 0, 200 });
        DrawRectangleLines(GetScreenWidth() - 220, 15, 200, 95, Color{ 80, 80, 100, 255 });
        DrawText("POSITION", GetScreenWidth() - 200, 25, 18, RAYWHITE);
        DrawText(TextFormat("X: %.2f m", s.x), GetScreenWidth() - 200, 55, 18, Color{ 220, 100, 100, 255 });
        DrawText(TextFormat("Y: %.2f m", s.y), GetScreenWidth() - 200, 80, 18, Color{ 100, 220, 100, 255 });

        DrawText("Shift/Ctrl - throttle   Arrows - tilt   Q/E - yaw   Space - Arm", 20, GetScreenHeight() - 55, 18, GRAY);
        DrawText("WASD - camera   Wheel - zoom", 20, GetScreenHeight() - 30, 18, GRAY);

        EndDrawing();
    }

    delete firmware;
}

// =====================================================
int main()
{
    InitWindow(1280, 720, "Drone Simulator");
    SetTargetFPS(60);

    IFirmware* firmware = ShowMainMenu();
    if (firmware != nullptr)
        RunSimulation(firmware);

    CloseWindow();
    return 0;
}
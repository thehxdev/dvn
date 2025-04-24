#include <raylib.h>

#define CAST(t, v) (static_cast<t>((v)))

typedef Vector2 Point;
typedef Vector2 Velocity;

typedef struct ball {
    Point center;
    float radius;
    Velocity v;
    Color color;
} Ball;

enum {
    NIGHT,
    DAY,
};

constexpr int WIDTH = 800;
constexpr int HEIGHT = WIDTH;

constexpr int BALL_RADIUS = (WIDTH/40.f)-5.f;
constexpr int BALL_INITIAL_Y = HEIGHT/2.f;

// Cell count in each row
constexpr int CELL_COUNT = 20;
constexpr int CELL_SIZE  = WIDTH/CELL_COUNT;

constexpr Rectangle cell_to_rect(int i, int j) {
    return Rectangle{
        CAST(float, (j)*CELL_SIZE),
        CAST(float, (i)*CELL_SIZE),
        CELL_SIZE,
        CELL_SIZE,
    };
}

// Hex value to raylib Color type
// This function assumes that the hex value is a 32-bit integer WITHOUT the alpha value.
constexpr Color htc(int c) {
    return (Color{
        CAST(unsigned char, (c >> 16) & 0xFF),
        CAST(unsigned char, (c >>  8) & 0xFF),
        CAST(unsigned char, (c & 0xFF)),
        0xFF,
    });
}

constexpr Color DAY_BALL_COLOR   = htc(0xB4D4FF);
constexpr Color NIGHT_BALL_COLOR = htc(0x3282B8);
constexpr Color DAY_BACKGROUND_COLOR   = htc(0xECF9FF);
constexpr Color NIGHT_BACKGROUND_COLOR = htc(0x001F3F);

static bool cells[CELL_COUNT][CELL_COUNT] = { 0 };

constexpr static inline bool& cell_get_state(int i, int j) {
    return cells[(i)][(j)];
}

static inline void cell_draw(Rectangle &r, bool state) {
    const Color c = (state) ? DAY_BACKGROUND_COLOR : NIGHT_BACKGROUND_COLOR;
    DrawRectangle(r.x, r.y, CELL_SIZE, CELL_SIZE, c);
}

static inline void cells_draw_all(void) {
    for (int i = 0; i < CELL_COUNT; i++)
        for (int j = 0; j < CELL_COUNT; j++) {
            Rectangle r = cell_to_rect(i, j);
            cell_draw(r, cell_get_state(i, j));
        }
}

static inline void cells_initialize(void) {
    constexpr int night_side = CELL_COUNT/2;
    for (int i = 0; i < CELL_COUNT; i++)
        for (int j = night_side; j < CELL_COUNT; j++) {
            bool &cell = cell_get_state(i, j);
            cell = DAY;
        }
}

static inline void ball_draw(Ball &b) {
    DrawCircle(b.center.x, b.center.y, b.radius, b.color);
}

static inline void ball_update_position(Ball &b) {
    b.center.x += b.v.x;
    b.center.y += b.v.y;
}

static inline void ball_window_collision_handler(Ball &b) {
    if (b.center.x >= WIDTH - b.radius || b.center.x <= b.radius) {
        b.v.x *= -1.f;
        b.center.x += b.v.x;
    }
    if (b.center.y >= HEIGHT - b.radius || b.center.y <= b.radius) {
        b.v.y *= -1.f;
        b.center.y += b.v.y;
    }
}

static inline void ball_cells_collision_handler(Ball &b, int kind) {
    for (int i = 0; i < CELL_COUNT; i++)
        for (int j = 0; j < CELL_COUNT; j++) {
            bool &cell_state = cell_get_state(i, j);
            if (CAST(int, cell_state) == kind) 
                continue;
            const Rectangle cell = cell_to_rect(i, j);
            if (!CheckCollisionCircleRec(b.center, b.radius, cell))
                continue;
            cell_state = !cell_state;
            if (b.center.x + b.radius >= cell.x || b.center.x - b.radius <= cell.x + cell.width)
                b.v.x *= -1.f;
            else if (b.center.y + b.radius >= cell.y || b.center.y - b.radius <= cell.y + cell.height)
                b.v.y *= -1.f;
            goto done;
        }
done:
    return;
}

int main(void) {
    // smother edge rendering
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(WIDTH, HEIGHT, "DvN (Day vs Night)");
    SetTargetFPS(60);

    Ball day = Ball{
        .center = Point{ WIDTH/4.f*3.f, BALL_INITIAL_Y },
        .radius = BALL_RADIUS,
        .v = Vector2{ -12.f, 9.f },
        .color = DAY_BALL_COLOR,
    };

    Ball night = Ball{
        .center = Point{ WIDTH/4.f, BALL_INITIAL_Y },
        .radius = BALL_RADIUS,
        .v = Vector2{ 8.f, -8.f },
        .color = NIGHT_BALL_COLOR,
    };

    cells_initialize();

    while (!WindowShouldClose()) {
        // Update
        // NOTE: This type of updating position and handling collisions
        // is not really correct. For example on high velocity and some
        // edge cases this approach fails. But for now it's fine.
        ball_update_position(day);
        ball_window_collision_handler(day);
        ball_cells_collision_handler(day, DAY);

        ball_update_position(night);
        ball_window_collision_handler(night);
        ball_cells_collision_handler(night, NIGHT);

        // Draw
        BeginDrawing();
        {
            cells_draw_all();
            ball_draw(day);
            ball_draw(night);

            DrawFPS(20, 20);
            DrawText("Press ESC to exit", 20, 45, 20, DARKGREEN);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

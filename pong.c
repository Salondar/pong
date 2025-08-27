#include <raylib.h>
#include <stdbool.h>

#define ARENA_OFFSET 50
#define PADDLE_HEIGHT 200
#define PADDLE_WIDTH 25
#define PADDLE_OFFSETX 5

typedef struct {
    int width;
    int height;
}Screen;

typedef struct {
    Rectangle rec;
    float velocityY;
    Color color;
    int score;
}Paddle;

typedef struct {
    int centerX;
    int centerY;
    float velocityX;
    float velocityY;
    float radius;
    Color color;
}Ball;

typedef struct {
    Vector2 start;
    float width;
    float height;
    Color color;
} Arena;

typedef enum {
    GM_MENU,
    GM_GAMEPLAY
}GameMode;

GameMode current_mode = GM_MENU;
bool is_enemy_ai = true;
static int hot_button = 0;

Sound wallCollisionSound = {0};
Sound brickCollisionSound = {0};

void InitGame(Screen *screen, Arena *arena, Paddle *player, Paddle *enemy, Ball *ball) {

    arena->start.x = ARENA_OFFSET;
    arena->start.y = ARENA_OFFSET;
    arena->width = screen->width - 2 * ARENA_OFFSET;
    arena->height = screen->height - 2 * ARENA_OFFSET;

    player->color = BLACK;
    player->rec.height = PADDLE_HEIGHT;
    player->rec.width = PADDLE_WIDTH;
    player->rec.x = arena->width- PADDLE_OFFSETX;
    player->rec.y = (arena->height - PADDLE_HEIGHT) / 2;
    player->velocityY = 20;
    player->score = 0;

    enemy->color = BLACK;
    enemy->rec.height = PADDLE_HEIGHT;
    enemy->rec.width = PADDLE_WIDTH;
    enemy->rec.x = ARENA_OFFSET + PADDLE_WIDTH + PADDLE_OFFSETX;
    enemy->rec.y = (arena->height - PADDLE_HEIGHT) / 2;
    enemy->velocityY = 10;
    enemy->score = 0;

    ball->centerX = screen->width / 2;
    ball->centerY = screen->height / 2;
    ball->color = WHITE;
    ball->radius = 20.0f;
    ball->velocityX = 20;
    ball->velocityY = 0;
}

void UpdateScore(Paddle *player, Ball *ball, Screen *screen) {
    ball->centerX = screen->width / 2;
    ball->centerY = screen->height / 2;
    ball->velocityY = 0;
    player->score++; 
}

void Update(Screen *screen, Arena *arena, Paddle *player, Paddle *enemy, Ball *ball) {
     // Player 1
    if (IsKeyDown(KEY_UP) && (player->rec.y > ARENA_OFFSET)) {
        player->rec.y -= player->velocityY;
    }
    if ((IsKeyDown(KEY_DOWN) && ((player->rec.y + player->rec.height)< arena->height + ARENA_OFFSET))) {
        player->rec.y += player->velocityY;
    }
    
    if (!is_enemy_ai) {
        // Player 2 Moves
        if (IsKeyDown(KEY_W) && (enemy->rec.y > ARENA_OFFSET)) {
            enemy->rec.y -= enemy->velocityY;
        }
        if ((IsKeyDown(KEY_S) && ((enemy->rec.y + enemy->rec.height)< arena->height + ARENA_OFFSET))) {
            enemy->rec.y += enemy->velocityY;
        }
    }
    else {
        enemy->velocityY = (ball->centerY - enemy->rec.y);
        enemy->rec.y += enemy->velocityY * 0.45;
        if (enemy->rec.y < ARENA_OFFSET) {
            enemy->rec.y = ARENA_OFFSET;
        }
        else if (enemy->rec.y + enemy->rec.height > arena->height + ARENA_OFFSET) {
            enemy->rec.y = (arena->height + ARENA_OFFSET) - enemy->rec.height;
        }
    }
    if (((ball->centerX + ball->radius) > player->rec.x) 
        && ((ball->centerX - ball->radius) < (player->rec.x + player->rec.width))
        && ((ball->centerY + ball->radius) > player->rec.y) 
        && ((ball->centerY - ball->radius) < (player->rec.y + player->rec.height))) {
    
            ball->centerX = player->rec.x - ball->radius;
            ball->velocityX *= -1;
            ball->velocityY = (ball->centerY - (player->rec.y + (player->rec.height / 2.0))) * 0.2;
    
            PlaySound(brickCollisionSound);
    }


    // Collision detection ball with enemy paddle
     if (((ball->centerX + ball->radius) > enemy->rec.x) 
        && ((ball->centerX - ball->radius) < enemy->rec.x + enemy->rec.width)
        && ((ball->centerY + ball->radius) > enemy->rec.y) 
        && ((ball->centerY - ball->radius) < enemy->rec.y + enemy->rec.height)) {

        ball->centerX = enemy->rec.x + ball->radius + PADDLE_WIDTH;
        ball->velocityX *= -1;
        ball->velocityY = (ball->centerY - (enemy->rec.y + (enemy->rec.height / 2.0))) * 0.2;
        PlaySound(brickCollisionSound);
    }

    // Collision detection ball with bottom wall
    if (ball->centerY + ball->radius > arena->start.x + arena->height) {
        ball->velocityY *= -1;
        ball->centerY = arena->start.x + arena->height - ball->radius;
        PlaySound(wallCollisionSound);
    }

    // Collision detection ball with top wall
    if ((ball->centerY - ball->radius) < arena->start.y) {
        ball->centerY = arena->start.y + ball->radius;
        ball->velocityY *= -1;
        PlaySound(wallCollisionSound);
    }
    ball->centerX += ball->velocityX;
    ball->centerY += ball->velocityY;

    if (ball->centerX + ball->radius > arena->width + ARENA_OFFSET) {
        UpdateScore(enemy, ball, screen);
    }

    if (ball->centerX - ball->radius < ARENA_OFFSET) {
        UpdateScore(player, ball, screen);
    }
}

void DrawMenu(Screen screen) {
    ClearBackground((Color){13, 18, 36, 100});
    BeginDrawing();
    DrawText("PONG", (screen.width - MeasureText("PONG", 110)) / 2, screen.height / 2 - 200, 110, WHITE);
    if (hot_button == 0) {
         DrawRectangleRounded((Rectangle){(screen.width - 630)/ 2.0, (screen.height - 80)/2.0, 630, 80}, 0.3, 10, (Color){94, 160, 255, 125});
    } else {
         DrawRectangleRounded((Rectangle){(screen.width - 630)/ 2.0, (screen.height - 80)/2.0, 630, 80}, 0.3, 10, (Color){40, 40, 40, 255});
    }
    DrawText("Single Player", (screen.width - MeasureText("Single Player", 70)) / 2, (screen.height - 70) / 2, 70, WHITE);
    if (hot_button == 1) {
        DrawRectangleRounded((Rectangle){(screen.width - 630)/ 2.0, (screen.height + 160)/2.0, 630, 80}, 0.3, 10, (Color){94, 160, 255, 125});
    } else {
        DrawRectangleRounded((Rectangle){(screen.width - 630)/ 2.0, (screen.height + 160)/2.0, 630, 80}, 0.3, 10, (Color){40, 40, 40, 255});
    }
    DrawText("Two Player", (screen.width - MeasureText("Two Player", 70)) / 2, (screen.height + 170) / 2, 70, WHITE);
    DrawText("PRESS ENTER TO SELECT", 50, screen.height - 50, 30, WHITE);
    EndDrawing();
}

void DrawFrame(Screen screen, Arena arena, Paddle player, Paddle enemy, Ball ball) {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    DrawRectangleV(arena.start, (Vector2){arena.width, arena.height}, SKYBLUE);
    DrawRectangleRec(player.rec, player.color);
    DrawRectangleRec(enemy.rec, enemy.color);
    DrawText(TextFormat("%d", player.score), screen.width / 2 + (2 * ARENA_OFFSET), 2 * ARENA_OFFSET, 100, WHITE);
    DrawText(TextFormat("%d", enemy.score), screen.width / 2 - (2 * ARENA_OFFSET), 2 * ARENA_OFFSET, 100, WHITE);
    DrawCircle(ball.centerX, ball.centerY, ball.radius, ball.color);
    EndDrawing();
}

int main(void) {
    Paddle player, enemy;
    Ball ball;
    Arena arena;
    Screen screen;

    screen.width = 1700;
    screen.height = 900;
    
    InitWindow(screen.width, screen.height, "Pong");
    InitAudioDevice();
    SetTargetFPS(60);
    InitGame(&screen, &arena, &player, &enemy, &ball);

    wallCollisionSound = LoadSound("sound/wall.mp3");
    brickCollisionSound = LoadSound("sound/block.mp3");

    while (!WindowShouldClose()) {
        if (current_mode == GM_MENU) {
            if (IsKeyPressed(KEY_UP)) {
                hot_button = 0;
            }
            else if (IsKeyPressed(KEY_DOWN)) {
                hot_button = 1;
            }
            if (IsKeyPressed(KEY_ENTER)) {
                current_mode = GM_GAMEPLAY;
                if (hot_button == 0) {
                    is_enemy_ai = true;
                }
                else {
                    is_enemy_ai = false;
                }
            }
            DrawMenu(screen);
        } else {
            Update(&screen, &arena, &player, &enemy, &ball);
            DrawFrame(screen, arena, player, enemy, ball);
        }
      
    }
    UnloadSound(wallCollisionSound);
    UnloadSound(brickCollisionSound);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}
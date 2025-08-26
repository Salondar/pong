#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define ARENA_OFFSET 50
#define PADDLE_HEIGHT 175
#define PADDLE_WIDTH 25
#define PADDLE_OFFSETX 5

typedef struct {
    Rectangle rec;
    float velocityY;
    Color color;
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

Paddle player, enemy;
Ball ball;
Arena arena;
Sound wallCollisionSound = {0};
Sound brickCollisionSound = {0};

static int screenWidth, screenHeight, playerScore, enemyScore;

void InitGame(void) {

    arena.start.x = ARENA_OFFSET;
    arena.start.y = ARENA_OFFSET;
    arena.width = screenWidth - 2 * ARENA_OFFSET;
    arena.height = screenHeight - 2 * ARENA_OFFSET;

    player.color = BLACK;
    player.rec.height = PADDLE_HEIGHT;
    player.rec.width = PADDLE_WIDTH;
    player.rec.x = arena.width- PADDLE_OFFSETX;
    player.rec.y = (arena.height - PADDLE_HEIGHT) / 2;
    player.velocityY = 15;

    enemy.color = BLACK;
    enemy.rec.height = PADDLE_HEIGHT;
    enemy.rec.width = PADDLE_WIDTH;
    enemy.rec.x = ARENA_OFFSET + PADDLE_WIDTH + PADDLE_OFFSETX;
    enemy.rec.y = (arena.height - PADDLE_HEIGHT) / 2;
    enemy.velocityY = 10;

    ball.centerX = screenWidth / 2;
    ball.centerY = screenHeight / 2;
    ball.color = WHITE;
    ball.radius = 20.0f;
    ball.velocityX = 13;
    ball.velocityY = 0;

    playerScore = 0;
    enemyScore = 0;
}

void Update(void) {

    if (IsKeyDown(KEY_UP) && (player.rec.y > ARENA_OFFSET)) {
        player.rec.y -= player.velocityY;
    
    }
    if ((IsKeyDown(KEY_DOWN) && ((player.rec.y + player.rec.height)< arena.height + ARENA_OFFSET))) {
        player.rec.y += player.velocityY;
    }

#if 0
    if (IsKeyDown(KEY_W) && (enemy.rec.y > ARENA_OFFSET)) {
        enemy.rec.y -= enemy.velocityY;
    }
    if ((IsKeyDown(KEY_S) && ((enemy.rec.y + enemy.rec.height)< arena.height + ARENA_OFFSET))) {
        enemy.rec.y += enemy.velocityY;
    }
#else
    //if ((ball.centerY + ball.radius) > enemy.rec.y) {
    //    if (enemy.rec.y + enemy.rec.height < arena.height + ARENA_OFFSET)
    //        enemy.rec.y += 5.2;
    //}
    //else if ((ball.centerY) < enemy.rec.y) {
    //        enemy.rec.y -= 5.2;
    //}
    enemy.velocityY = (ball.centerY - enemy.rec.y);
    enemy.rec.y += enemy.velocityY * 0.35;
    if (enemy.rec.y < ARENA_OFFSET) {
        enemy.rec.y = ARENA_OFFSET;
    }
    else if (enemy.rec.y + enemy.rec.height > arena.height + ARENA_OFFSET) {
        enemy.rec.y = (arena.height + ARENA_OFFSET) - enemy.rec.height;
    }
#endif
    // Collision detection ball with player paddle
    if (((ball.centerX + ball.radius) > player.rec.x) && ((ball.centerX - ball.radius) < player.rec.x + player.rec.width)
        && ((ball.centerY + ball.radius) > player.rec.y) && ((ball.centerY - ball.radius) < player.rec.y + player.rec.height)) {
        ball.centerX = player.rec.x - ball.radius;
        ball.velocityX *= -1;
        ball.velocityY = (ball.centerY - (player.rec.y + (player.rec.height / 2.0))) * 0.2;
        PlaySound(brickCollisionSound);
    }

    // Collision detection ball with enemy paddle
     if (((ball.centerX + ball.radius) > enemy.rec.x) && ((ball.centerX - ball.radius) < enemy.rec.x + enemy.rec.width)
        && ((ball.centerY + ball.radius) > enemy.rec.y) && ((ball.centerY - ball.radius) < enemy.rec.y + enemy.rec.height)) {
        ball.centerX = enemy.rec.x + ball.radius + PADDLE_WIDTH;
        ball.velocityX *= -1;
        ball.velocityY = (ball.centerY - (enemy.rec.y + (enemy.rec.height / 2.0))) * 0.2;
        PlaySound(brickCollisionSound);
    }

    // Collision detection ball with bottom wall
    if (ball.centerY + ball.radius > arena.start.x + arena.height) {
        ball.velocityY *= -1;
        ball.centerY = arena.start.x + arena.height - ball.radius;
        PlaySound(wallCollisionSound);
    }

    // Collision detection ball with top wall
    if ((ball.centerY - ball.radius) < arena.start.y) {
        ball.centerY = arena.start.y + ball.radius;
        ball.velocityY *= -1;
        PlaySound(wallCollisionSound);
    }
    ball.centerX += ball.velocityX;
    ball.centerY += ball.velocityY;

    if (ball.centerX + ball.radius > arena.width + ARENA_OFFSET) {
        ball.centerX = screenWidth / 2;
        ball.centerY = screenHeight / 2;
        ball.velocityY = 0;
        enemyScore++;
    }

    if (ball.centerX - ball.radius < ARENA_OFFSET) {
        ball.centerX = screenWidth / 2;
        ball.centerY = screenHeight / 2;
        ball.velocityY = 0;
        playerScore++;
    }
}

void DrawFrame(void) {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    DrawRectangleV(arena.start, (Vector2){arena.width, arena.height}, SKYBLUE);
    DrawRectangleRec(player.rec, player.color);
    DrawRectangleRec(enemy.rec, enemy.color);
    DrawText(TextFormat("%d", playerScore), screenWidth / 2 + (2 * ARENA_OFFSET), 2 * ARENA_OFFSET, 100, WHITE);
    DrawText(TextFormat("%d", enemyScore), screenWidth / 2 - (2 * ARENA_OFFSET), 2 * ARENA_OFFSET, 100, WHITE);
    DrawCircle(ball.centerX, ball.centerY, ball.radius, ball.color);

    EndDrawing();
}

int main(void) {
    
    screenWidth = 1280;
    screenHeight = 720;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Pong");
    InitAudioDevice();
    SetTargetFPS(60);
    InitGame();

    wallCollisionSound = LoadSound("sound/wall.mp3");
    brickCollisionSound = LoadSound("sound/block.mp3");

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();

            arena.width = screenWidth - 2 * ARENA_OFFSET;
            arena.height = screenHeight - 2 * ARENA_OFFSET;

            player.rec.x = arena.width - PADDLE_OFFSETX;
            player.rec.y = (arena.height - PADDLE_HEIGHT) / 2;
            enemy.rec.x = ARENA_OFFSET + PADDLE_WIDTH + PADDLE_OFFSETX;
            enemy.rec.y = (arena.height - PADDLE_HEIGHT) / 2;
        }
        Update();
        DrawFrame();
    }
    UnloadSound(wallCollisionSound);
    UnloadSound(brickCollisionSound);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}
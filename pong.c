/*
 * Pong written using raylib C API. This is mainly to test if I enjoy raylib more than SDL2.
 * If you're testing this out, enjoy!
*/

#include "raylib.h"
#include <stdio.h>

/*
 * Paddle Constants
*/

#define PADDLE_VERT_SPEED 200.0f
#define PADDLE_X_PADDING 20
#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 75

/*
 * Ball Constants
*/
#define BALL_HOR_SPEED 200.0f
#define BALL_VERT_SPEED 200.0f
#define BALL_RADIUS 10

typedef struct Paddle {
    int yPosition;
    float velocity;
    Color color;
} Paddle;

typedef struct Ball {
    Vector2 position;
    float velocity;
    Color color;
} Ball;

typedef struct Slope {
    int x;
    int y;
} Slope;

typedef struct Score {
    int player1Score;
    int player2Score;
} Score;

void updatePaddlePosition(Paddle *player, float delta);
void updateBallPosition(Ball *ball, float delta);
void readRestart(Ball *ball);
void drawScore();
void drawRestartText();
void drawStartScreen();

bool touchTopBoundary(int yPos);
bool touchBottomBoundary(int yPos);
bool checkBallPaddleCollision(Ball *ball, Paddle *paddle);
bool checkIfScore(Ball *ball);
bool isScored;

char screen = 'M';

Paddle player1;
Paddle player2;
Slope ball_path;
Score score;

Slope generateSlope();

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong!");

    SetTargetFPS(60);

    player1 = (Paddle){ (SCREEN_HEIGHT/2) - (PADDLE_HEIGHT/2), 0, GOLD };
    player2 = (Paddle){ (SCREEN_HEIGHT/2) - (PADDLE_HEIGHT/2), 0, GOLD };

    Vector2 center = { SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };

    Ball ball = { center, 0, WHITE };

    score = (Score){ 0, 0 };

    ball_path = generateSlope();

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();


        if (isScored) {
            readRestart(&ball);
        } else {
            updatePaddlePosition(&player1, delta);
            updatePaddlePosition(&player2, delta);

            updateBallPosition(&ball, delta);

            isScored = checkIfScore(&ball);
        }

        switch (screen) {
            case 'M':
                if (IsKeyPressed(KEY_ENTER)) {
                    screen = 'G';
                }
                break;
            case 'G':
                if (isScored) {
                    readRestart(&ball);
                } else {
                    updatePaddlePosition(&player1, delta);
                    updatePaddlePosition(&player2, delta);

                    updateBallPosition(&ball, delta);

                    isScored = checkIfScore(&ball);
                }
                break;
        }

        BeginDrawing();
            ClearBackground(LIGHTGRAY);
            Vector2 paddleOrigin = (Vector2){ 0, 39 }; 
            Rectangle paddle1 = { PADDLE_X_PADDING, player1.yPosition, PADDLE_WIDTH, PADDLE_HEIGHT }; 
            Rectangle paddle2 = { SCREEN_WIDTH - (PADDLE_X_PADDING * 2), player2.yPosition, PADDLE_WIDTH, PADDLE_HEIGHT }; 

            switch (screen) {
                case 'M':
                    drawStartScreen();
                    break;
                case 'G':
                    DrawRectangleRec(paddle1, player1.color);
                    DrawRectangleRec(paddle2, player2.color);

                    DrawCircleV(ball.position, BALL_RADIUS, ball.color);

                    drawScore();

                    if (isScored) {
                        drawRestartText();
                    }
                    break;
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

bool touchTopBoundary(int yPos) {
    return yPos <= 0;
}

bool touchBottomBoundary(int yPos) {
    return yPos >= SCREEN_HEIGHT;
}

bool checkBallPaddleCollision(Ball *ball, Paddle *paddle) {
    float ballLeft = ball->position.x - BALL_RADIUS;
    float ballRight = ball->position.x + BALL_RADIUS;
    float ballTop = ball->position.y - BALL_RADIUS;
    float ballBottom = ball->position.y + BALL_RADIUS;

    float paddleLeft = paddle == &player1 ? PADDLE_X_PADDING : SCREEN_WIDTH - (PADDLE_X_PADDING * 2) - PADDLE_WIDTH;
    float paddleRight = paddleLeft + PADDLE_WIDTH;
    float paddleTop = paddle->yPosition;
    float paddleBottom = paddle->yPosition + PADDLE_HEIGHT;

    return ballLeft <= paddleRight && ballRight >= paddleLeft && ballTop <= paddleBottom && ballBottom >= paddleTop;
}

void updatePaddlePosition(Paddle *player, float delta) {
    bool topBoundary = touchTopBoundary(player->yPosition);
    bool bottomBoundary = touchBottomBoundary((player->yPosition + PADDLE_HEIGHT));

    if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && !topBoundary) player->yPosition -= PADDLE_VERT_SPEED*delta; 
    if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && !bottomBoundary) player->yPosition += PADDLE_VERT_SPEED*delta;
}

/*
 * TODO: randomly generate the x and y slope of the trajectory the ball should initially follow.
*/
Slope generateSlope() {
    Slope slope;
    slope.x = GetRandomValue(0, 1) == 0 ? -1 : 1; // Randomly choose the horizontal direction
    slope.y = GetRandomValue(0, 1) == 0 ? -1 : 1; // Randomly choose the vertical direction
    return slope;
}

/* 
 * TODO: Somehow use generateSlope function to calculate trajectory of ball depending on if it's the start
 * of the round or if it just bounced off an object.
*/
void updateBallPosition(Ball *ball, float delta) {
    bool topBoundary = touchTopBoundary((ball->position.y - BALL_RADIUS));
    bool bottomBoundary = touchBottomBoundary((ball->position.y + BALL_RADIUS));

    if (topBoundary || bottomBoundary) {
        ball_path.y *= -1; // Reverse the vertical direction of the ball
    }

    if (checkBallPaddleCollision(ball, &player1) || checkBallPaddleCollision(ball, &player2)) {
        ball_path.x *= -1; // Reverse the horizontal direction of the ball
    }

    ball->position = (Vector2){ ball->position.x + ball_path.x * BALL_HOR_SPEED * delta, ball->position.y + ball_path.y * BALL_VERT_SPEED * delta };
}

bool checkIfScore(Ball *ball) {
    bool isScore = false;

    if (ball->position.x >= SCREEN_WIDTH) {
        score.player1Score++;
        isScore = true;
    } else if(ball->position.x <= 0) {
        score.player2Score++;
        isScore = true;
    }

    return isScore;
}

void drawScore() {
    char scoreText[20];
    sprintf(scoreText, "%d - %d", score.player1Score, score.player2Score);
    int textWidth = MeasureText(scoreText, 20);
    DrawText(scoreText, (SCREEN_WIDTH - textWidth) / 2, 20, 20, BLACK);
}

void drawRestartText() {
    const char *text = "Ready? Press R";
    int textWidth = MeasureText(text, 30);
    DrawText(text, (SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT / 2, 30, BLACK);
}

void readRestart(Ball *ball) {
    if (IsKeyPressed(KEY_R)) {
        isScored = false;
        ball->position = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        ball_path = generateSlope();
    }
}

void drawStartScreen() {
    const char *title = "PONG";
    const char *startText = "Press ENTER to start";
    const char *quitText = "Press ESC to quit";

    int titleWidth = MeasureText(title, 60);
    int startTextWidth = MeasureText(startText, 30);
    int quitTextWidth = MeasureText(quitText, 30);

    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, SCREEN_HEIGHT / 3, 60, BLACK);
    DrawText(startText, (SCREEN_WIDTH - startTextWidth) / 2, SCREEN_HEIGHT / 2, 30, BLACK);
    DrawText(quitText, (SCREEN_WIDTH - quitTextWidth) / 2, (SCREEN_HEIGHT * 2) / 3, 30, BLACK);
}


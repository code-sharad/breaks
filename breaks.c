#include <GL/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 10
#define BALL_SIZE 10
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_WIDTH (WIDTH / BRICK_COLS)
#define BRICK_HEIGHT 30

typedef struct {
    float x, y;
    float dx, dy;
} Ball;

typedef struct {
    float x;
} Paddle;

typedef struct {
    bool alive;
} Brick;

Ball ball = {WIDTH/2, HEIGHT/2, 5, -5};
Paddle paddle = {WIDTH/2 - PADDLE_WIDTH/2};
Brick bricks[BRICK_ROWS][BRICK_COLS];
int score = 0;
bool game_over = false;

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);

    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].alive = true;
        }
    }
}

void drawRect(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawText(float x, float y, const char *string) {
    glRasterPos2f(x, y);
    for (const char *c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (game_over) {
        glColor3f(1.0, 1.0, 1.0);
        char game_over_text[50];
        sprintf(game_over_text, "Game Over! Final Score: %d", score);
        drawText(WIDTH/2 - 100, HEIGHT/2, game_over_text);
        drawText(WIDTH/2 - 100, HEIGHT/2 - 30, "Press 'R' to restart or 'Q' to quit");
    } else {
        // Draw paddle
        glColor3f(1.0, 1.0, 1.0);
        drawRect(paddle.x, 10, PADDLE_WIDTH, PADDLE_HEIGHT);

        // Draw ball
        glColor3f(1.0, 0.0, 0.0);
        drawRect(ball.x - BALL_SIZE/2, ball.y - BALL_SIZE/2, BALL_SIZE, BALL_SIZE);

        // Draw bricks
        for (int i = 0; i < BRICK_ROWS; i++) {
            for (int j = 0; j < BRICK_COLS; j++) {
                if (bricks[i][j].alive) {
                    glColor3f(0.0, 1.0, 0.0);
                    drawRect(j * BRICK_WIDTH, HEIGHT - (i + 1) * BRICK_HEIGHT, BRICK_WIDTH - 1, BRICK_HEIGHT - 1);
                }
            }
        }

        // Draw score
        glColor3f(1.0, 1.0, 1.0);
        char score_text[20];
        sprintf(score_text, "Score: %d", score);
        drawText(10, HEIGHT - 30, score_text);
    }

    glutSwapBuffers();
}

void update(int value) {
    if (!game_over) {
        // Move ball
        ball.x += ball.dx;
        ball.y += ball.dy;

        // Ball collision with walls
        if (ball.x <= 0 || ball.x >= WIDTH) ball.dx = -ball.dx;
        if (ball.y >= HEIGHT) ball.dy = -ball.dy;

        // Ball collision with paddle
        if (ball.y <= PADDLE_HEIGHT + 10 && 
            ball.x >= paddle.x && ball.x <= paddle.x + PADDLE_WIDTH) {
            ball.dy = -ball.dy;
        }

        // Ball collision with bricks
        for (int i = 0; i < BRICK_ROWS; i++) {
            for (int j = 0; j < BRICK_COLS; j++) {
                if (bricks[i][j].alive) {
                    float brickX = j * BRICK_WIDTH;
                    float brickY = HEIGHT - (i + 1) * BRICK_HEIGHT;
                    if (ball.x >= brickX && ball.x <= brickX + BRICK_WIDTH &&
                        ball.y >= brickY && ball.y <= brickY + BRICK_HEIGHT) {
                        bricks[i][j].alive = false;
                        ball.dy = -ball.dy;
                        score += 10;
                    }
                }
            }
        }

        // Game over condition
        if (ball.y < 0) {
            game_over = true;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'r':
        case 'R':
            if (game_over) {
                // Reset game state
                ball.x = WIDTH/2;
                ball.y = HEIGHT/2;
                ball.dx = 5;
                ball.dy = -5;
                paddle.x = WIDTH/2 - PADDLE_WIDTH/2;
                score = 0;
                game_over = false;
                for (int i = 0; i < BRICK_ROWS; i++) {
                    for (int j = 0; j < BRICK_COLS; j++) {
                        bricks[i][j].alive = true;
                    }
                }
            }
            break;
        case 'q':
        case 'Q':
            exit(0);
    }
}

void specialKeys(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_LEFT:
            if (paddle.x > 0) paddle.x -= 20;
            break;
        case GLUT_KEY_RIGHT:
            if (paddle.x < WIDTH - PADDLE_WIDTH) paddle.x += 20;
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Brick Breaker");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}

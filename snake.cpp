#include <SDL.h>
#include <vector>
#include <iostream>

// TODO: allow player to select the next move if key pressed just before the snake moves
// TODO: set const values to reset and init the game variables

const int GRID_SZ_X = 25,
          GRID_SZ_Y = 25,
          CELL_SIZE = 20,
          WIDTH = GRID_SZ_X * CELL_SIZE,
          HEIGHT = GRID_SZ_Y * (CELL_SIZE + 1);

typedef SDL_Point pos;
enum DIR {
    RIGHT = 1,
    LEFT = -1,
    UP = 2,
    DOWN = -2
};

Uint8 point;
bool gameOver;
pos head {GRID_SZ_X / 2, GRID_SZ_Y / 2};
pos apple {};
std::vector<pos> body {{head.x, head.y + 1}};
DIR dir = UP;
DIR newDir = UP;

#define POSEQUALSTO(A, B) (A.x == B.x && A.y == B.y)


bool inBody(pos p) {
    for (const pos &body_ : body) {
        if (POSEQUALSTO(p, body_)) {
            return true;
        }
    }
    return false;
}

void reposApple() {
    do
    {
        apple.x = rand() % GRID_SZ_X;
        apple.y = rand() % GRID_SZ_Y;
    } while (inBody(apple) || POSEQUALSTO(apple, head));
}

void updateSnake() {
    pos oldHead = head;
    dir = newDir;
    switch (dir) {
        case UP: head.y = (head.y + GRID_SZ_Y - 1) % GRID_SZ_Y; break;
        case DOWN: head.y = (head.y + 1) % GRID_SZ_Y; break;
        case RIGHT: head.x = (head.x + 1) % GRID_SZ_X; break;
        case LEFT: head.x = (head.x + GRID_SZ_X - 1) % GRID_SZ_Y; break;
    }

    
    if (POSEQUALSTO(head, apple)) {
        point++;
        SDL_LogInfo(SDL_LOG_CATEGORY_CUSTOM, "Score: ", point);
        body.push_back(pos{});
        reposApple();
    }
    else if (inBody(head)) {
        gameOver = true;
    }

    body[0] = oldHead;
    for (int i = body.size() - 1; i >= 1; i--)
    {
        body[i] = body[i - 1];
    }
}

void reset() {
    point = 0;
    gameOver = false;
    head = {GRID_SZ_X / 2, GRID_SZ_Y / 2};
    reposApple();
    body = {{head.x, head.y + 1}};
    dir = newDir = UP;
}

SDL_Window *window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

#define GETRECT(POS, OFFSET) {POS.x * CELL_SIZE - OFFSET, POS.y * CELL_SIZE - OFFSET, CELL_SIZE + 2 * OFFSET, CELL_SIZE + 2 * OFFSET}
#define APPLE_COL 255,   0,   0, 255
#define SNAKE_COL   0,   0,   0, 255
#define BACKG_COL 255, 255, 255, 255

// snake and apple
void drawForeground() {
    SDL_SetRenderDrawColor(renderer, SNAKE_COL);
    SDL_Rect head_rect = GETRECT(head, 2);
    SDL_RenderFillRect(renderer, &head_rect);

    for (const pos &body_: body) {
        SDL_Rect body_rect = GETRECT(body_, -1);
        SDL_RenderFillRect(renderer, &body_rect);
    }

    SDL_SetRenderDrawColor(renderer, APPLE_COL);
    SDL_Rect apple_rect = GETRECT(apple, -3);
    SDL_RenderFillRect(renderer, &apple_rect);
}

const float DIGIT_WITDH = WIDTH / 8.0;

#define SCOREBOARD_FG_COL 175, 175, 175, 255
#define SCOREBOARD_BG_COL 210, 210, 210, 255

// as binary B)
// not that i am lazy to implement a text rendering system
void drawScoreboard() {
    Uint8 mask = 0b10000000;
    SDL_SetRenderDrawColor(renderer, SCOREBOARD_BG_COL);
    SDL_FRect rect {0, CELL_SIZE * GRID_SZ_Y, WIDTH, CELL_SIZE};
    SDL_RenderFillRectF(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, SCOREBOARD_FG_COL);
    for (size_t i = 0; i < 8; i++) {
        rect = {i * DIGIT_WITDH, CELL_SIZE * GRID_SZ_Y, DIGIT_WITDH, CELL_SIZE};
        if (point & mask)
            SDL_RenderFillRectF(renderer, &rect);

        mask >>= 1;
    }
}

void updateRender() {
    SDL_SetRenderDrawColor(renderer, BACKG_COL);
    SDL_RenderClear(renderer);
    drawForeground();
    drawScoreboard();
    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Event event;

    Uint64 _now = SDL_GetPerformanceCounter();
    Uint64 _last = 0;
    double deltaTime;

    double startTimeLeftToMove = 100.0; // in ms
    double timeLeftToMove = startTimeLeftToMove;

    bool running = true;
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN: {
                    switch(event.key.keysym.sym) {
                        case SDLK_w: newDir = UP; break;
                        case SDLK_s: newDir = DOWN; break;
                        case SDLK_d: newDir = RIGHT; break;
                        case SDLK_a: newDir = LEFT; break;

                        case SDLK_r: reset(); break;
                    }
                    // This way, the snake will not be able to go to the opposite direction
                    newDir = newDir * -1 == dir ? dir : newDir;
                    break;
                }
            }
        }

        _last = _now;
        _now = SDL_GetPerformanceCounter();
        deltaTime = (double)((_now - _last) * 1000 / (double)SDL_GetPerformanceFrequency());

        updateRender();

        if (gameOver) {
            continue;
        }

        if (timeLeftToMove > 0) {
            timeLeftToMove -= deltaTime;
        }
        else {
            updateSnake();

            timeLeftToMove = startTimeLeftToMove;
        }
    }

    SDL_Quit();
    return 0;
}
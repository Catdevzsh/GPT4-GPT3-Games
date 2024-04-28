#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SNAKE_SIZE 20
#define SNAKE_LENGTH 100
#define FOOD_SIZE 20

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[SNAKE_LENGTH];
    int length;
    Point dir;
} Snake;

void init_snake(Snake *snake) {
    snake->length = 5;
    for (int i = 0; i < snake->length; ++i) {
        snake->body[i].x = WINDOW_WIDTH / 2 + i * SNAKE_SIZE;
        snake->body[i].y = WINDOW_HEIGHT / 2;
    }
    snake->dir.x = -SNAKE_SIZE;
    snake->dir.y = 0;
}

void move_snake(Snake *snake) {
    Point next = {snake->body[0].x + snake->dir.x, snake->body[0].y + snake->dir.y};
    for (int i = snake->length - 1; i > 0; --i) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0] = next;
}

bool check_collision_with_walls(Point head) {
    return head.x < 0 || head.x >= WINDOW_WIDTH || head.y < 0 || head.y >= WINDOW_HEIGHT;
}

bool check_collision_with_self(Snake *snake) {
    for (int i = 1; i < snake->length; ++i) {
        if (snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y) {
            return true;
        }
    }
    return false;
}

bool check_food_collision(Point head, Point food) {
    return head.x == food.x && head.y == food.y;
}

Point generate_food() {
    int x = rand() % (WINDOW_WIDTH / FOOD_SIZE) * FOOD_SIZE;
    int y = rand() % (WINDOW_HEIGHT / FOOD_SIZE) * FOOD_SIZE;
    return (Point){x, y};
}

void update_direction(Snake *snake, SDL_Event *event) {
    switch (event->key.keysym.sym) {
        case SDLK_w: if(snake->dir.y == 0) { snake->dir.x = 0; snake->dir.y = -SNAKE_SIZE; } break;
        case SDLK_s: if(snake->dir.y == 0) { snake->dir.x = 0; snake->dir.y = SNAKE_SIZE; } break;
        case SDLK_a: if(snake->dir.x == 0) { snake->dir.x = -SNAKE_SIZE; snake->dir.y = 0; } break;
        case SDLK_d: if(snake->dir.x == 0) { snake->dir.x = SNAKE_SIZE; snake->dir.y = 0; } break;
    }
}

void grow_snake(Snake *snake) {
    if (snake->length < SNAKE_LENGTH) {
        snake->body[snake->length] = snake->body[snake->length - 1];
        snake->length++;
    }
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetWindowResizable(window, SDL_FALSE); // Disable the maximize button

    Snake snake;
    init_snake(&snake);
    Point food = generate_food();
    int score = 0;

    bool running = true;
    SDL_Event event;
    Uint32 start_time;

    while (running) {
        start_time = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                update_direction(&snake, &event);
            }
        }

        move_snake(&snake);

        if (check_collision_with_walls(snake.body[0]) || check_collision_with_self(&snake)) {
            running = false; // Game over
        }

        if (check_food_collision(snake.body[0], food)) {
            grow_snake(&snake);
            food = generate_food();
            score++;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw food
        SDL_Rect food_rect = {food.x, food.y, FOOD_SIZE, FOOD_SIZE};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &food_rect);

        // Draw snake
        for (int i = 0; i < snake.length; ++i) {
            SDL_Rect rect = {snake.body[i].x, snake.body[i].y, SNAKE_SIZE, SNAKE_SIZE};
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);

        // Cap the frame rate
        Uint32 frame_time = SDL_GetTicks() - start_time;
        if (frame_time < 1000 / 15) {
            SDL_Delay(1000 / 15 - frame_time);
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Game Over! Your score was: %d\n", score);
    return 0;
}

#include "constants.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Game_Object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} Game_Object;
Game_Object ball, paddle_1, paddle_2;

int last_frame_time = 0;
int game_is_running = false;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int initialize_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS);

    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    return true;
}

void setup() {
    int middleHeight = WINDOW_HEIGHT / 2;
    int middleWidth = WINDOW_WIDTH / 2;

    ball.height = 10;
    ball.width = 10;
    ball.x = middleWidth - ball.width / 2;
    ball.y = middleHeight - ball.height / 2;
    ball.vel_x = 0;
    ball.vel_y = 0;

    paddle_1.height = 100;
    paddle_1.width = 10;
    paddle_1.x = 25;
    paddle_1.y = middleHeight - paddle_1.height / 2;
    paddle_1.vel_x = 0;
    paddle_1.vel_y = 0;

    paddle_2.height = 100;
    paddle_2.width = 10;
    paddle_2.x = WINDOW_WIDTH - 25 - paddle_2.width;
    paddle_2.y = middleHeight - paddle_2.height / 2;
    paddle_2.vel_x = 0;
    paddle_2.vel_y = 0;
}

void keep_ball_on_screen() {
    if ((ball.y + ball.height) >= WINDOW_HEIGHT) {
        ball.vel_y = -BALL_VELOCITY;
    } else if (ball.y <= 0) {
        ball.vel_y = BALL_VELOCITY;
    }

    if ((ball.x + ball.width) >= WINDOW_WIDTH) {
        ball.vel_x = -BALL_VELOCITY;
    } else if (ball.x <= 0) {
        ball.vel_x = BALL_VELOCITY;
    }
}

void keep_paddle_on_screen(Game_Object *paddle) {
    if (paddle->y <= 0) {
        paddle->vel_y = 0;
    } else if (paddle->y + paddle->height >= WINDOW_HEIGHT) {
        paddle->vel_y = 0;
    }
}

void move_right_paddle(SDL_Event event) {
    if (event.key.keysym.sym == SDLK_UP && paddle_2.y > 0) {
        paddle_2.vel_y = -PADDLE_VELOCITY;
    }
    if (event.key.keysym.sym == SDLK_DOWN &&
        (paddle_2.y + paddle_2.height) < WINDOW_HEIGHT) {
        paddle_2.vel_y = +PADDLE_VELOCITY;
    }
}

void move_left_paddle(SDL_Event event) {
    if (event.key.keysym.sym == SDLK_w && paddle_1.y > 0) {
        paddle_1.vel_y = -PADDLE_VELOCITY;
    }
    if (event.key.keysym.sym == SDLK_s &&
        (paddle_1.y + paddle_1.height) < WINDOW_HEIGHT) {
        paddle_1.vel_y = PADDLE_VELOCITY;
    }
}

void detect_ball_paddle_collision(Game_Object paddle, int paddle_side_multiplier) {
    if (ball.y < paddle.y || ball.y > (paddle.y + paddle.height)) {
        return;
    }

    if (paddle_side_multiplier == RIGHT) {
        if (ball.x >= paddle.x)
            ball.vel_x = BALL_VELOCITY * paddle_side_multiplier;
    } else {
        if (ball.x < (paddle.x + paddle.width)) {
            ball.vel_x = BALL_VELOCITY * paddle_side_multiplier;
        }
    }
}

void detect_score() {
    if (ball.x >= (WINDOW_WIDTH - 10) || ball.x <= 10) {
        setup();
    }
}

void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        game_is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = false;

        move_left_paddle(event);
        move_right_paddle(event);

        if (event.key.keysym.sym == SDLK_SPACE && ball.vel_x == 0 && ball.vel_y == 0) {
            ball.vel_x = BALL_VELOCITY;
            ball.vel_y = BALL_VELOCITY;
        }
        break;

    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN) {
            paddle_2.vel_y = 0;
        }
        if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_s) {
            paddle_1.vel_y = 0;
        }
        break;

    default:
        break;
    }
}

void update() {
    // Logic to keep a fixed timestamp

    // Sleep until we reach the frame target time
    // int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    // if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    //     SDL_Delay(time_to_wait);

    // Get a delta time factor converted to seconds to be used to update my objects
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();

    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;

    keep_ball_on_screen();

    paddle_1.y += paddle_1.vel_y * delta_time;
    paddle_2.y += paddle_2.vel_y * delta_time;

    keep_paddle_on_screen(&paddle_1);
    keep_paddle_on_screen(&paddle_2);

    detect_ball_paddle_collision(paddle_1, LEFT);
    detect_ball_paddle_collision(paddle_2, RIGHT);
    detect_score();
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // TODO: Start drawing game objects
    // Draw the "ball"
    SDL_Rect ball_rect = {ball.x, ball.y, ball.width, ball.height};
    SDL_Rect paddle_1_rect = {paddle_1.x, paddle_1.y, paddle_1.width, paddle_1.height};
    SDL_Rect paddle_2_rect = {paddle_2.x, paddle_2.y, paddle_2.width, paddle_2.height};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);
    SDL_RenderFillRect(renderer, &paddle_1_rect);
    SDL_RenderFillRect(renderer, &paddle_2_rect);

    SDL_RenderPresent(renderer);
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    game_is_running = initialize_window();

    setup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
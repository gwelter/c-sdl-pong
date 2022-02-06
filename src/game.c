#include "constants.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int last_frame_time = 0;
int game_is_running = FALSE;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

struct ball {
    float x;
    float y;
    float width;
    float height;
} ball;

int initialize_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return FALSE;
    }

    window = SDL_CreateWindow(NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS);

    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return FALSE;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return FALSE;
    }

    return TRUE;
}

void setup() {
    ball.x = 20;
    ball.y = 20;
    ball.height = 15;
    ball.width = 15;
}

void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        game_is_running = FALSE;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = FALSE;
        break;

    default:
        break;
    }
}

void update() {
    // Logic to keep a fixed timestamp

    // Sleep until we reach the frame target time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    // Get a delta time factor converted to seconds to be used to update my objects
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();

    ball.x += 200 * delta_time;
    ball.y += 200 * delta_time;
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // TODO: Start drawing game objects
    // Draw the "ball"
    SDL_Rect ball_rect = {ball.x, ball.y, ball.width, ball.height};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

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
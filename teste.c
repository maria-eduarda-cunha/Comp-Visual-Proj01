// #include <SDL3/SDL.h>
// #include <stdio.h>

// int main(int argc, char* argv[]) {
//     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//         printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
//         return 1;
//     }

//     SDL_Window *window = SDL_CreateWindow(
//         "Hello SDL3",
//         800,
//         600,
//         0
//     );

//     if (!window) {
//         printf("Erro ao criar janela: %s\n", SDL_GetError());
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

//     int running = 1;
//     SDL_Event event;

//     while (running) {
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_EVENT_QUIT) {
//                 running = 0;
//             }
//         }

//         SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // azul
//         SDL_RenderClear(renderer);
//         SDL_RenderPresent(renderer);
//     }

//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     SDL_Quit();

//     return 0;
// }

#include <stdio.h>

int main() {
    printf("FUNCIONANDO\n");
    return 0;
}
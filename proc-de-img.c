#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

int main(int argc, char* argv[]){
if (argc != 2){
    fprintf(stderr, "Uso: %s <caminho_da_imagem.ext>\n", argv[0]);
    return 1;
}

if (!SDL_Init(SDL_INIT_VIDEO)){
    fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError);
    return 1;
}


const char* caminho_imagem = argv[1];
SDL_Surface* surface_original = IMG_Load(caminho_imagem);

if (!surface_original){
    fprintf(stderr, "Erro: Nao foi possivel carregar a imagem\n");
    fprintf(stderr, "Verifique se o arquivo existe e que esteja em um formato válido (PNG, JPG, BMP)\n");
    fprintf(stderr, "Detalhes do erro: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}

printf("Imagem '%s' carregada!\n", caminho_imagem);

SDL_DestroySurface(surface_original);

SDL_Surface* imagem = SDL_ConvertSurface(surface_original, SDL_PIXELFORMAT_RGBA32);

if(!imagem){
    fprintf(stderr, "Erro ao converter o formato da imagem: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}
}
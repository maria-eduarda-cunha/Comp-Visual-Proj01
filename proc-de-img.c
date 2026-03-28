#include <stdio.h>
#include <stdbool.h>    
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

int main(int argc, char* argv[]){
if (argc != 2){
    fprintf(stderr, "Uso: %s <caminho_da_imagem.ext>\n", argv[0]);
    return 1;
}

// Inicializa o SDL3
if (!SDL_Init(SDL_INIT_VIDEO)){
    fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError);
    return 1;
}

const char* caminho_imagem = argv[1];

// Carrega a imagem em PNG, JPG, BMP
SDL_Surface* surface_original = IMG_Load(caminho_imagem);

// Caso o arquivo não seja suportado, faz o tratamento sem 
if (!surface_original){
    fprintf(stderr, "Erro: Nao foi possivel carregar a imagem\n");
    fprintf(stderr, "Verifique se o arquivo existe e que esteja em um formato válido (PNG, JPG, BMP)\n");
    fprintf(stderr, "Detalhes do erro: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}

printf("Imagem '%s' carregada!\n", caminho_imagem);

// Converte a imagem para RGBA32
SDL_Surface* imagem = SDL_ConvertSurface(surface_original, SDL_PIXELFORMAT_RGBA32);
SDL_DestroySurface(surface_original);

if(!imagem){
    fprintf(stderr, "Erro ao converter o formato da imagem: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}


// Analise da imagem (ver se é colorida)
bool colorida = false;
Uint32* pixels = (Uint32*)imagem->pixels;
int total_pixels = imagem-> w * imagem->h;


const SDL_PixelFormatDetails* detalhes = SDL_GetPixelFormatDetails(imagem->format);

// Se RGB igual, então imagem colorida
for(int i = 0; i < total_pixels; ++i){
    Uint8 r, g, b, a;
    SDL_GetRGBA(pixels[i], detalhes, NULL, &r, &g, &b, &a);

    if(r != g || r != b || g != b) {
        colorida = true;
        break;
        }
    }

    // Conversão para escala de cinza
    if(colorida){
        printf("Imagem Colorida. Fazendo conversão para escala de cinza!\n");

        for(int i = 0; i < total_pixels; ++i){
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixels[i], detalhes, NULL, &r, &g, &b, &a);

            // Formula para conversão
            Uint8 y = (Uint8)((0.2125 * r) + (0.7154 * g) + (0.0721 * b));

            pixels[i] = SDL_MapRGBA(detalhes, NULL, y, y, y, a);
        }
        printf("Conversão Finalizada\n");
    } else {
        printf("Imagem convertida para escala de cinza!\n");
    }


SDL_DestroySurface(imagem);
SDL_Quit();

return 0;
}
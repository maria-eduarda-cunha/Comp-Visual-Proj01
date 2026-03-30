#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>

void calcular_histograma(SDL_Surface* img, int hist[256], int total_pixels) {
    memset(hist, 0, 256 * sizeof(int));

    Uint32* pixels = (Uint32*)img->pixels;
    const SDL_PixelFormatDetails* detalhes = SDL_GetPixelFormatDetails(img->format);

    for (int i = 0; i < total_pixels; i++) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], detalhes, NULL, &r, &g, &b, &a);
        hist[r]++;
    }
}

void equalizar_histograma(SDL_Surface* imagem, int hist[256], int total_pixels) {
    int soma_acumulada_hist[256] = {0};

    soma_acumulada_hist[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        soma_acumulada_hist[i] = soma_acumulada_hist[i - 1] + hist[i];
    }

    Uint8 mapa_equalizacao[256];

    int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (soma_acumulada_hist[i] != 0) {
            cdf_min = soma_acumulada_hist[i];
            break;
        }
    }

    if (total_pixels == cdf_min) {
        return;
    }

    for (int i = 0; i < 256; i++) {
        double valor = ((double)(soma_acumulada_hist[i] - cdf_min) * 255.0) /
                       (double)(total_pixels - cdf_min);

        if (valor < 0) valor = 0;
        if (valor > 255) valor = 255;

        mapa_equalizacao[i] = (Uint8)(valor + 0.5);
    }

    Uint32* pixels = (Uint32*)imagem->pixels;
    const SDL_PixelFormatDetails* formato_pixel = SDL_GetPixelFormatDetails(imagem->format);

    for (int i = 0; i < total_pixels; i++) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], formato_pixel, NULL, &r, &g, &b, &a);

        Uint8 nova_intensidade = mapa_equalizacao[r];
        pixels[i] = SDL_MapRGBA(formato_pixel, NULL, nova_intensidade, nova_intensidade, nova_intensidade, a);
    }
}

double calcular_media(int hist[256], int total_pixels) {
    double soma = 0;

    for (int i = 0; i < 256; i++) {
        soma += hist[i] * i;
    }

    return soma / total_pixels;
}

double calcular_desvio(int hist[256], double media, int total_pixels) {
    double variancia = 0;

    for (int i = 0; i < 256; i++) {
        variancia += hist[i] * ((i - media) * (i - media));
    }
    variancia /= total_pixels;

    return sqrt(variancia);
}

const char* classificar_intensidade(double media) {
    if (media < 85)
        return "Escura";
    else if (media < 170)
        return "Media";
    else
        return "Clara";
}

const char* classificar_contraste(double desvio) {
    if (desvio < 40)
        return "Baixo";
    else if (desvio < 80)
        return "Medio";
    else
        return "Alto";
}

void restaurar_imagem_original(SDL_Surface* imagem_origem, SDL_Surface* imagem_destino) {
    Uint32* pixel_origem = (Uint32*)imagem_origem->pixels;
    Uint32* pixel_destino = (Uint32*)imagem_destino->pixels;

    int total = imagem_destino->w * imagem_destino->h;

    for (int i = 0; i < total; i++) {
        pixel_destino[i] = pixel_origem[i];
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <caminho_da_imagem.ext>\n", argv[0]);
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (!TTF_Init()) {
        fprintf(stderr, "Erro ao inicializar SDL_ttf: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    const char *caminho_imagem = argv[1];

    SDL_Surface *surface_original = IMG_Load(caminho_imagem);
    if (!surface_original) {
        fprintf(stderr, "Erro: Nao foi possivel carregar a imagem\n");
        fprintf(stderr, "Verifique se o arquivo existe e esta em um formato valido (PNG, JPG, BMP)\n");
        fprintf(stderr, "Detalhes do erro: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    printf("Imagem '%s' carregada!\n", caminho_imagem);

    SDL_Surface *imagem = SDL_ConvertSurface(surface_original, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface_original);

    if (!imagem) {
        fprintf(stderr, "Erro ao converter o formato da imagem: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    int total_pixels = imagem->w * imagem->h;
    Uint32 *pixels = (Uint32 *)imagem->pixels;
    const SDL_PixelFormatDetails *detalhes = SDL_GetPixelFormatDetails(imagem->format);
    bool colorida = false;

    for (int i = 0; i < total_pixels; ++i) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], detalhes, NULL, &r, &g, &b, &a);

        if (r != g || r != b || g != b) {
            colorida = true;
            break;
        }
    }

    if (colorida) {
        printf("Imagem colorida. Convertendo para escala de cinza...\n");

        for (int i = 0; i < total_pixels; ++i) {
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixels[i], detalhes, NULL, &r, &g, &b, &a);

            Uint8 y = (Uint8)((0.2125 * r) + (0.7154 * g) + (0.0721 * b));
            pixels[i] = SDL_MapRGBA(detalhes, NULL, y, y, y, a);
        }

        printf("Conversao finalizada!\n");
    } else {
        printf("Imagem ja esta em escala de cinza!\n");
    }

    SDL_Surface* imagem_original = SDL_ConvertSurface(imagem, SDL_PIXELFORMAT_RGBA32);
    if (!imagem_original) {
        fprintf(stderr, "Erro ao copiar a imagem original: %s\n", SDL_GetError());
        SDL_DestroySurface(imagem);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    int hist[256] = {0};
    calcular_histograma(imagem, hist, total_pixels);

    int max = 0;
    for (int i = 0; i < 256; i++) {
        if (hist[i] > max) max = hist[i];
    }

    double media = calcular_media(hist, total_pixels);
    const char* intensidade = classificar_intensidade(media);

    printf("Media de intensidade: %.2f (%s)\n", media, intensidade);

    double desvio = calcular_desvio(hist, media, total_pixels);
    const char* contraste = classificar_contraste(desvio);

    printf("Desvio padrao: %.2f (%s)\n", desvio, contraste);

    TTF_Font *fonte = TTF_OpenFont("04B_03__.ttf", 16);
    if (!fonte) {
        fprintf(stderr, "Erro ao carregar fonte: %s\n", SDL_GetError());
        SDL_DestroySurface(imagem_original);
        SDL_DestroySurface(imagem);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window *janela_img = SDL_CreateWindow("Imagem", imagem->w, imagem->h, 0);
    SDL_Renderer *renderer_img = SDL_CreateRenderer(janela_img, NULL);
    SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer_img, imagem);

    int largura_hist = 512;
    int altura_hist = 340;
    int margem_topo = 40;
    int margem_botao = 50;

    SDL_Window *janela_hist = SDL_CreateWindow("Histograma", largura_hist, altura_hist, 0);
    SDL_Renderer *renderer_hist = SDL_CreateRenderer(janela_hist, NULL);

    SDL_FRect botao = {10.0f, (float)(altura_hist - 40), 120.0f, 30.0f};
    bool imagem_equalizada = false;
    bool mouse_em_cima = false;
    bool mouse_pressionado = false;

    bool rodando = true;
    SDL_Event e;

    while (rodando) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                rodando = false;
            }

             SDL_WindowID id_janela_hist = SDL_GetWindowID(janela_hist);

            // seta do mouse
            if(e.type == SDL_EVENT_MOUSE_MOTION){
                if(e.motion.windowID == id_janela_hist){
                    float mouse_x = e.motion.x;
                    float mouse_y = e.motion.y;
                }

                // Verifica se seta em cima do botão
                mouse_em_cima = (mouse_x >= botao.x && mouse_x <= botao.x + botao.w && //seta entre borda esquerda e direita do botão
                                mouse_y >= botao.y && mouse_y <= botao.y + botao.h); //seta entre borda superior e inferior do botão
            }

            if (e.type == SDL_EVENT_MOUSE_MOTION) {
                float mouse_x = e.motion.x;
                float mouse_y = e.motion.y;

                mouse_em_cima = (mouse_x >= botao.x && mouse_x <= botao.x + botao.w &&
                                 mouse_y >= botao.y && mouse_y <= botao.y + botao.h);
            }

            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (mouse_em_cima) {
                    mouse_pressionado = true;
                }
            }

            if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (mouse_pressionado && mouse_em_cima) {
                    imagem_equalizada = !imagem_equalizada;

                    if (imagem_equalizada) {
                        equalizar_histograma(imagem, hist, total_pixels);
                    } else {
                        restaurar_imagem_original(imagem_original, imagem);
                    }

                    calcular_histograma(imagem, hist, total_pixels);

                    max = 0;
                    for (int i = 0; i < 256; i++) {
                        if (hist[i] > max) max = hist[i];
                    }

                    media = calcular_media(hist, total_pixels);
                    intensidade = classificar_intensidade(media);

                    desvio = calcular_desvio(hist, media, total_pixels);
                    contraste = classificar_contraste(desvio);

                    SDL_DestroyTexture(textura);
                    textura = SDL_CreateTextureFromSurface(renderer_img, imagem);
                }
                mouse_pressionado = false;
            }

            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_S) {
                    printf("\nSalvando imagem\n");

                    if (IMG_SavePNG(imagem, "output_image.png")) {
                        printf("Imagem salva com sucesso!\n");
                    } else {
                        fprintf(stderr, "Erro ao salvar a imagem: %s\n", SDL_GetError());
                    }
                }
            }
        }

        SDL_RenderClear(renderer_img);
        SDL_RenderTexture(renderer_img, textura, NULL, NULL);
        SDL_RenderPresent(renderer_img);

        SDL_SetRenderDrawColor(renderer_hist, 0, 0, 0, 255);
        SDL_RenderClear(renderer_hist);

        char info_texto[200];
        snprintf(
            info_texto,
            sizeof(info_texto),
            "Claridade: %s | Contraste: %s",
            intensidade,
            contraste
        );

        SDL_Color text_color = {255, 255, 255, 255};

        SDL_Surface *surface_texto = TTF_RenderText_Blended(fonte, info_texto, 0, text_color);
        if (surface_texto) {
            SDL_Texture *textura_info = SDL_CreateTextureFromSurface(renderer_hist, surface_texto);

            SDL_FRect info_rect = {
                10.0f,
                10.0f,
                (float)surface_texto->w,
                (float)surface_texto->h
            };

            SDL_RenderTexture(renderer_hist, textura_info, NULL, &info_rect);
            SDL_DestroyTexture(textura_info);
            SDL_DestroySurface(surface_texto);
        }

        SDL_SetRenderDrawColor(renderer_hist, 255, 255, 255, 255);

        int largura_barra = largura_hist / 256;
        int altura_desenho = altura_hist - margem_topo - margem_botao;

        for (int i = 0; i < 256; i++) {
            int h = (max > 0) ? (hist[i] * altura_desenho) / max : 0;

            SDL_RenderLine(
                renderer_hist,
                (float)(i * largura_barra),
                (float)(altura_hist - margem_botao),
                (float)(i * largura_barra),
                (float)(altura_hist - margem_botao - h)
            );
        }

        if (mouse_pressionado)
            SDL_SetRenderDrawColor(renderer_hist, 0, 0, 139, 255);
        else if (mouse_em_cima)
            SDL_SetRenderDrawColor(renderer_hist, 173, 216, 230, 255);
        else
            SDL_SetRenderDrawColor(renderer_hist, 0, 0, 255, 255);

        SDL_RenderFillRect(renderer_hist, &botao);

        const char* texto_botao = imagem_equalizada ? "Ver original" : "Equalizar";

        SDL_Surface* text_surface = TTF_RenderText_Blended(fonte, texto_botao, 0, text_color);
        if (text_surface) {
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer_hist, text_surface);

            SDL_FRect text_rect = {
                botao.x + 10.0f,
                botao.y + 5.0f,
                (float)text_surface->w,
                (float)text_surface->h
            };

            SDL_RenderTexture(renderer_hist, text_texture, NULL, &text_rect);

            SDL_DestroyTexture(text_texture);
            SDL_DestroySurface(text_surface);
        }

        SDL_RenderPresent(renderer_hist);
    }

    SDL_DestroyTexture(textura);
    SDL_DestroySurface(imagem_original);
    SDL_DestroySurface(imagem);

    SDL_DestroyRenderer(renderer_img);
    SDL_DestroyWindow(janela_img);

    SDL_DestroyRenderer(renderer_hist);
    SDL_DestroyWindow(janela_hist);

    TTF_CloseFont(fonte);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>

// void normalizar_histograma(SDL_Surface* img, int hist[256], int total_pixels){
//     
// };
// 
//
// void restaurar_imagem_original(SDL_Surface* img_orig, SDL_Surface* img){
//     
// };
//
//
// void analisar_histograma(SDL_Surface* imagem, int total_pixels){
//
// }

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <caminho_da_imagem.ext>\n", argv[0]);
        return 1;
    }

    // Inicializa o SDL3
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

    // Carrega a imagem
    SDL_Surface *surface_original = IMG_Load(caminho_imagem);

    if (!surface_original) {
        fprintf(stderr, "Erro: Não foi possível carregar a imagem\n");
        fprintf(stderr, "Verifique se o arquivo existe e está em um formato válido (PNG, JPG, BMP)\n");
        fprintf(stderr, "Detalhes do erro: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    printf("Imagem '%s' carregada!\n", caminho_imagem);

    // Converte para RGBA32
    SDL_Surface *imagem = SDL_ConvertSurface(surface_original, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface_original);

    // Salva cópia de imagem em escala de cinza antes de ser normalizada
    // Usa para alternar entre versão original e normalizada
    SDL_Surface* imagem_original = SDL_ConvertSurface(imagem, SDL_PIXELFORMAT_RGBA32);

    // Erro conversão formato de imagem
    if (!imagem) {
        fprintf(stderr, "Erro ao converter o formato da imagem: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // ===== ANALISE DE COR =====
    bool colorida = false;
    Uint32 *pixels = (Uint32 *)imagem->pixels;
    int total_pixels = imagem->w * imagem->h;

    const SDL_PixelFormatDetails *detalhes = SDL_GetPixelFormatDetails(imagem->format);

    for (int i = 0; i < total_pixels; ++i) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], detalhes, NULL, &r, &g, &b, &a);

        if (r != g || r != b || g != b) {
            colorida = true;
            break;
        }
    }

    // ===== CONVERSÃO PARA ESCALA DE CINZA =====
    if (colorida) {
        printf("Imagem colorida. Convertendo para escala de cinza...\n");

        for (int i = 0; i < total_pixels; ++i) {
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixels[i], detalhes, NULL, &r, &g, &b, &a);

            Uint8 y = (Uint8)((0.2125 * r) + (0.7154 * g) + (0.0721 * b));
            pixels[i] = SDL_MapRGBA(detalhes, NULL, y, y, y, a);
        }

        printf("Conversão finalizada!\n");
    } else {
        printf("Imagem já está em escala de cinza!\n");
    }

    // ===== HISTOGRAMA =====
    int hist[256] = {0};

    for (int i = 0; i < total_pixels; i++) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], detalhes, NULL, &r, &g, &b, &a);
        hist[r]++;
    }

    // Normalização
    int max = 0;
    for (int i = 0; i < 256; i++) {
        if (hist[i] > max) max = hist[i];
    }

    // Média de intensidade
    double soma = 0;
    for (int i = 0; i < 256; i++) {
        soma += hist[i] * i;
    }

    double media = soma / total_pixels;

    // Classificação Intensidade
    const char* intensidade;
    if (media < 85)
        intensidade = "Escura";
    else if (media < 170)
        intensidade = "Média";
    else
        intensidade = "Clara";

    printf("Média de intensidade: %.2f (%s)\n", media, intensidade);

    // Desvio Padrão 
    double variancia = 0;
    for(int i = 0; i < 256; i++){
        variancia += hist[i] * ((i - media) * (i - media));
    }
    variancia /= total_pixels;
    double desvio = sqrt(variancia);

    // Classificação Contraste
    const char* contraste;
    if(desvio < 85)
        contraste = "Baixo";
    else if(desvio < 170)
        contraste = "Médio";
    else
        contraste = "Alto";

    printf("Desvio padrão: %.2f (%s)\n", desvio, contraste);

    // ===== JANELAS =====
    
    //Carregando fonte a ser usada
    TTF_Font *fonte = TTF_OpenFont("04B_03__.ttf", 16);
    if (!fonte) {
        fprintf(stderr, "Erro ao carregar fonte: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Janela da imagem
    SDL_Window *janela_img = SDL_CreateWindow("Imagem", imagem->w, imagem->h, 0);
    SDL_Renderer *renderer_img = SDL_CreateRenderer(janela_img, NULL);
    SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer_img, imagem);

    // Janela do histograma
    int largura_hist = 512;
    int altura_hist = 300;

    char info_texto[200];
    snprintf(
            info_texto,
            sizeof(info_texto),
            "Claridade: %s | Contraste: %s",
            intensidade,
            contraste
        );

    SDL_Color text_color = {255, 255, 255};
    
    //Definindo a superfície da janela 
    SDL_Surface *surface_text =  TTF_RenderText_Blended(fonte, info_texto, 0, text_color);
    if (!surface_texto) {
        fprintf(stderr, "Erro ao renderizar texto: %s\n", SDL_GetError());
    }

    SDL_Window *janela_hist = SDL_CreateWindow("Histograma", largura_hist, altura_hist, 0);
    SDL_Renderer *renderer_hist = SDL_CreateRenderer(janela_hist, NULL);

    // ===== LOOP =====
    bool rodando = true;
    SDL_Event e;

    // Botão
    SDL_Rect botao = {10, altura_hist - 40, 120, 30} // posição e tamanho
    bool imagem_normalizada = false; // versão original ou normalizada
    bool mouse_em_cima = false; // seta
    bool mouse_pressionado = false; // clique

    while (rodando) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                rodando = false;
            }

            // seta do mouse
            if(e.type == SDL_EVENT_MOUSE_MOTION){
                int mouse_x = e.button.x;
                int mouse_y = e.button.y;

                // Verifica se seta em cima do botão
                mouse_em_cima = (mouse_x >= botao.x && mouse_x <= botao.x + botao.w && //seta entre borda esquerda e direita do botão
                                mouse_y >= botao.y && mouse_y <= botao.y + botao.h); //seta entre borda superior e inferior do botão
            }

            // mouse down (pressionado, antes de soltar)
            if(e.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                int mouse_x = e.button.x;
                int mouse_y = e.button.y;

                if(mouse_em_cima)
                    mouse_pressionado = true;
            }

            //mouse up (momento que é solto após pressionado)
            if(e.type == SDL_EVENT_MOUSE_BUTTON_UP){
                if(mouse_pressionado && mouse_em_cima){
                    imagem_normalizada = !imagem_normalizada; //alterna para outra imagem

                    if(imagem_normalizada)
                        normalizar_histograma(imagem, hist, total_pixels);

                    else
                        restaurar_imagem_original(imagem_original, imagem);

                    analisar_histograma(imagem, total_pixels);
                }
                mouse_pressionado = false;
            }
        }

        // Render imagem
        SDL_RenderClear(renderer_img);
        SDL_RenderTexture(renderer_img, textura, NULL, NULL);
        SDL_RenderPresent(renderer_img);

        // Render histograma
        SDL_SetRenderDrawColor(renderer_hist, 0, 0, 0, 255);
        SDL_RenderClear(renderer_hist);

        SDL_SetRenderDrawColor(renderer_hist, 255, 255, 255, 255);

        int largura_barra = largura_hist / 256;

        for (int i = 0; i < 256; i++) {
            int h = (hist[i] * altura_hist) / max;

            SDL_RenderLine(
                renderer_hist,
                i * largura_barra,
                altura_hist,
                i * largura_barra,
                altura_hist - h
            );
        }

        // Desenhar botão
        if(mouse_pressionado)
            SDL_SetRenderDrawColor(renderer_hist, 0, 0, 139, 255); //azul escuro
        else if(mouse_em_cima)
            SDL_SetRenderDrawColor(renderer_hist, 173, 216, 230, 255); //azul claro
        else
            SDL_SetRenderDrawColor(renderer_hist, 0, 0, 255, 255); //azul médio

        // Renderiza botão
        SDL_RenderFillRect(renderer_hist, &botao);

        // Texto do botão
        char texto_botao[32];
        if(!imagem_normalizada)
            strcpy(texto_botao, "Normalizar");
        else
            strcpy(texto_botao, "Ver original");

        SDL_Color cor_texto = {255, 255, 255, 255}
        //SDL_Surface* text_surface = //precisa de sdl_tff
        SDL_Surface* text_texture = SDL_CreateTextureFromSurface(renderer_hist, text_surface);

        SDL_Rect text_rect = {botao.x + 10, botao.y + 5, text_surface->w, text_surface->h};
        SDL_RenderCopy(renderer_hist, text_texture, NULL, &text_rect);

        SDL_DestroySurface(text_surface);
        SDL_DestroyTexture(text_texture);

        SDL_RenderPresent(renderer_hist);
        
        // Salvar programa ao pressionar "S"
        if(e.type == SDL_EVENT_KEY_DOWN){
            if(e.key.key == SDLK_S){
                printf("\nSalvando Imagem\n");
                
                if(IMG_SavePNG(ima)em, "output_image.png")

printf()                 "Imagem salva com sucesso!\n" ;  
                 else {}
                    printf(stderr, "Erro ao salvar a imagem: %s\n", SDDL_GetErro)
                {}
            }
        }
    }

    // ===== LIMPEZA =====
    SDL_DestroyTexture(textura);
    SDL_DestroySurface(imagem);

    SDL_DestroyRenderer(renderer_img);
    SDL_DestroyWindow(janela_img);

    SDL_DestroyRenderer(renderer_hist);
    SDL_DestroyWindow(janela_hist);

    SDL_Quit();
    return 0;
}
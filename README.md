# Comp-Visual-Proj

## Integrantes:
- Arthur Ryuiti Sato Furukawa - 10409106
- Guilherme Garcia Lima - 10409637
- Maria Eduarda Pinheiro Leal da Cunha - 10416676
- Rodrigo Nascimento Tomaz - 10418449

## Explicação do projeto
Este projeto tem como objetivo ealizar o processamento de imagens digitais utilizando a biblioteca SDL3

Funcionalidades implementadas:

- Leitura de imagens (PNG, JPG, BMP)
- Conversão automática para escala de cinza
- Geração de histograma
- Cálculo de Média de intensidade
- Cálculo de Contraste (desvio padrão)
- Exibição gráfica da imagem e do histograma
- Equalização do histograma via botão interativo
- Alternância entre imagem original e equalizada
- Interface com botão interativo (hover, clique e mudança de estado)
- Opção de salvar a imagem processada

## Como compilar

No terminal do powershell, rode o seguinte comando para mudar para MYSY2:

`& "C:\msys64\usr\bin\env.exe" MSYSTEM=UCRT64 CHERE_INVOKING=1 /usr/bin/bash -l`

Dentro do terminal MINGW64:

1. Garantir que o GCC está instalado:

`gcc --version`

2. Garantir que as bibliotecas estão instaladas:

`pkg-config --list-all | grep sdl`

3. Compilar

`gcc proc-de-img.c -o programa.exe $(pkg-config --cflags --libs sdl3 sdl3-image sdl3-ttf) -lm`

## Como executar

1. Colocar a imagem (jpg, png, bmp) que será processada, na mesma pasta do executável

2. No terminal UCRT64, rodar o comando :

`./proc-de-img.exe [sua-imagem.extensão]`

3. Aparecerão duas janelas, uma conterá a sua imagem em escala de cinza, e a outra o histograma da imagem.

4. Na janela do histograma, há um botão que, ao ser clicado, realiza a equalização da imagem. O resultado é exibido na janela de processamento em escala de cinza. Ao clicar novamente, a imagem retorna à sua forma original em escala de cinza.

5. Ao pressionar a tecla 'S', será salva a imagem na forma processada com o nome 'output_image.png', na mesma pasta que se encontra o executável, caso já exista um arquivo com o nome 'output_image.png, o mesmo será sobreescrito.

6. Para encerrar o programa, basta clicar o 'x' em uma das janelas.

## Divisão de tarefas

O desenvolvimento foi realizado de forma colaborativa utilizando o recurso Live Share do VS Code, permitindo programação simultânea e testes em tempo real. A divisão foi feita da seguinte forma:

**Arthur Ryuiti Sato Furukawa**

- Função para carregamento da imagem
- Conversão para escala de Cinza
- Função para geração do arquivo output_image.png

**Guilherme Garcia Lima**

- Configuração do ambiente (MSYS2 e integração das bibliotecas SDL3, SDL3_image e SDL3_ttf)
- Execução de testes funcionais com múltiplos inputs de imagem (PNG, JPG e BMP)

**Maria Eduarda Pinheiro Leal da Cunha**

- Cálculo do histograma
- Renderização gráfica (janela da imagem e histograma)

**Rodrigo Nascimento Tomaz**

- Função de equalização do histograma
- Cálculo de média e desvio, assim como classificação de intensidade e contraste
- Implementação da Interface interativa (botão, eventos)

## Tecnologias Utilizadas
 
- Linguagem C
- SDL3
- SDL3_image
- SDL3_ttf
- MSYS2 / MinGW
- Visual Studio Code (Live Share)
- Ambiente Windows

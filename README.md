# Comp-Visual-Proj

## Integrantes:
- Arthur Ryuiti Sato Furukawa - 10409106
- Guilherme Garcia Lima - 10409637
- Maria Eduarda Pinheiro Leal da Cunha - 10416676
- Rodrigo Nascimento Tomaz - 10418449

## Explicação do projeto

Instalação da biblioteca SDL3

1. Instalar MSYS2

`https://www.msys2.org`


2. Baixar SDL no terminal de MSYS2

`pacman -Syu`

`pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-sdl3 mingw-w64-x86_64-sdl3-image pkgconf`

## Como compilar

No terminal do powershell, rode o seguinte comando para mudar para MYSY2:

`& "C:\Program Files\msys64\usr\bin\env.exe" MSYSTEM=MINGW64 CHERE_INVOKING=1 /usr/bin/bash -l`

Dentro do terminal MINGW64:

1. Garantir que o GCC está instalado:

`gcc --version`

2. Garantir que as bibliotecas estão instaladas:

`pkg-config --list-all | grep sdl`

3. Compilar

`gcc proc-de-img.c -o programa $(pkg-config --cflags --libs sdl3 sdl3-image)`

## Como executar

1. Colocar a imagem 

## Divisão de tarefas


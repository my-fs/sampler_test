#pragma once

#include "SDL2/SDL.h"

struct Display
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    void init(std::string title, int winWidth, int winHeight, bool isRetina)
    {
        if (isRetina)
        {
            window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
        }
        else
        {
            window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_SHOWN);
        }

        if (window == NULL)
        {
            std::cout << "Window failed to init err " << SDL_GetError() << std::endl;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        if (isRetina)
        {
            SDL_RenderSetScale(renderer, 2, 2); // retina is 2x scale
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            printf("Couldn't initialize SDL: \n%s\n", SDL_GetError());
        }

    }
};
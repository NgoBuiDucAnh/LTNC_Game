// menu.h
#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_image.h>

struct Menu {
    SDL_Texture* thumbnailTexture;
    SDL_Texture* howToPlayTexture;
    SDL_Rect playButtonRect;
    SDL_Rect howToPlayButtonRect;
    SDL_Rect backButtonRect;

    bool isStartScreen = true;
    bool isShowingHowToPlay = false;

    void load(SDL_Renderer* renderer);
    void handleEvent(SDL_Event& e, bool& quit, bool& startGame);
    void render(SDL_Renderer* renderer);
    void cleanup();
};

#endif

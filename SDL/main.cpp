#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

bool init(SDL_Window*& window, SDL_Renderer*& renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    window = SDL_CreateWindow("Jump King Clone", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;

    return true;
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (!loadedSurface) return nullptr;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return texture;
}

class Player {
public:
    vector<SDL_Texture*> runTextures;
    SDL_Texture* idleTexture;
    SDL_Texture* currentTexture;
    SDL_Rect rect;
    int speed;
    int frameCount;
    bool isMoving;

    Player() : idleTexture(nullptr), currentTexture(nullptr), speed(5), frameCount(0), isMoving(false) {
        rect = { 380, 495, 50, 50 };
    }

    bool loadTextures(SDL_Renderer* renderer) {
        idleTexture = loadTexture(renderer, "player_idle.png");
        runTextures.push_back(loadTexture(renderer, "player_run_1.png"));
        runTextures.push_back(loadTexture(renderer, "player_run_2.png"));
        runTextures.push_back(loadTexture(renderer, "player_run_3.png"));

        if (!idleTexture || runTextures[0] == nullptr || runTextures[1] == nullptr || runTextures[0] == nullptr) {
            return false;
        }

        currentTexture = idleTexture;
        return true;
    }

    bool checkCollision(SDL_Rect newPos, vector<SDL_Rect>& walls) {
        for (SDL_Rect wall : walls) {
            if (SDL_HasIntersection(&newPos, &wall)) {
                return true;
            }
        }
        return false;
    }

    void handleEvent(SDL_Event& e, vector<SDL_Rect>& walls) {
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            SDL_Rect newPos = rect;
            isMoving = (e.type == SDL_KEYDOWN);

            switch (e.key.keysym.sym) {
                case SDLK_LEFT:  newPos.x -= speed; break;
                case SDLK_RIGHT: newPos.x += speed; break;
            }

            if (!checkCollision(newPos, walls)) {
                rect = newPos;
            }
        }
    }

    void updateAnimation() {
        if (isMoving) {
            frameCount++;
            currentTexture = runTextures[(frameCount / 10) % runTextures.size()];
        } else {
            currentTexture = idleTexture;
        }
    }

    void render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, currentTexture, nullptr, &rect);
    }

    void destroy() {
        SDL_DestroyTexture(idleTexture);
        for (SDL_Texture* tex : runTextures) {
            SDL_DestroyTexture(tex);
        }
    }
};

void close(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* bgTexture) {
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!init(window, renderer)) {
        cout << "Failed to initialize!" << endl;
        return -1;
    }

    SDL_Texture* bgTexture = loadTexture(renderer, "1.png");
    Player player;
    if (!player.loadTextures(renderer)) {
        return -1;
    }

    vector<SDL_Rect> walls = {
        {0, 300, 210, 250},
        {590, 300, 210, 250}
    };

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            player.handleEvent(e, walls);
        }

        player.updateAnimation();
        SDL_RenderClear(renderer);

        SDL_Rect bgRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);
        player.render(renderer);
        SDL_RenderPresent(renderer);
    }

    player.destroy();
    close(window, renderer, bgTexture);
    return 0;
}

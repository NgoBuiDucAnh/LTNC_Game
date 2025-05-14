#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL_mixer.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;

struct Cactus {
    SDL_Rect rect;
    SDL_Texture* texture;
    bool active;
};

struct Bird {
    SDL_Rect rect;
    bool active;
    int frameOffsetY;
    int flyLevel;
};

struct Cloud {
    SDL_Rect rect;
    bool active;
};

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void spawnObstacle(Cactus &cactus, Bird &bird, SDL_Texture* cactusTextures[6], SDL_Texture* birdTextures[2]) {
    if (!cactus.active && !bird.active) {
        int choice = rand() % 5;
        if (choice != 0) {
            int cactusType = rand() % 6;
            cactus.texture = cactusTextures[cactusType];
            switch (cactusType) {
                case 0: cactus.rect = {SCREEN_WIDTH, SCREEN_HEIGHT - 54, 25, 50}; break;
                case 1: cactus.rect = {SCREEN_WIDTH, SCREEN_HEIGHT - 54, 50, 50}; break;
                case 2: cactus.rect = {SCREEN_WIDTH, SCREEN_HEIGHT - 54, 75, 50}; break;
                case 3: cactus.rect = {SCREEN_WIDTH, SCREEN_HEIGHT - 40, 17, 35}; break;
                case 4: cactus.rect = {SCREEN_WIDTH, SCREEN_HEIGHT - 40, 34, 35}; break;
                case 5: cactus.rect = {SCREEN_WIDTH, SCREEN_HEIGHT - 40, 51, 35}; break;
            }
            cactus.active = true;
        } else {
            bird.rect.x = SCREEN_WIDTH + rand() % 200;
            bird.flyLevel = rand() % 3 + 1;
            switch (bird.flyLevel) {
                case 1: bird.rect.y = SCREEN_HEIGHT - 45; break;
                case 2: bird.rect.y = SCREEN_HEIGHT - 65; break;
                case 3: bird.rect.y = SCREEN_HEIGHT - 80; break;
            }
            bird.rect.w = 40;
            bird.rect.h = 30;
            bird.active = true;
        }
    }
}

void spawnCloud(Cloud &cloud, SDL_Texture* cloudTexture) {
    if (!cloud.active) {
        cloud.rect.x = SCREEN_WIDTH + rand() % 200;
        cloud.rect.y = rand() % 100;
        SDL_QueryTexture(cloudTexture, NULL, NULL, &cloud.rect.w, &cloud.rect.h);
        cloud.active = true;
    }
}

void moveClouds(Cloud clouds[], int cloudCount) {
    for (int i = 0; i < cloudCount; i++) {
        if (clouds[i].active) {
            clouds[i].rect.x -= 2;
            if (clouds[i].rect.x + clouds[i].rect.w < 0) {
                clouds[i].active = false;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    srand(static_cast<unsigned int>(time(NULL)));

    SDL_Window* window = SDL_CreateWindow("Dino Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* thumbnailTexture = IMG_LoadTexture(renderer, "assets/Thumbnail.png");
    SDL_Rect playButtonRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 200, 80};
    SDL_Rect howToPlayButtonRect = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 90, 250, 80};
    SDL_Rect optionsButtonRect = {SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 + 90, 250, 80};
    bool isStartScreen = true;

    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "assets/Track.png");
    SDL_Rect bg1 = {0, SCREEN_HEIGHT - 12, 600, 12};
    SDL_Rect bg2 = {600, SCREEN_HEIGHT - 12, 600, 12};
    int bgSpeed = 4;

    Uint32 lastSpeedIncrease = SDL_GetTicks();
    int speedMultiplier = 0;

    SDL_Texture* dinoStart = IMG_LoadTexture(renderer, "assets/DinoStart.png");
    SDL_Texture* dinoRun1 = IMG_LoadTexture(renderer, "assets/DinoRun1.png");
    SDL_Texture* dinoRun2 = IMG_LoadTexture(renderer, "assets/DinoRun2.png");
    SDL_Texture* dinoJump = IMG_LoadTexture(renderer, "assets/DinoJump.png");
    SDL_Texture* dinoDuck1 = IMG_LoadTexture(renderer, "assets/DinoDuck1.png");
    SDL_Texture* dinoDuck2 = IMG_LoadTexture(renderer, "assets/DinoDuck2.png");
    SDL_Texture* dinoDead = IMG_LoadTexture(renderer, "assets/DinoDead.png");

    SDL_Texture* tickTexture = IMG_LoadTexture(renderer, "tick.png");
    SDL_Texture* gameOverTexture = IMG_LoadTexture(renderer, "assets/GameOver.png");
    SDL_Texture* resetTexture = IMG_LoadTexture(renderer, "assets/Reset.png");
    SDL_Texture* pausedTexture = IMG_LoadTexture(renderer, "assets/GamePaused.png");
    SDL_Texture* howToPlayTexture = IMG_LoadTexture(renderer, "assets/HowToPlay.png");
    SDL_Texture* optionsScreenTexture = IMG_LoadTexture(renderer, "assets/Options.png");
    SDL_Rect soundToggleRect = {SCREEN_WIDTH / 2 + 70, SCREEN_HEIGHT / 2 - 60, 80, 90};
    SDL_Rect backgroundToggleRect = {SCREEN_WIDTH / 2 + 70, SCREEN_HEIGHT / 2 + 60, 30, 30};


    SDL_Rect resetRectPaused = {SCREEN_WIDTH / 2 - 130, SCREEN_HEIGHT / 2 + 40, 200, 100};
    SDL_Rect resetRectGameOver = {SCREEN_WIDTH / 2 - 32, SCREEN_HEIGHT / 2 + 40, 64, 64};

    SDL_Rect mainMenuRectPaused = {SCREEN_WIDTH / 2 + 80, SCREEN_HEIGHT / 2 + 40, 200, 100};

    SDL_Texture* cloudTexture = IMG_LoadTexture(renderer, "assets/Cloud.png");
    const int cloudCount = 5;
    Cloud clouds[cloudCount] = {};

    SDL_Texture* numberTextures[10] = {
        IMG_LoadTexture(renderer, "assets/0.png"), IMG_LoadTexture(renderer, "assets/1.png"),
        IMG_LoadTexture(renderer, "assets/2.png"), IMG_LoadTexture(renderer, "assets/3.png"),
        IMG_LoadTexture(renderer, "assets/4.png"), IMG_LoadTexture(renderer, "assets/5.png"),
        IMG_LoadTexture(renderer, "assets/6.png"), IMG_LoadTexture(renderer, "assets/7.png"),
        IMG_LoadTexture(renderer, "assets/8.png"), IMG_LoadTexture(renderer, "assets/9.png")
    };

    SDL_Texture* cactusTextures[6] = {
        IMG_LoadTexture(renderer, "assets/LargeCactus1.png"),
        IMG_LoadTexture(renderer, "assets/LargeCactus2.png"),
        IMG_LoadTexture(renderer, "assets/LargeCactus3.png"),
        IMG_LoadTexture(renderer, "assets/SmallCactus1.png"),
        IMG_LoadTexture(renderer, "assets/SmallCactus2.png"),
        IMG_LoadTexture(renderer, "assets/SmallCactus3.png")
    };

    SDL_Texture* birdTextures[2] = {
        IMG_LoadTexture(renderer, "assets/Bird1.png"),
        IMG_LoadTexture(renderer, "assets/Bird2.png")
    };

    Mix_Chunk* jumpSound = Mix_LoadWAV("assets/jump.wav");
    Mix_Chunk* pointSound = Mix_LoadWAV("assets/Points.wav");
    Mix_Chunk* deadSound = Mix_LoadWAV("assets/dead.wav");
    Mix_Music* backgroundMusic = Mix_LoadMUS("assets/SoundBackGround.wav");

    Mix_PlayMusic(backgroundMusic, -1);

    Cactus cactus = {{SCREEN_WIDTH, SCREEN_HEIGHT - 60, 20, 40}, nullptr, false};
    Bird bird = {{SCREEN_WIDTH, SCREEN_HEIGHT - 80, 40, 30}, false, 0};

    SDL_Texture* currentDino = dinoStart;
    bool isDucking = false;
    bool isGameOver = false;
    bool isPaused = false;
    bool hasDucked = false;
    bool isSoundOn = true;

    SDL_Rect dinoRect = {100, SCREEN_HEIGHT - 46, 44, 47};
    float groundY = SCREEN_HEIGHT - 46;
    float jumpVelocity = 0;
    float gravity = 0.8f;
    bool isJumping = false;
    bool isSpaceHeld = false;
    int frameCount = 0;

    bool isHowToPlayScreen = false;
    bool isOptionsScreen = false;
    bool isBackgroundWhite = false;

    int score = 0;
    Uint32 lastScoreTime = SDL_GetTicks();

    bool quit = false;
    SDL_Event e;

    int lastMilestone = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (isStartScreen) {
                if (e.type == SDL_QUIT) quit = true;
                else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    int x = e.button.x, y = e.button.y;

                    if (isOptionsScreen && x >= soundToggleRect.x && x <= soundToggleRect.x + soundToggleRect.w && y >= soundToggleRect.y && y <= soundToggleRect.y + soundToggleRect.h) {
                        if (isOptionsScreen) {
                            if (x >= soundToggleRect.x && x <= soundToggleRect.x + soundToggleRect.w &&
                                y >= soundToggleRect.y && y <= soundToggleRect.y + soundToggleRect.h) {
                                isSoundOn = !isSoundOn;
                                if (isSoundOn) Mix_ResumeMusic();
                                else Mix_PauseMusic();
                                continue;
                            }

                            if (x >= backgroundToggleRect.x && x <= backgroundToggleRect.x + backgroundToggleRect.w &&
                                y >= backgroundToggleRect.y && y <= backgroundToggleRect.y + backgroundToggleRect.h) {
                                isBackgroundWhite = !isBackgroundWhite;
                                continue;
                            }
                        }
                        isSoundOn = !isSoundOn;
                        if (isSoundOn) {
                            Mix_ResumeMusic();
                        } else {
                            Mix_PauseMusic();
                        }
                        continue;
                    }

                    if (isStartScreen) {
                        if (x >= playButtonRect.x && x <= playButtonRect.x + playButtonRect.w && y >= playButtonRect.y && y <= playButtonRect.y + playButtonRect.h) {
                            isStartScreen = false;
                            lastScoreTime = SDL_GetTicks();
                            lastSpeedIncrease = SDL_GetTicks();
                            speedMultiplier = 0;
                        } else if (x >= howToPlayButtonRect.x && x <= howToPlayButtonRect.x + howToPlayButtonRect.w &&
                                   y >= howToPlayButtonRect.y && y <= howToPlayButtonRect.y + howToPlayButtonRect.h) {
                            isStartScreen = false;
                            isHowToPlayScreen = true;
                        } else if (x >= optionsButtonRect.x && x <= optionsButtonRect.x + optionsButtonRect.w &&
                                   y >= optionsButtonRect.y && y <= optionsButtonRect.y + optionsButtonRect.h) {
                            isStartScreen = false;
                            isOptionsScreen = true;
                        }
                        continue;
                    }
                }
                continue;
            }

            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                if (isOptionsScreen) {
                    isOptionsScreen = false;
                    isStartScreen = true;
                }
                else if (isHowToPlayScreen) {
                    isHowToPlayScreen = false;
                    isStartScreen = true;
                }
                else if (!isGameOver && !isStartScreen) {
                    isPaused = !isPaused;
                }
                else if (isGameOver) {
                    quit = true;
                }
            } else if (!isGameOver && !isPaused) {
                if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_UP)) {
                    isSpaceHeld = true;
                    if (!isJumping) {
                        isJumping = true;
                        jumpVelocity = -7.0f;
                        if (isSoundOn) Mix_PlayChannel(-1, jumpSound, 0);
                    }
                } else if (e.type == SDL_KEYUP && (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_UP)) {
                    isSpaceHeld = false;
                } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
                    isDucking = true;
                    if (!hasDucked) {
                        if (isSoundOn) Mix_PlayChannel(-1, jumpSound, 0);
                        hasDucked = true;
                    }
                } else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_DOWN) {
                    isDucking = false;
                    hasDucked = false;
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && (isGameOver || isPaused)) {
                int x = e.button.x, y = e.button.y;
                SDL_Rect activeResetRect = isPaused ? resetRectPaused : resetRectGameOver;
                if (x >= activeResetRect.x && x <= activeResetRect.x + activeResetRect.w && y >= activeResetRect.y && y <= activeResetRect.y + activeResetRect.h) {
                    isGameOver = false;
                    isPaused = false;
                    cactus.active = false;
                    bird.active = false;
                    dinoRect = {100, SCREEN_HEIGHT - 46, 44, 47};
                    isJumping = false;
                    isDucking = false;
                    jumpVelocity = 0;
                    currentDino = dinoStart;
                    score = 0;
                    lastScoreTime = SDL_GetTicks();
                    lastSpeedIncrease = SDL_GetTicks();
                    speedMultiplier = 0;
                } else if (isPaused && x >= mainMenuRectPaused.x && x <= mainMenuRectPaused.x + mainMenuRectPaused.w && y >= mainMenuRectPaused.y && y <= mainMenuRectPaused.y + mainMenuRectPaused.h) {
                    isPaused = false;
                    isGameOver = false;
                    isStartScreen = true;
                }
                SDL_Rect resumeRect = {SCREEN_WIDTH / 2 - 330, SCREEN_HEIGHT / 2 + 40, 200, 100};
                if (isPaused &&
                    x >= resumeRect.x && x <= resumeRect.x + resumeRect.w &&
                    y >= resumeRect.y && y <= resumeRect.y + resumeRect.h) {
                    isPaused = false;
                }
            }
        }

        if (isBackgroundWhite) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Trắng
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Đen
        }
        SDL_RenderClear(renderer);

        if (isStartScreen) {
            SDL_RenderCopy(renderer, thumbnailTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            continue;
        }

        if (isHowToPlayScreen) {
            SDL_RenderCopy(renderer, howToPlayTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            continue;
        }

        if (isOptionsScreen) {
            SDL_RenderCopy(renderer, optionsScreenTexture, NULL, NULL);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &backgroundToggleRect);

            if (isBackgroundWhite && tickTexture) {
                SDL_Rect tickRect = {backgroundToggleRect.x + 2, backgroundToggleRect.y + 2,
                                     backgroundToggleRect.w - 4, backgroundToggleRect.h - 4};
                SDL_RenderCopy(renderer, tickTexture, NULL, &tickRect);
            }
            if (isBackgroundWhite) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderDrawLine(renderer, backgroundToggleRect.x + 5, backgroundToggleRect.y + 15,
                                             backgroundToggleRect.x + 13, backgroundToggleRect.y + 23);
                SDL_RenderDrawLine(renderer, backgroundToggleRect.x + 13, backgroundToggleRect.y + 23,
                                             backgroundToggleRect.x + 25, backgroundToggleRect.y + 5);
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            continue;
        }


        if (!isPaused && !isGameOver) {
            if (SDL_GetTicks() - lastScoreTime >= 100) {
                score++;
                if ((score / 200) % 2 == 1)
                    isBackgroundWhite = true;
                else
                    isBackgroundWhite = false;
                lastScoreTime = SDL_GetTicks();
                if (score % 100 == 0 && score != 0 && score != lastMilestone) {
                    Mix_PlayChannel(-1, pointSound, 0);
                    lastMilestone = score;
                }
            }

            if (SDL_GetTicks() - lastSpeedIncrease >= 3000) {
                lastSpeedIncrease = SDL_GetTicks();
                speedMultiplier++;
            }

            if (isJumping) {
                dinoRect.y += jumpVelocity;
                jumpVelocity += isSpaceHeld ? gravity * 0.4f : gravity;
                if (dinoRect.y >= groundY) {
                    dinoRect.y = groundY;
                    isJumping = false;
                }
            }

            if (isDucking) {
                frameCount++;
                currentDino = (frameCount / 10 % 2 == 0) ? dinoDuck1 : dinoDuck2;
                dinoRect.h = 30;
                dinoRect.w = 56;
                dinoRect.y = SCREEN_HEIGHT - 36;
            } else if (isJumping) {
                currentDino = dinoJump;
            } else {
                frameCount++;
                currentDino = (frameCount / 10 % 2 == 0) ? dinoRun1 : dinoRun2;
                dinoRect.h = 47;
                dinoRect.w = 44;
                dinoRect.y = SCREEN_HEIGHT - 46;
            }

            int effectiveSpeed = 5 + speedMultiplier;
            bg1.x -= (bgSpeed + speedMultiplier);
            bg2.x -= (bgSpeed + speedMultiplier);
            if (bg1.x + bg1.w < 0) bg1.x = bg2.x + bg2.w;
            if (bg2.x + bg2.w < 0) bg2.x = bg1.x + bg1.w;

            spawnObstacle(cactus, bird, cactusTextures, birdTextures);

            if (cactus.active) {
                cactus.rect.x -= effectiveSpeed;
                if (cactus.rect.x + cactus.rect.w < 0) cactus.active = false;
            }
            if (bird.active) {
                bird.rect.x -= effectiveSpeed;
                if (bird.rect.x + bird.rect.w < 0) bird.active = false;
            }

            SDL_Rect dinoCollisionBox = {dinoRect.x + 5, dinoRect.y + 5, dinoRect.w - 10, dinoRect.h - 10};
            if (dinoRect.y > SCREEN_HEIGHT - 120) {
                if (checkCollision(dinoCollisionBox, cactus.rect) || checkCollision(dinoCollisionBox, bird.rect)) {
                    isGameOver = true;
                    currentDino = dinoDead;
                    Mix_PlayChannel(-1, deadSound, 0);
                }
            }

            for (int i = 0; i < cloudCount; i++) spawnCloud(clouds[i], cloudTexture);
            moveClouds(clouds, cloudCount);
        }

        SDL_RenderCopy(renderer, bgTexture, NULL, &bg1);
        SDL_RenderCopy(renderer, bgTexture, NULL, &bg2);
        SDL_RenderCopy(renderer, currentDino, NULL, &dinoRect);

        for (int i = 0; i < cloudCount; i++)
            if (clouds[i].active) SDL_RenderCopy(renderer, cloudTexture, NULL, &clouds[i].rect);

        if (bird.active) SDL_RenderCopy(renderer, birdTextures[frameCount / 10 % 2], NULL, &bird.rect);
        if (cactus.active) SDL_RenderCopy(renderer, cactus.texture, NULL, &cactus.rect);

        int digits[5];
        int tempScore = score;
        for (int i = 4; i >= 0; --i) {
            digits[i] = tempScore % 10;
            tempScore /= 10;
        }
        for (int i = 0; i < 5; ++i) {
            SDL_Rect digitRect = {SCREEN_WIDTH - (5 - i) * 20 - 10, 10, 20, 30};
            SDL_RenderCopy(renderer, numberTextures[digits[i]], NULL, &digitRect);
        }

        if (isPaused) {
            SDL_Rect pausedRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, pausedTexture, NULL, &pausedRect);
            SDL_Rect resumeRect = {SCREEN_WIDTH / 2 - 350, SCREEN_HEIGHT / 2 + 65, 200, 100};
        }

        if (isGameOver) {
            SDL_Rect gameOverRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 30, 200, 60};
            SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
            SDL_RenderCopy(renderer, resetTexture, NULL, &resetRectGameOver);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    Mix_FreeChunk(jumpSound);
    Mix_CloseAudio();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    Mix_FreeMusic(backgroundMusic);
    Mix_HaltMusic();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

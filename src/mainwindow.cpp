#include "mainwindow.h"
#include "exception.h"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <algorithm>

MainWindow* mainwindow;

MainWindow::MainWindow(int w, int h, const std::string& title)
    : width{w}, height{h}, gWindow{nullptr}, gRenderer{nullptr}
{
    int res = SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (res < 0) {throw SDLException(std::string("SDL could not initialize! SDL Error: ") + SDL_GetError());}

    res = SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    if (!res) {std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;}

    gWindow = SDL_CreateWindow( title.c_str(),
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                width,
                                height,
                                SDL_WINDOW_SHOWN );

    if (gWindow == nullptr)
        throw SDLException(std::string("Could not create Window! SDL Error: ") + SDL_GetError());

    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if (gRenderer == nullptr)
        throw SDLException(std::string("Renderer could not be created! SDL Error: ") + SDL_GetError());

    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

    res = IMG_INIT_PNG;
    if( !( IMG_Init( res ) & res ) )
        throw SDLException(std::string("SDL_image could not initialize! SDL_image Error: ") + IMG_GetError());

    res = TTF_Init();
    if (res == -1)
        throw SDLException(std::string("SDL_ttf could not initialize! SDL_ttf Error: ") + TTF_GetError());

    res = Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
    if (res < 0)
        throw SDLException(std::string("SDL_mixer could not initialize! SDL_mixer Error: ") + Mix_GetError());

    mainwindow = this;
}

MainWindow::~MainWindow() noexcept
{
    SDL_DestroyRenderer(gRenderer);

    SDL_DestroyWindow(gWindow);

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    mainwindow = nullptr;
}


void MainWindow::setIcon(const std::string& path)
{
    SDL_Surface* icon = IMG_Load(path.c_str());
    if (icon == nullptr)
        throw SDLException(std::string("Could not load Icon! SDL_image Error: ") + IMG_GetError());

    SDL_SetWindowIcon(gWindow, icon);
    SDL_FreeSurface(icon); // icon is no longer required
}

void MainWindow::clear()
{
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( gRenderer );
}

void MainWindow::update()
{
    SDL_RenderPresent( gRenderer );
}

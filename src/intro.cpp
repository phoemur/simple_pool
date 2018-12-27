#include "intro.h"

Intro::Intro()
    : background{}, title{}, subtitle{}, f1{}, f2{}
{
    f1.loadFromFile("./pool_assets/Purisa-BoldOblique.ttf", 30);
    f2.loadFromFile("./pool_assets/Purisa-BoldOblique.ttf", 16);
    title.loadFromRenderedText("Wellcome to Simple Pool", f1, SDL_Color{0xFF,0,0,0xFF});
    subtitle.loadFromRenderedText("Press any key to continue...", f2, SDL_Color{0xFF,0xFF,0xFF,0xFF});

    background.loadFromFile("./pool_assets/background.png");
}

void Intro::handle_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
            GameState::next_state = GameStates::Quit;
        else if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONUP)
            GameState::next_state = GameStates::Level;
    }
}

void Intro::logic() {}

void Intro::render()
{
    mainwindow->clear();
    background.render(0, 0);
    title.render(static_cast<int>((mainwindow->getWidth() - title.getWidth()) / 2),
                 static_cast<int>((mainwindow->getHeight() - title.getHeight()) / 2));
    subtitle.render(static_cast<int>((mainwindow->getWidth() - subtitle.getWidth()) / 2),
                    static_cast<int>((mainwindow->getHeight() + title.getHeight() - subtitle.getHeight()) / 2 ));
    mainwindow->update();
}

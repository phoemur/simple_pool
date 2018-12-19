#ifndef INTRO_H
#define INTRO_H

#include "gamestate.h"
#include "mainwindow.h"
#include "texture.h"
#include "font.h"

#include <SDL.h>

class Intro : public GameState {
    Texture background;
    Texture title;
    Texture subtitle;
    Font f1, f2;
public:
    Intro();
    Intro(const Intro&) = delete;
    Intro& operator=(const Intro&) = delete;

    virtual void handle_events() override;
    virtual void logic() override;
    virtual void render() override;
};

#endif // INTRO_H

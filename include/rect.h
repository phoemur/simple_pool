#ifndef RECT_H
#define RECT_H

#include "subject.h"

#include <SDL.h>

class Rect : public Subject {
    SDL_Color border, inside;
    bool solid;


public:
    MovementData movData {0.0, 0.0, 0.0, 0.0};
    SDL_Rect rect;
    bool visible = true;

    Rect();
    Rect(int x, int y, int w, int h);
    Rect(int x, int y, int w, int h, const SDL_Color& b);
    Rect(int x, int y, int w, int h, const SDL_Color& b, const SDL_Color& i);
    explicit Rect(const SDL_Rect& r);
    Rect(const SDL_Rect& r, const SDL_Color& b);
    Rect(const SDL_Rect& r, const SDL_Color& b, const SDL_Color& i);

    Rect(const Rect&) = default;
    Rect& operator=(const Rect&) = default;
    Rect(Rect&&) = default;
    Rect& operator=(Rect&&) = default;

    virtual void render() override;
    virtual void move() override;

    void setShape(int x, int y, int w, int h);
    void setSpeed(double spx, double spy, double acx = 0.0, double acy = 0.0) noexcept;
    void setBorder(const SDL_Color& c) {border = c;}
    void setFill(const SDL_Color& c) {inside = c; solid=true;}
    void setHollow() {solid = false;}
};

#endif // RECT_H

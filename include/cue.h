#ifndef CUE_H
#define CUE_H

#include "texture.h"

class Cue {
    Texture t;
    double posX, posY;
    double degrees;
    SDL_Point center;

public:
    Cue();

    void render();

    void setPos(double x, double y);
    void setDeg(double angle);

    double getHeight() const noexcept {return t.getHeight();}
    double  getWidth() const noexcept {return t.getWidth();}
    double  getAngle() const noexcept {return degrees;}
};

#endif // CUE_H

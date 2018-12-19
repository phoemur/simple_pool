#ifndef TABLE_H
#define TABLE_H

#include "texture.h"
#include "rect.h"
#include "ball.h"

class CollisionObserver; // Forward declare

class Table {
    int posX, posY;
    Texture background;
    Rect r[6] {};  // Rect colliders for walls
    Ball b[12] {}; // Ball colliders to simulate pocket sides

public:
    Table(CollisionObserver* c = nullptr);

    void render();

    double  getWidth() const noexcept {return background.getWidth(); }
    double getHeight() const noexcept {return background.getHeight();}
    int         getX() const noexcept {return posX;}
    int         getY() const noexcept {return posY;}
};

#endif // TABLE_H

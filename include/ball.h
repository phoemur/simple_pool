#ifndef BALL_H
#define BALL_H

#include "subject.h"
#include "texture.h"

struct Circle {
    double pos_x;
    double pos_y;
    double radius;
};

class Ball : public Subject  {
    Texture texture;
public:
    bool is_movable = true;
    bool is_visible = true;
    Circle posData;
    MovementData movData;
    double mass = 10.0;
    int id = 0; // the number of the ball

    Ball();
    Ball(const std::string& path);

    void setTex(const std::string& path);
    void setPos(double px, double py);
    void setVel(double sx, double sy);

    virtual void   move() override;
    virtual void render() override;

    bool is_moving() const noexcept;

    static bool   is_solid(int num) {return num > 0 && num < 8;}
    static bool is_stripes(int num) {return num > 8 && num < 16;}
};

#endif // BALL_H

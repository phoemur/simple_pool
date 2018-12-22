#ifndef LEVEL_H
#define LEVEL_H

#include "gamestate.h"
#include "texture.h"
#include "font.h"
#include "table.h"
#include "collisionobserver.h"
#include "audioobserver.h"
#include "ball.h"
#include "cue.h"

#include <vector>
#include <utility>

extern const double PI;

class Level : public GameState {
    Texture background;
    Texture p1, p2;
    Font f1;
    Texture b8, b1, b9;

    CollisionObserver collobserver;
    AudioObserver audio;
    Table tab;
    Ball balls[15] {};
    Ball cueball {};
    Cue cue {};

    std::vector<int> pockets {};
    bool moving_state = false;
    bool player1turn = true;
    bool move_was_made = false;
    int team_color = 0; // 0 = undefined, 1 = player1 kills solid, 2 = player1 kills stripes
    bool mouse_pressed = false;

public:
    Level();

    virtual void handle_events() override;
    virtual void logic() override;
    virtual void render() override;

private:
    void handle_when_still(SDL_Event& e);
    void create_balls();
    void create_cue_ball();
    void check_pocket(Ball& b);
    bool ball_off_table(Ball& b);
    std::pair<double,double> get_safe_pos();
    void render_head();
    void recenter_cue();
    void change_state();
    void check_first_hit(bool cur_turn);
    void check_balls_off_table(bool cur_turn);
    void check_team_color(bool cur_turn);
    void check_balls_in_pockets(bool cur_turn);
    void won(bool cur_turn);
    void lost(bool cur_turn);
    void message(const std::string& msg, unsigned delay);
    void shoot(double speed);
};

#endif // LEVEL_H

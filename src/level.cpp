#include "level.h"
#include "mainwindow.h"

#include <algorithm>
#include <cmath>
#include <string>

#include <SDL.h>

Level::Level()
    : background{}, p1{}, p2{}, f1{}, b8{}, b1{}, b9{},
      collobserver{}, audio{}, tab{&collobserver}
{
    background.loadFromFile("./pool_assets/background.png");
    f1.loadFromFile("./pool_assets/Purisa-BoldOblique.ttf", 20);
    p1.loadFromRenderedText("Player 1", f1, SDL_Color{0xFF,0xFF,0xFF,0xFF});
    p2.loadFromRenderedText("Player 2", f1, SDL_Color{0xFF,0xFF,0xFF,0xFF});
    b8.loadFromFile("./pool_assets/ball8.png");
    b1.loadFromFile("./pool_assets/ball1.png");
    b9.loadFromFile("./pool_assets/ball9.png");

    create_balls();
    create_cue_ball();
    create_sensors();
}

void Level::handle_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
            GameState::next_state = GameStates::Quit;
        else if (!moving_state)
            handle_when_still(e);
    }
}

void Level::logic()
{
    moving_state = false;

    for (auto& b: balls)
        if (b.is_moving())
        {
            moving_state = true;
            b.move();
            check_pocket(b);
        }

    if (cueball.is_moving())
    {
        moving_state = true;
        cueball.move();
        check_pocket(cueball);
    }


    if (!moving_state) // Check player moves and state changes
    {
        recenter_cue();

        if (move_was_made)
            change_state();
    }
}

void Level::render()
{
    mainwindow->clear();

    background.render(0, 0);
    tab.render();

    for (auto& b: balls)
        if (b.is_visible)
            b.render();

    if (cueball.is_visible)
        cueball.render();

    if (!moving_state && cueball.is_visible)
        cue.render();

    render_head();

    mainwindow->update();
}

void Level::handle_when_still(SDL_Event& e)
{
    static const double pi = std::atan(1) * 4;

    if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN)
    {
        collobserver.reset_first_hit();

        double speed = 11.0;
        double angle = (cue.getAngle() * pi) / 180.0;

        cueball.movData.speed_y = -1 * std::sin(angle) * speed;
        cueball.movData.speed_x = -1 * std::cos(angle) * speed;

        cueball.notify(Event::SUBJECT_CUE_COLLIDED);
        move_was_made = true;
    }
    else if (e.type == SDL_MOUSEMOTION)
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState( &x, &y );
        auto oposed = cueball.posData.pos_y - y;
        auto hyp = std::hypot(cueball.posData.pos_x - x, cueball.posData.pos_y - y);
        double degrees = (std::asin(oposed / hyp) * 180.0) / pi + 180.0;

        if (x > cueball.posData.pos_x)
            degrees = 180.0 - degrees;

        if (degrees < 0.0)
            degrees += 360.0;

        cue.setDeg(degrees);
    }
}

void Level::create_balls()
{
    // Create 15 balls
    for (int i = 1; i <= 15; ++i)
    {
        balls[i-1].setTex("./pool_assets/ball" + std::to_string(i) + ".png");
        balls[i-1].id = i;
        balls[i-1].addObserver(&collobserver);
        balls[i-1].addObserver(&audio);
    }

    // Place the balls
    int cur = 0;
    double radius = balls[0].posData.radius + 0.5;
    double px = 200.0;
    double py = (tab.getY() + tab.getHeight()/2) - 5*radius;
    for (int i = 5; i > 0; --i)
    {
        for (int j = 0; j < i; ++j)
            balls[cur++].setPos(px, py + radius*j*2);

        px += radius * 2.0 * std::sqrt(3) / 2;
        py += radius;
    }

    // 8-ball must be at the center of the triangle
    std::swap(balls[7].posData, balls[10].posData);

    // Oposite signs at the corners
    std::swap(balls[4].posData, balls[8].posData);
}

void Level::create_cue_ball()
{
    // Create and place the cue ball
    cueball = Ball{};
    cueball.setTex("./pool_assets/ball0.png");
    cueball.id = 0;
    cueball.mass = 13.0;
    cueball.addObserver(&collobserver);
    cueball.addObserver(&audio);

    auto pos = get_safe_pos();
    cueball.setPos(pos.first, pos.second);
}

void Level::create_sensors()
{
    // Place pocket sensors
    pocket_sensors[0].x = tab.getX() + 35; // Top Left
    pocket_sensors[0].y = tab.getY() + 35;
    pocket_sensors[1].x = tab.getX() + 33; // Bottom left
    pocket_sensors[1].y = tab.getY() - 33 + static_cast<int>(tab.getHeight());
    pocket_sensors[2].x = tab.getX() - 35 + static_cast<int>(tab.getWidth());// Top right
    pocket_sensors[2].y = tab.getY() + 35;
    pocket_sensors[3].x = tab.getX() - 35 + static_cast<int>(tab.getWidth()); // Bottom right
    pocket_sensors[3].y = tab.getY() - 35 + static_cast<int>(tab.getHeight());
    pocket_sensors[4].x = tab.getX() + static_cast<int>(tab.getWidth()/2); // Top middle
    pocket_sensors[4].y = tab.getY() + 25;
    pocket_sensors[5].x = tab.getX() + static_cast<int>(tab.getWidth()/2); // Bottom middle
    pocket_sensors[5].y = tab.getY() - 23 + static_cast<int>(tab.getHeight());
}

bool Level::check_pocket(Ball& b)
{
    for (auto& p : pocket_sensors)
    {
        double dist = std::hypot(b.posData.pos_x - p.x, b.posData.pos_y - p.y);

        if (dist <= b.posData.radius) // hit the pocket
        {
            b.notify(Event::SUBJECT_POCKET_COLLIDED);
            b.removeObserver(&collobserver);
            b.is_movable = false;
            b.is_visible = false;

            pockets.push_back(b.id);

            std::cout << b.id << std::endl;

            return true;
        }
    }

    return false;
}

// Check if a ball has fallen off the table
bool Level::ball_off_table(Ball& b)
{
    if (b.posData.pos_x < tab.getX())
        return true;
    else if (b.posData.pos_x > tab.getX() + tab.getWidth())
        return true;
    else if (b.posData.pos_y < tab.getY())
        return true;
    else if (b.posData.pos_y > tab.getY() + tab.getHeight())
        return true;
    else
        return false;
}

// Get a safe position to place a ball that returns to the table
std::pair<double, double> Level::get_safe_pos()
{
    double posX = 600.0;
    double posY = tab.getY() + tab.getHeight()/2 - 2.5;

    bool shifted = true;
    while (shifted)
    {
        shifted = false;

        for (auto& b: balls)
            if (b.is_visible)
                if (std::hypot(b.posData.pos_x - posX, b.posData.pos_y - posY) < b.posData.radius*2)
                {
                    posX -= b.posData.radius * 2.0;
                    shifted = true;
                    break;
                }

        if (std::hypot(cueball.posData.pos_x - posX, cueball.posData.pos_y - posY) < cueball.posData.radius*2)
        {
            posX -= cueball.posData.radius;
            shifted = true;
        }
    }

    return {posX, posY};
}

void Level::render_head()
{
    p1.render(5, 5);
    p2.render(static_cast<int>(background.getWidth() - p2.getWidth()) - 5, 5);

    if (player1turn)
    {
        switch(team_color)
        {
            case 0:
            default:
                b8.render(10 + static_cast<int>(p1.getWidth()), 10);
                break;
            case 1:
                b1.render(10 + static_cast<int>(p1.getWidth()), 10);
                break;
            case 2:
                b9.render(10 + static_cast<int>(p1.getWidth()), 10);
                break;
        }
    }
    else // player 2 turn
    {
        switch(team_color)
        {
            case 0:
            default:
                b8.render(static_cast<int>(background.getWidth() - p2.getWidth()) - 30, 10);
                break;
            case 1:
                b9.render(static_cast<int>(background.getWidth() - p2.getWidth()) - 30, 10);
                break;
            case 2:
                b1.render(static_cast<int>(background.getWidth() - p2.getWidth()) - 30, 10);
                break;
        }
    }
}

void Level::recenter_cue()
{
    cue.setPos(cueball.posData.pos_x + cueball.posData.radius, cueball.posData.pos_y - cue.getHeight()/2);
}

void Level::change_state()
{
    bool cur_turn = player1turn;

    // Check own ball not hit first
    check_first_hit(cur_turn);

    // Balls pocketed
    check_balls_in_pockets(cur_turn);

    // Check balls off the table
    check_balls_off_table(cur_turn);

    move_was_made = false;
}

void Level::check_first_hit(bool cur_turn)
{
    // Check own ball not hit first
    int f = collobserver.get_first_hit();
    if (cur_turn)
    {
        if (team_color == 1 && (Ball::is_stripes(f) || f == 8))
            player1turn = !cur_turn;
        else if (team_color == 2 && (Ball::is_solid(f) || f == 8))
            player1turn = !cur_turn;
    }
    else // Player 2 turn
    {
        if (team_color == 2 && (Ball::is_stripes(f) || f == 8))
            player1turn = !cur_turn;
        else if (team_color == 1 && (Ball::is_solid(f) || f == 8))
            player1turn = !cur_turn;
    }
}

void Level::check_balls_off_table(bool cur_turn)
{
    // Check balls off the table
    for (auto& b : balls)
        if (ball_off_table(b))
        {
            auto pos = get_safe_pos();
            b.posData.pos_x = pos.first;
            b.posData.pos_y = pos.second;
            b.movData.speed_x = 0.0;
            b.movData.speed_y = 0.0;

            player1turn = !cur_turn;
        }

    if (ball_off_table(cueball))
    {
        create_cue_ball();
        player1turn = !cur_turn;
    }
}

void Level::check_team_color(bool cur_turn)
{
    // Check team color
    if (team_color == 0)
    {
        if (std::all_of(pockets.begin(), pockets.end(), &Ball::is_solid))
            team_color = cur_turn ? 1 : 2;
        else if (std::all_of(pockets.begin(), pockets.end(), &Ball::is_stripes))
            team_color = cur_turn ? 2 : 1;
    }
}

void Level::check_balls_in_pockets(bool cur_turn)
{
    // Balls pocketed
    if (pockets.size() == 0)
        player1turn = !cur_turn;
    else
    {
        // Cue ball in the pocket
        if (std::find(pockets.begin(), pockets.end(), 0) != pockets.end())
        {
            create_cue_ball();
            player1turn = !cur_turn;
        }
        else if (cur_turn) // Player 1 turn
        {
            // Own ball not in the pocket
            if (team_color == 1 && std::none_of(pockets.begin(), pockets.end(), &Ball::is_solid))
                player1turn = !cur_turn;
            else if (team_color == 2 && std::none_of(pockets.begin(), pockets.end(), &Ball::is_stripes))
                player1turn = !cur_turn;
            // 8 ball in the pocket
            else if (std::find(pockets.begin(), pockets.end(), 8) != pockets.end())
            {
                if (team_color == 0)
                    lost(cur_turn);
                else if (team_color == 1)
                {
                    if (std::any_of(std::begin(balls),
                                    std::end(balls),
                                    [](auto& b) {
                                        return b.is_visible && b.id > 0 && b.id < 8;
                                    }))
                    {
                        lost(cur_turn);
                    }
                    else
                    {
                        won(cur_turn);
                    }
                }
                else if (team_color == 2)
                {
                    if (std::any_of(std::begin(balls),
                                    std::end(balls),
                                    [](auto& b) {
                                        return b.is_visible && b.id > 8 && b.id < 16;
                                    }))
                    {
                        lost(cur_turn);
                    }
                    else
                    {
                        won(cur_turn);
                    }
                }
            }
        }
        else // Player 2 turn
        {
            // Own ball not in the pocket
            if (team_color == 2 && std::none_of(pockets.begin(), pockets.end(), &Ball::is_solid))
                player1turn = !cur_turn;
            else if (team_color == 1 && std::none_of(pockets.begin(), pockets.end(), &Ball::is_stripes))
                player1turn = !cur_turn;
            // 8 ball in the pocket
            else if (std::find(pockets.begin(), pockets.end(), 8) != pockets.end())
            {
                if (team_color == 0)
                    lost(cur_turn);
                else if (team_color == 2)
                {
                    if (std::any_of(std::begin(balls),
                                    std::end(balls),
                                    [](auto& b) {
                                        return b.is_visible && b.id > 0 && b.id < 8;
                                    }))
                    {
                        lost(cur_turn);
                    }
                    else
                    {
                        won(cur_turn);
                    }
                }
                else if (team_color == 1)
                {
                    if (std::any_of(std::begin(balls),
                                    std::end(balls),
                                    [](auto& b) {
                                        return b.is_visible && b.id > 8 && b.id < 16;
                                    }))
                    {
                        lost(cur_turn);
                    }
                    else
                    {
                        won(cur_turn);
                    }
                }
            }
        }

        // Check team color
        check_team_color(cur_turn);

        pockets.clear();
    }
}

void Level::won(bool cur_turn)
{
    Font f {};
    f.loadFromFile("./pool_assets/Purisa-BoldOblique.ttf", 20);

    Texture t {};

    if (cur_turn)
        t.loadFromRenderedText("Player 1 Wins!!", f, SDL_Color{0xFF,0xFF,0xFF,0xFF});
    else
        t.loadFromRenderedText("Player 2 Wins!!", f, SDL_Color{0xFF,0xFF,0xFF,0xFF});

    t.render(static_cast<int>((mainwindow->getWidth() - t.getWidth()) / 2),
             static_cast<int>((mainwindow->getHeight() - t.getHeight()) / 2));

    mainwindow->update();

    SDL_Delay(4000);

    GameState::next_state = GameStates::Intro;
}

void Level::lost(bool cur_turn)
{
    Font f {};
    f.loadFromFile("./pool_assets/Purisa-BoldOblique.ttf", 20);

    Texture t {};

    if (cur_turn)
        t.loadFromRenderedText("Player 1 Lost!!", f, SDL_Color{0xFF,0xFF,0xFF,0xFF});
    else
        t.loadFromRenderedText("Player 2 Lost!!", f, SDL_Color{0xFF,0xFF,0xFF,0xFF});

    t.render(static_cast<int>((mainwindow->getWidth() - t.getWidth()) / 2),
             static_cast<int>((mainwindow->getHeight() - t.getHeight()) / 2));

    mainwindow->update();

    SDL_Delay(4000);

    GameState::next_state = GameStates::Intro;
}

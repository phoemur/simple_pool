#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <memory>

enum class GameStates {
    Intro,
    Level,
    Won,
    Lost,
    Quit,
    Null
};

class GameState {
public:
    virtual void handle_events() = 0;
    virtual void logic() = 0;
    virtual void render() = 0;
    virtual ~GameState() noexcept;

    // Current game state
    static GameStates curr_state;
    static GameStates next_state;
};

void changeState(std::unique_ptr<GameState>& state);

#endif // GAMESTATE_H

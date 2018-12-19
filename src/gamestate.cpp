#include "gamestate.h"
#include "intro.h"
#include "level.h"
#include "texture.h"
#include "font.h"
#include "mainwindow.h"

GameStates GameState::curr_state = GameStates::Intro;
GameStates GameState::next_state = GameStates::Null;
GameState::~GameState() noexcept = default;


void changeState(std::unique_ptr<GameState>& state)
{
    if (GameState::next_state != GameStates::Null) {
        switch (GameState::next_state) {
            case GameStates::Intro:
                state.reset(nullptr);
                state = std::make_unique<Intro>();
                break;
            case GameStates::Level:
                state.reset(nullptr);
                state = std::make_unique<Level>();
                break;
            case GameStates::Won:
                //won();
                GameState::next_state = GameStates::Intro;
                state.reset(nullptr);
                state = std::make_unique<Intro>();
                break;
            case GameStates::Lost:
                //lost();
                GameState::next_state = GameStates::Intro;
                state.reset(nullptr);
                state = std::make_unique<Intro>();
                break;
            default:
                break;
        }

        GameState::curr_state = GameState::next_state;
        GameState::next_state = GameStates::Null;
    }
}

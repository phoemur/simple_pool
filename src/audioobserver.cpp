#include "audioobserver.h"

AudioObserver::AudioObserver()
    : wall{"./pool_assets/wall.wav"},
      ball{"./pool_assets/poolballhit.wav"},
      cue{"./pool_assets/cue.wav"},
      pocket{"./pool_assets/pocket.wav"}
{

}

void AudioObserver::onNotify(Subject&, Event event)
{
    switch (event) {
        case Event::SUBJECT_WALL_COLLIDED:
            wall.play();
            break;
        case Event::SUBJECT_BALL_COLLIDED:
            ball.play();
            break;
        case Event::SUBJECT_CUE_COLLIDED:
            cue.play();
            break;
        case Event::SUBJECT_POCKET_COLLIDED:
            pocket.play();
            break;
        default:
            break;
    }
}

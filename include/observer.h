#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>

class Subject; // Forward declare to break circular includes

enum class Event {
    SUBJECT_MOVED,
    SUBJECT_WALL_COLLIDED,
    SUBJECT_BALL_COLLIDED,
    SUBJECT_POCKET_COLLIDED,
    SUBJECT_CUE_COLLIDED
};

class Observer {
protected:
    std::vector<Subject*> observed_;
public:
    Observer();
    virtual ~Observer() noexcept;
    virtual void onNotify(Subject& entity, Event event) = 0;
    void registerSub(Subject* s);
    void unregisterSub(Subject* s);
};

#endif // OBSERVER_H

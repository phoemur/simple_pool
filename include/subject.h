#ifndef SUBJECT_H
#define SUBJECT_H

#include "observer.h"
#include <vector>

struct MovementData {
    double speed_x;
    double speed_y;
    double accel_x;
    double accel_y;
};

class Subject {
    std::vector<Observer*> observers_;
public:
    Subject();
    virtual ~Subject() noexcept;
    void addObserver(Observer* observer);
    void removeObserver(Observer* observer);
    void notify(Event event);

    virtual void render() = 0;
    virtual void move() = 0;
};

#endif // SUBJECT_H

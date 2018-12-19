#include "subject.h"

#include <algorithm>

Subject::Subject() : observers_{}
{
    observers_.reserve(5);
}

Subject::~Subject() noexcept
{
    for (Observer* ob: observers_)
        ob->unregisterSub(this);
}

void Subject::addObserver(Observer* observer)
{
    if (observer != nullptr)
    {
        bool found = std::find(std::begin(observers_), std::end(observers_), observer) != std::end(observers_);

        if (!found)
        {
            observers_.push_back(observer);
            observer->registerSub(this);
        }
    }
}

void Subject::removeObserver(Observer* observer)
{
    auto it = std::find(std::begin(observers_), std::end(observers_), observer);

    if (it != std::end(observers_))
    {
        observers_.erase(it);
        observer->unregisterSub(this);
    }
}

void Subject::notify(Event event)
{
    for (Observer* ob: observers_)
        ob->onNotify(*this, event);
}

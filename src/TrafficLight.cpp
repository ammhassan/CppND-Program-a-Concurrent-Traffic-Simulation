#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <chrono>
#include <future>

/* Implementation of class "MessageQueue" */
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mutx);
    _cond.wait(lock);
    auto msg{std::move(_queue.front())};
    _queue.pop_front();
    return msg; 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    const std::lock_guard<std::mutex> lock(_mutx);
    _queue.emplace_back(msg);
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    bool condition{true};
    while (condition)
    {
        if (_trafficLightQueue.receive() == TrafficLightPhase::green)
        {
            condition = false;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread{&TrafficLight::cycleThroughPhases, this});
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        if (_currentPhase == TrafficLightPhase::red)
        {
            _currentPhase = TrafficLightPhase::green;
        }
        else
        {
            _currentPhase = TrafficLightPhase::red;
        }
        auto msg = _currentPhase;
        auto msgFuture = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, 
                                    &_trafficLightQueue, std::move(msg));
        msgFuture.wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } 
}


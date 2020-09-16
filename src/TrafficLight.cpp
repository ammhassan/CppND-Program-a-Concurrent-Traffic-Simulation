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
    auto msg{std::move(_queue.back())};
    _queue.pop_back();
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
    std::random_device randomNum;
    std::mt19937 gen(randomNum());
    std::uniform_int_distribution<> uniformDistr(4000, 6000);

    // init stop watch
    auto lastUpdate = std::chrono::system_clock::now();

    while (true)
    {
        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();

        // toggle between the two phases when timeSinceLastUpdate exceeds the randomly generated cycle duration
        if (timeSinceLastUpdate >= uniformDistr(gen))
        {
            if (_currentPhase == TrafficLightPhase::red)
            {
                _currentPhase = TrafficLightPhase::green;
            }
            else
            {
                _currentPhase = TrafficLightPhase::red;
            }
            _trafficLightQueue.send(std::move(_currentPhase));
        
            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }

        // sleep between two cycles to reduce cpu usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } 
}


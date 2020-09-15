#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// Define TrafficLightPhase enum  to be used by class TrafficLight
enum TrafficLightPhase {red, green};

// forward declarations to avoid include cycle
class Vehicle;


template <class T>
class MessageQueue
{

public:
void send(T&& msg);
T receive();

private:
std::deque<T> _queue;
std::condition_variable _cond;
std::mutex _mutx;
    
};
 

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();
    
    // private members
    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _trafficLightQueue;
};

#endif
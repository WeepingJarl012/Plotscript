#ifndef MESSAGEQUEUE_HPP
#define MESSAGEQUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include "expression.hpp"

template<typename T>
class MessageQueue
{
public:
    // Adds the value to the queue
    void push(const T & value){
        std::unique_lock<std::mutex> lock(the_mutex);
        the_queue.push(value);
        lock.unlock();
        the_condition_variable.notify_one();
    }
    
    // Checks if queue is empty
    bool empty(){
        std::lock_guard<std::mutex> lock(the_mutex);
        return the_queue.empty();
    }
    
    // Trys to pop the next value from the queue
    bool try_pop(T& popped_value){
        std::lock_guard<std::mutex> lock(the_mutex);
        if (the_queue.empty()) {
            return false;
        }
        
        popped_value = the_queue.front();
        the_queue.pop();
        return true;
    }
    
    // Waits and pops the next entry in the queue
    void wait_and_pop(T & popped_value){
        std::unique_lock<std::mutex> lock(the_mutex);
        
        while (the_queue.empty()) {
            the_condition_variable.wait(lock);
        }
        
        popped_value = the_queue.front();
        the_queue.pop();
    }
    
private:
    std::condition_variable the_condition_variable;
    std::queue<T> the_queue;
    mutable std::mutex the_mutex;
};

#endif

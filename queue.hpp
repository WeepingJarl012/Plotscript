#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <queue>
#include <mutex>

template<typename T>
class Queue
{
public:
    void push(const T & item){
        std::lock_guard<std::mutex> lock(the_mutex);
        the_queue.push(item);
    }
    
    void pop(){
        std::lock_guard<std::mutex> lock(the_mutex);
        the_queue.pop();
    }
    
    std::size_t size() const {
        std::lock_guard<std::mutex> lock(the_mutex);
        return the_queue.size();
    }
    
    T front(){
        std::lock_guard<std::mutex> lock(the_mutex);
        T result;
        if(the_queue.size() > 0){
            result = the_queue.front();
            return result;
        }
        return result;
    }
    
private:
    std::queue<T> the_queue;
    mutable std::mutex the_mutex;
};

#endif

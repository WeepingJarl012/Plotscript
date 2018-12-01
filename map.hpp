#ifndef MAP_HPP
#define MAP_HPP

#include <map>
#include <mutex>
#include "expression.hpp"

class Map
{
public:
    // Adds the key and value to the map
    void emplace(std::string key, Expression value){
        std::lock_guard<std::mutex> lock(the_mutex);
        the_map.emplace(key, value);
    }
    
    // Erases the entry in the map with requested key
    void erase(std::string key){
        std::lock_guard<std::mutex> lock(the_mutex);
        auto search = the_map.find(key);
        if (search != the_map.end()){
            the_map.erase(search);
        } else {
            // Do nothing since value is not in map
        }
    }
    
    // Returns the value mapped to the requested key, empty expression if not found
    Expression find(std::string key){
        std::lock_guard<std::mutex> lock(the_mutex);
        auto search = the_map.find(key);
        if (search != the_map.end()){
            return search->second;
        } else {
            return Expression();
        }
    }
    
    // Return the current size of the map
    std::size_t size() const {
        std::lock_guard<std::mutex> lock(the_mutex);
        return the_map.size();
    }
    
private:
    std::map<std::string, Expression> the_map;
    mutable std::mutex the_mutex;
};

#endif

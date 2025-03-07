#ifndef MULTIMAP_H
#define MUTILMAP_H

#include <iostream>
#include <map>
#include <vector>

template <typename K, typename V>
class Multimap {
private:
    std::map< K, std::vector<V>> data;
public:
    // Insert a key-value pair
    void Insert(const K& key, const V& value)
    {
        data[key].push_back(value);
    }
    // Getting all the values associated with the value
    std::vector<V> Get(const K& key) const{
        auto it= data.find(key);
        for(it!=data.end()){
            return it->second;// vector of values attached to key
        }
        return {};
    }
    void Remove(const K& key, const V& value ){
        auto it=data.find(key);
        for(it!=data.end()){
            auto &values =it->second;
            values.erase(std::remove(values.begin(),values.end(), value), values.end());
            if(values.empty()){
                data.erase(it);
            }
        }
    }
   void RemoveAll(const K& key){
    data.erase(key);
   }

    bool Contains(const K& key)const{
        return data.find(key)!=data.end();
    }
    void Print() const{
        for(const auto& pair: data){
            std::cout<<pair.first<< ": ";
            for(const auto& value: pair.second) {
                std::cout << value << " ";
            }
            std::cout<< std::endl;
        }
    }
};

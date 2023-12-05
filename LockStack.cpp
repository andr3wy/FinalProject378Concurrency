#include <exception>
#include <stack>
#include <mutex>
#include <stdlib.h>
#include <memory>

#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <ostream>
#include <chrono>
#include <mutex>
#include <assert.h>

using namespace std;

template<typename T>
class LockStack
{
    private:
        std::stack<T> data;
        pthread_mutex_t mutex;
    public:
    LockStack(){
        pthread_mutex_init(&mutex, NULL);
    }

    void push(T new_value)
    {
        pthread_mutex_lock(&mutex);
        data.push(std::move(new_value));
        pthread_mutex_unlock(&mutex);
    }

    std::shared_ptr<T> peek()
    {
        pthread_mutex_lock(&mutex);
        if(data.empty()) {
            pthread_mutex_unlock(&mutex);
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(
        std::make_shared<T>(data.top()));
        pthread_mutex_unlock(&mutex);
        return res;
    }

    std::shared_ptr<T> pop()
    {
        pthread_mutex_lock(&mutex);
        if(data.empty()) {
            pthread_mutex_unlock(&mutex);
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(
        std::make_shared<T>(std::move(data.top())));
        data.pop();
        pthread_mutex_unlock(&mutex);
        return res;
    }
    void pop(T& value)
    {
        pthread_mutex_lock(&mutex);
        if(data.empty()) {
            pthread_mutex_unlock(&mutex);
            return;
        }
            
        value=std::move(data.top());
        data.pop();
        pthread_mutex_unlock(&mutex);
    }
    bool empty() const
    {
        bool res;
        pthread_mutex_lock(&mutex);
        res = data.empty();
        pthread_mutex_unlock(&mutex);
        return res;
    }
};
// #include <ctype.h>
// #include <string>
// #include <mutex>
// #include <condition_variable>
// #include <queue>

// template <class T>
// class ConcurrentQueueBase{
// public:
//     virtual std::string description() const = 0;

//     virtual std::recursive_mutex& getPopMutex() = 0;

//     virtual void lock() const = 0;
//     virtual void unlock() const = 0;
//     virtual bool try_lock() const = 0;

//     virtual void push(const T &val) = 0;

//     virtual T pop() = 0;
//     virtual T peek() = 0;
//     virtual bool tryPop(T *target=nullptr) = 0;

//     virtual bool empty() const = 0;
//     virtual size_t size() const = 0;

//     virtual ~ConcurrentQueueBase() = default;
// };

// template <class T>
// class ConcurrentQueueSimple final: public ConcurrentQueueBase<T>{
// public:
//     std::string description() const override { return "ConcurrentDequeSimple"; }

//     std::recursive_mutex& getPopMutex() override { return mux_; }

//     void lock() const override { mux_.lock(); }
//     void unlock() const override { mux_.unlock(); }
//     bool try_lock() const override { return mux_.try_lock(); }

//     void push(const T &val) override {
//         std::lock_guard<std::recursive_mutex> lck(mux_);
//         q_.push(val);
//     }

//     T pop() override {
//         std::unique_lock<std::recursive_mutex> lck(mux_);

//         while(empty())
//             continue;

//         T res = q_.front();
//         q_.pop();

//         return res;
//     }

//     T peek() override {
//         std::unique_lock<std::recursive_mutex> lck(mux_);

//         if(empty())
//             return NULL;

//         T res = q_.front();
//         return res;
//     }


//     bool tryPop(T *target=nullptr) override {
//         std::lock_guard<std::recursive_mutex> lck(mux_);

//         if(empty())
//             return false;
        
//         if(target != nullptr)
//             *target = q_.front();
        
//         q_.pop();

//         return true;
//     }

//     bool empty() const override{
//         std::lock_guard<std::recursive_mutex> lck(mux_);
//         return q_.empty();
//     }
//     size_t size() const override{
//         std::lock_guard<std::recursive_mutex> lck(mux_);
//         return q_.size();
//     }

//     virtual ~ConcurrentQueueSimple() = default;
// private:
//     mutable std::recursive_mutex mux_{};
//     std::queue<T> q_{};
// };



















#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <ostream>
#include <chrono>
#include <mutex>
#include <assert.h>

using namespace std;

typedef struct __node_t {
int value;
struct __node_t *next;
} node_t;

typedef struct __queue_t {
node_t *head;
node_t *tail;
pthread_mutex_t head_lock, tail_lock;
} queue_t;

void Queue_Init(queue_t *q) {
    node_t *tmp = (node_t *) malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

void Queue_Enqueue(queue_t *q, int value) {
    node_t *tmp = (node_t *) malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;

    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
}

int Queue_Dequeue(queue_t *q, int *value) {
    pthread_mutex_lock(&q->head_lock);
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if (new_head == NULL) {
        pthread_mutex_unlock(&q->head_lock);
        return -1; // queue was empty
    }
    *value = new_head->value;
    q->head = new_head;
    pthread_mutex_unlock(&q->head_lock);
    free(tmp);
    return 0;
}

int Queue_Peek(queue_t *q) {
    int val = 0;
    pthread_mutex_lock(&q->head_lock);
    if (q->head != NULL )
        val = q->head->value;
    pthread_mutex_unlock(&q->head_lock);
    return val;
}
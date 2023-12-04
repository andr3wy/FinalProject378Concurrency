#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <ostream>
#include <chrono>
#include <utility>

#include <vector>


#include "main.hpp"
#include "FreeStack.cpp"
#include "LockStack.cpp"
#include "FreeQueue.cpp"
#include "LockQueue.cpp"
#include "FreeMap.cpp"
#include "LockMap.cpp"
using namespace std;

void read_arguments(int argc, char** argv, Params& params) {
    int opt;
    while ((opt = getopt(argc, argv, "r:w:R:W:")) != -1) {
        switch (opt) {
            case 'r':
                params.readers = atoi(optarg);
                break;
            case 'w':
                params.writers = atoi(optarg);
                break;
            case 'R':
                params.reads = atoi(optarg);
                break;
            case 'W':
                params.writes = atoi(optarg);
                break;
            case '?':
                std::cerr << "Usage: " << argv[0] << " -n <n> -i <i> -o <o> -s <s>" << std::endl;
        }
    }
}

// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// QUEUE

void *free_queue_write(void *input) {

    Params *params = (Params *) input;
    lockfree::mpmc::Queue <int, 1000> *free_queue = (lockfree::mpmc::Queue <int, 1000> *) (params->data_structure);
    //
    int tmp;
    for (int i = 0; i < params->writes / 2; ++ i ) {
        (*free_queue).Push(1);
        (*free_queue).Pop(tmp);
    }

    return NULL;
}

void *free_queue_read(void *input) {

    Params *params = (Params *) input;
    lockfree::mpmc::Queue <int, 1000> *free_queue = (lockfree::mpmc::Queue <int, 1000> *) (params->data_structure);
    for (int i = 0; i < params->reads; ++ i ) {
        (*free_queue).Peek();
    }

    return NULL;
}

void *lock_queue_write(void *input) {

    Params *params = (Params *) input;
    queue_t *queue = (queue_t *) (params->data_structure);
    int tmp;
    for (int i = 0; i < params->writes / 2; ++ i ) {
        Queue_Enqueue(queue, 1);
        Queue_Dequeue(queue, &tmp);
        // (*queue).push(1);
        // (*queue).pop();
    }

    return NULL;
}

void *lock_queue_read(void *input) {

    Params *params = (Params *) input;
    queue_t *queue = (queue_t *) (params->data_structure);
    for (int i = 0; i < params->reads; ++ i ) {
        Queue_Peek(queue);
    }

    return NULL;
}


// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// STACK
void *free_stack_write(void *input) {

    Params *params = (Params *) input;
    FreeStack <int> *free_stack = (FreeStack<int> *) (params->data_structure);
    for (int i = 0; i < params->writes / 2; ++ i ) {
        (*free_stack).push(1);
        (*free_stack).pop();
    }

    return NULL;
}

void *free_stack_read(void *input) {

    Params *params = (Params *) input;
    FreeStack <int> *free_stack = (FreeStack<int> *) (params->data_structure);
    for (int i = 0; i < params->reads; ++ i ) {
        (*free_stack).peek();
    }

    return NULL;
}

void *lock_stack_write(void *input) {

    Params *params = (Params *) input;
    LockStack <int> *lock_stack = (LockStack<int> *) (params->data_structure);
    for (int i = 0; i < params->writes / 2; ++ i ) {
        (*lock_stack).push(1);
        (*lock_stack).pop();
    }

    return NULL;
}

void *lock_stack_read(void *input) {

    Params *params = (Params *) input;
    LockStack <int> *lock_stack = (LockStack<int> *) (params->data_structure);
    for (int i = 0; i < params->reads; ++ i ) {
        (*lock_stack).peek();
    }

    return NULL;
}


// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// MAP

void *free_map_write(void *input) {
    Params *params = (Params *) input;
    struct hasher {
        uint32_t operator()(const int& i) {
            return static_cast<uint32_t>(i);
        }
    };
    hmap<int, int, hasher, 32*1024, 2 * 1024 * 1024> *map = (hmap<int, int, hasher, 32*1024, 2 * 1024 * 1024> *) (params->data_structure);
    for(int i = 0; i < params->writes; ++ i) {
        map->insert(i + pthread_self() * params->writes, i );
    }

    return NULL;
}
void *free_map_read(void *input) {
    Params *params = (Params *) input;
    struct hasher {
        uint32_t operator()(const int& i) {
            return static_cast<uint32_t>(i);
        }
    };
    hmap<int, int, hasher, 32*1024, 2 * 1024 * 1024> *map = (hmap<int, int, hasher, 32*1024, 2 * 1024 * 1024> *) (params->data_structure);
    for(int i = 0; i < params->writes; ++ i) {
        map->find(i + pthread_self() * params->writes);
    }

    return NULL;
}

void *lock_map_write(void *input) {
    Params *params = (Params*) input;
    threadsafe_lookup_table<int,double> * ihatecodingmap = (threadsafe_lookup_table<int, double> *)  (params->data_structure);

    for(int i = 0; i < params->writes; ++i) {
        ihatecodingmap->add_or_update_mapping(i + pthread_self() * params->writes, i);
        
    }
    // map(32*1024);
    // for(int i = 0; i < params->writes; ++i) {
        
    // }

    return NULL;
}

void *lock_map_read(void *input) {
    Params *params = (Params*) input;
    threadsafe_lookup_table<int,double> * map = (threadsafe_lookup_table<int, double> *)  (params->data_structure);

    for(int i = 0; i < params->writes; ++i) {
        map->value_for(i + pthread_self() * params->writes);
        
    }

    return NULL;
}

int main (int argc, char** argv) {

    Params params;
    read_arguments(argc, argv, params);

    FreeStack <int> free_stack;

    params.data_structure = &free_stack;

    pthread_t tid_readers[params.readers];
    pthread_t tid_writers[params.writers];


// ==========================================================================
// ==========================================================================
// ==========================================================================
// ==========================================================================
// STACK
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < params.readers; i ++) {
        pthread_create(&tid_readers[i], NULL, free_stack_read, &params);
    }


    for (int i = 0; i < params.writers; i ++) {
        pthread_create(&tid_writers[i], NULL, free_stack_write, &params);
    }

    for (int i = 0; i < params.readers; i ++) {
        pthread_join(tid_readers[i], NULL);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_join(tid_writers[i], NULL);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << duration.count() << endl;


    // lock stack
    LockStack <int> lock_stack;
    params.data_structure = &lock_stack;

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < params.readers; i ++) {
        pthread_create(&tid_readers[i], NULL, lock_stack_read, &params);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_create(&tid_writers[i], NULL, lock_stack_write, &params);
    }

    for (int i = 0; i < params.readers; i ++) {
        pthread_join(tid_readers[i], NULL);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_join(tid_writers[i], NULL);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << duration.count() << endl;


    // ==========================================================================
    // ==========================================================================
    // ==========================================================================
    // ==========================================================================
    // QUEUE


    lockfree::mpmc::Queue <int, 1000> free_queue;
    queue_t queue;
    Queue_Init(&queue);

    params.data_structure = &free_queue;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < params.readers; i ++) {
        pthread_create(&tid_readers[i], NULL, free_queue_read, &params);
    }


    for (int i = 0; i < params.writers; i ++) {
        pthread_create(&tid_writers[i], NULL, free_queue_write, &params);
    }

    for (int i = 0; i < params.readers; i ++) {
        pthread_join(tid_readers[i], NULL);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_join(tid_writers[i], NULL);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << duration.count() << endl;


    params.data_structure = &queue;

    

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < params.readers; i ++) {
        pthread_create(&tid_readers[i], NULL, lock_queue_read, &params);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_create(&tid_writers[i], NULL, lock_queue_write, &params);
    }

    for (int i = 0; i < params.readers; i ++) {
        pthread_join(tid_readers[i], NULL);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_join(tid_writers[i], NULL);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << duration.count() << endl;




    struct hasher {
        uint32_t operator()(const int& i) {
            return static_cast<uint32_t>(i);
        }
    };
    hmap<int, int, hasher, 32*1024, 2 * 1024 * 1024> map;

    params.data_structure = &map;

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < params.readers; i ++) {
        pthread_create(&tid_readers[i], NULL, free_map_read, &params);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_create(&tid_writers[i], NULL, free_map_write, &params);
    }

    for (int i = 0; i < params.readers; i ++) {
        pthread_join(tid_readers[i], NULL);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_join(tid_writers[i], NULL);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << duration.count() << endl;


    
    // threadsafe_lookup_table<int,double> thread_lock_map() = );
    // int size = 32 * 1024;
    params.data_structure = new threadsafe_lookup_table<int, double>();

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < params.readers; i ++) {
        pthread_create(&tid_readers[i], NULL, lock_map_read, &params);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_create(&tid_writers[i], NULL, lock_map_write, &params);
    }

    for (int i = 0; i < params.readers; i ++) {
        pthread_join(tid_readers[i], NULL);
    }

    for (int i = 0; i < params.writers; i ++) {
        pthread_join(tid_writers[i], NULL);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << duration.count() << endl;

    return 0;

}
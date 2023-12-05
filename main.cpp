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
    // lockfree::mpmc::Queue <int, 1000> *free_queue = (lockfree::mpmc::Queue <int, 1000> *) (params->data_structure);
    //
    FreeQueue<int> *free_queue = (FreeQueue<int> *) (params->data_structure);
    int tmp;
    for (int i = 0; i < params->writes; ++ i ) {
        (*free_queue).push(1);
        int lol = 0;
        (*free_queue).pop(lol);
    }

    return NULL;
}

void *free_queue_read(void *input) {

    Params *params = (Params *) input;
    FreeQueue <int> *free_queue = (FreeQueue <int> *) (params->data_structure);
    int tmp;
    for (int i = 0; i < params->reads; ++ i ) {
        // (*free_queue).peek();
        int lol;
        (*free_queue).pop(lol);
    }

    return NULL;
}

void *lock_queue_write(void *input) {

    Params *params = (Params *) input;
    Queue *queue = (Queue *) (params->data_structure);
    int tmp;
    for (int i = 0; i < params->writes; ++ i ) {
        (*queue).enqueue(1);
        (*queue).dequeue(&tmp);
    }

    return NULL;
}

void *lock_queue_read(void *input) {

    Params *params = (Params *) input;
    Queue *queue = (Queue *) (params->data_structure);
    int tmp;
    for (int i = 0; i < params->reads; ++ i ) {
        (*queue).peek();
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
    for (int i = 0; i < params->writes; ++ i ) {
        (*free_stack).push(1);
        // (*free_stack).pop();
    }

    return NULL;
}

void *free_stack_read(void *input) {

    Params *params = (Params *) input;
    FreeStack <int> *free_stack = (FreeStack<int> *) (params->data_structure);
    for (int i = 0; i < params->reads; ++ i ) {
        // (*free_stack).peek();
        (*free_stack).pop();
    }

    return NULL;
}

void *lock_stack_write(void *input) {

    Params *params = (Params *) input;
    LockStack <int> *lock_stack = (LockStack<int> *) (params->data_structure);
    for (int i = 0; i < params->writes; ++ i ) {
        (*lock_stack).push(1);
        // (*lock_stack).pop();
    }

    return NULL;
}

void *lock_stack_read(void *input) {

    Params *params = (Params *) input;
    LockStack <int> *lock_stack = (LockStack<int> *) (params->data_structure);
    for (int i = 0; i < params->reads; ++ i ) {
        // (*lock_stack).peek();
        (*lock_stack).pop();
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
    HashMap<int,double> * ihatecodingmap = (HashMap<int, double> *)  (params->data_structure);

    for(int i = 0; i < params->writes; ++i) {
        ihatecodingmap->put(i + pthread_self() * params->writes, i);
        
    }
    // map(32*1024);
    // for(int i = 0; i < params->writes; ++i) {
        
    // }

    return NULL;
}

void *lock_map_read(void *input) {
    Params *params = (Params*) input;
    HashMap<int,double> * map = (HashMap<int, double> *)  (params->data_structure);

    double tmp;
    for(int i = 0; i < params->writes; ++i) {
        int j = i + pthread_self() * params->writes;
        map->get(j, tmp);
        
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
    cout << "FreeStack: " << duration.count() << endl;


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
    cout << "LockStack: " << duration.count() << endl;


    // ==========================================================================
    // ==========================================================================
    // ==========================================================================
    // ==========================================================================
    // QUEUE


    FreeQueue <int> free_queue(100000);
    Queue queue;

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
    cout << "FreeQueue: " << duration.count() << endl;


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
    cout << "LockQueue: " << duration.count() << endl;




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
    cout << "FreeMap: " << duration.count() << endl;


    
    // threadsafe_lookup_table<int,double> thread_lock_map() = );
    // int size = 32 * 1024;
    params.data_structure = new HashMap<int, double>();

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
    cout << "LockMap: " << duration.count() << endl;

    return 0;

}
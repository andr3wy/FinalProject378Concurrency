

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <ostream>
#include <chrono>
#include <mutex>
#include <assert.h>


template <typename K, typename V>
class HashNode {
public:
    K key;  
    V value;
    // next bucket with the same key
    HashNode *next;

    HashNode(const K &key, const V &value) :
    key(key), value(value), next(NULL) {
    }

};

#define TABLE_SIZE 50000

static pthread_mutex_t locks[TABLE_SIZE];

// Hash map class template
template <typename K, typename V>
class HashMap {
public:
    HashMap() {
        // construct zero initialized hash table of size
        table = new HashNode<K, V> *[TABLE_SIZE]();
        // locks = new pthread_mutex_t [TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i ++) {
            pthread_mutex_init(&locks[i], NULL);
        }
    }

    ~HashMap() {
        // destroy all buckets one by one
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode<K, V> *entry = table[i];
            while (entry != NULL) {
                HashNode<K, V> *prev = entry;
                entry = entry->next;
                delete prev;
            }
            table[i] = NULL;
            pthread_mutex_destroy(&locks[i]);
        }
        // destroy the hash tables
        delete [] table;
        delete [] locks;
    }

    bool get(const K &key, V &value) {
        unsigned long hashValue = hashFunc(key);
        pthread_mutex_lock(&locks[hashValue]);
        HashNode<K, V> *entry = table[hashValue];

        while (entry != NULL) {
            if (entry->key == key) {
                value = entry->value;
                return true;
            }
            entry = entry->next;
        }
        pthread_mutex_unlock(&locks[hashValue]);
        return false;
    }

    void put(const K &key, const V &value) {
        unsigned long hashValue = hashFunc(key);
        
        pthread_mutex_lock(&locks[hashValue]);
        HashNode<K, V> *prev = NULL;
        HashNode<K, V> *entry = table[hashValue];
        

        while (entry != NULL && entry->key != key) {
            prev = entry;
            entry = entry->next;
        }

        if (entry == NULL) {
            entry = new HashNode<K, V>(key, value);
            if (prev == NULL) {
                // insert as first bucket
                table[hashValue] = entry;
            } else {
                prev->next = entry;
            }
        } else {
            // just update the value
            entry->value = value;
        }
        pthread_mutex_unlock(&locks[hashValue]);
    }

    void remove(const K &key) {
        unsigned long hashValue = hashFunc(key);
        pthread_mutex_lock(&locks[hashValue]);
        HashNode<K, V> *prev = NULL;
        HashNode<K, V> *entry = table[hashValue];

        while (entry != NULL && entry->key != key) {
            prev = entry;
            entry = entry->next;
        }

        if (entry == NULL) {
            // key not found
            pthread_mutex_unlock(&locks[hashValue]);
            return;
        }
        else {
            if (prev == NULL) {
                // remove first bucket of the list
                table[hashValue] = entry->next;
            } else {
                prev->next = (entry->next);
            }
            delete entry;
        }
        pthread_mutex_unlock(&locks[hashValue]);
    }

    int hashFunc(const K &key) {
        return static_cast<unsigned long>(key) % TABLE_SIZE;
    }

private:
    // hash table
    HashNode<K, V> **table;
    // pthread_mutex_t **locks;
};
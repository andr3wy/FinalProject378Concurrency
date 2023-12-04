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

#define BUCKETS (101)

// basic node structure
typedef struct __node_t {
int key;
struct __node_t *next;
} node_t;

// basic list structure (one used per list)
typedef struct __list_t {
node_t *head;
pthread_mutex_t lock;
} list_t;

void List_Init(list_t *L) {
L->head = NULL;
pthread_mutex_init(&L->lock, NULL);
}
int List_Insert(list_t *L, int key) {
// synchronization not needed
node_t *new_node = (node_t *) malloc(sizeof(node_t));
if (new_node == NULL) {
perror("malloc");
return -1;
}
new_node->key = key;
// just lock critical section
pthread_mutex_lock(&L->lock);
new_node->next = L->head;
L->head = new_node;
pthread_mutex_unlock(&L->lock);
return 0; // success
}

int List_Lookup(list_t *L, int key) {
int rv = -1;
pthread_mutex_lock(&L->lock);
node_t *curr = L->head;
while (curr) {
if (curr->key == key) {
rv = 0;
break;
}
curr = curr->next;
}
pthread_mutex_unlock(&L->lock);
return rv; // now both success and failure
}

typedef struct __hash_t {
list_t lists[BUCKETS];
} hash_t;

void Hash_Init(hash_t *H) {
    int i;
    for (i = 0; i < BUCKETS; i++) 
        List_Init(&H->lists[i]);
}

int Hash_Insert(hash_t *H, int key) {
return List_Insert(&H->lists[key % BUCKETS], key);
}

int Hash_Lookup(hash_t *H, int key) {
return List_Lookup(&H->lists[key % BUCKETS], key);
}
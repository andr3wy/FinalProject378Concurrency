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

class Queue {
    typedef struct node_t {
        int value;
        struct node_t* next;
    } note_t;

    node_t *head;
    node_t *tail;
    pthread_mutex_t head_lock, tail_lock;

    public :
        Queue() {
            node_t *tmp = (node_t *) malloc(sizeof(node_t));
            tmp->next = NULL;
            head = tail = tmp;
            pthread_mutex_init(&head_lock, NULL);
            pthread_mutex_init(&tail_lock, NULL);
        }

        void enqueue(int value) {
            node_t *tmp = (node_t *) malloc(sizeof(node_t));
            assert(tmp != NULL);
            tmp->value = value;
            tmp->next = NULL;

            pthread_mutex_lock(&tail_lock);
            tail->next = tmp;
            tail = tmp;
            pthread_mutex_unlock(&tail_lock);
        }

        int dequeue(int *value) {
            pthread_mutex_lock(&head_lock);
            node_t *tmp = head;
            node_t *new_head = tmp->next;
            if (new_head == NULL) {
                pthread_mutex_unlock(&head_lock);
                return -1; // queue was empty
            }
            *value = new_head->value;
            head = new_head;
            pthread_mutex_unlock(&head_lock);
            free(tmp);
            return 0;
        }

        int peek() {
            int val = 0;
            pthread_mutex_lock(&head_lock);
            if (head != NULL )
                val = head->value;
            pthread_mutex_unlock(&head_lock);
            return val;
        }
};
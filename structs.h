#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    unsigned char* data;
    node* next;
} node;

typedef struct {
    node* front;
    node* rear;

} queue;


typedef struct {
    short health;
    short maxHealth;
    short attack;
} enemy;

#endif

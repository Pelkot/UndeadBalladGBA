#ifndef STRUCTS_H
#define STRUCTS_H

// data structures
typedef struct {
    unsigned char* data;
    unsigned char* next;
} node;

typedef struct {
    node* front;
    node* rear;

} queue;

typedef struct{
    unsigned char id; // item id
    unsigned char type; // 0=permanent, 1=consumable

    char* message;

    unsigned char useType; // 0=heal, 1=boost attack, 2=melee, 3=ranged, 4=defense
    unsigned char useAmount; // how much the item heals/hurts 
    unsigned char useArea; // how many tiles the item affects for attack items
    unsigned char staminaCost; // how much stamina it costs to use the item

    unsigned const short* bitmap; // pointer to the item's bitmap for the inventory screen
} item;

typedef struct {
    short health;
    short maxHealth;
    short attack;
    
    unsigned const short* bitmap;
} enemy;

#endif

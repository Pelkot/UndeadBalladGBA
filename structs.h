#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct{
    
} animation;

typedef struct{
    unsigned char type; // 1=permanent, 0=consumable

    char* message;
    char* useMessage;

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
    short facingLeft;
    char nextMove; // 0= no move, 1= move left, 2= move right, 3= attack left, 4= attack right
    
    unsigned const short* bitmap;
} enemy;

#endif

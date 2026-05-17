#include "gba.h"
#include "textures.h"
#include "structs.h"

u8 playerHealth;
u8 maxPlayerHealth;

u8 playerStrength;

u8 playerStamina;
u8 maxPlayerStamina;

item* inventory[14]; // slots 0-3 are permanent items, 4-13 are consumables

item allItems[256]; // all items in the game, indexed by item id

// heal items (0-15)

// boost items (16-31)

// melee items (32-47)
allItems[32] = {32, 0, *"Broad Sword\n2 Melee Dmg", 2, 2, 2, 2, *broadSword_Map};

// ranged items (48-63)

// defense items (64-79)


void init()
{
  playerHealth=5;
  maxPlayerHealth=5;

  playerStrength=0;

  playerStamina=5;
  maxPlayerStamina=5;
}

void characterCreation()
{
}

void selectLevel()
{
}

void fight()
{
    drawImage(120,80, 0,0, fightBackground_Map, 0); //draw fight background
}

void shopLevelUp()
{
}

#include "gba.h"
#include "textures.h"
#include "structs.h"

u8 playerHealth;
u8 maxPlayerHealth;

u8 playerStrength;

u8 playerStamina;
u8 maxPlayerStamina;

item* inventory[14]; // slots 0-3 are permanent items, 4-13 are consumables

u8 inventoryYs[14] = {3, 3, 11, 11, 22, 22, 30, 30, 38, 38, 46, 46, 54, 54}; // y positions for inventory items




void init()
{
  playerHealth=5;
  maxPlayerHealth=5;

  playerStrength=0;

  playerStamina=5;
  maxPlayerStamina=5;

  item allItems[256]; // all items in the game, indexed by item id
  //              ID, type, message, useType, useAmount, useArea, staminaCost, bitmap
  // heal items (0-15)
  allItems[0] = (item){.id = 32, .type = 0, .message = "Health Potion\n+30 Health", .useType = 0, .useAmount = 5, .useArea = 0, .staminaCost = 0, .bitmap = healthPotion_Map};

  // boost items (16-31)

  // melee items (32-47)
  allItems[32] = (item){.id = 32, .type = 0, .message = "Broad Sword\n2 Melee Dmg", .useType = 2, .useAmount = 2, .useArea = 2, .staminaCost = 2, .bitmap = broadSword_Map};

  // ranged items (48-63)

  // defense items (64-79)

  inventory[0] = &allItems[32]; // give player broad sword at start
  inventory[4] = &allItems[0]; // give player health potion at start
}

void drawInventory()
{
  for (int i = 0; i < 14; i++)
  {
    if (inventory[i] != 0)
    {
      if (i % 2 == 0) // left column
      {
        drawImage(8, 8, 1, inventoryYs[i], inventory[i]->bitmap, 0);
      }
      else // right column
      {
        drawImage(8, 8, 10, inventoryYs[i], inventory[i]->bitmap, 0);
      }
    }
  }
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
    drawInventory();
}

void shopLevelUp()
{
}

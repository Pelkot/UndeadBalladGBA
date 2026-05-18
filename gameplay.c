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
item allItems[256]; // all items in the game, indexed by item id

void makeItems() // create all the items in the game and store them in allItems array for easy access when adding to inventory or shops
{
  //              ID, type, message, useType, useAmount, useArea, staminaCost, bitmap
  // heal items (1-15)
  allItems[0] = (item){.id = 0, .type = 0, .message = "Health Potion\n+30 Health", .useType = 0, .useAmount = 5, .useArea = 0, .staminaCost = 0, .bitmap = healthPotion_Map};

  // boost items (16-31)
  allItems[16] = (item){.id = 16, .type = 0, .message = "Fire Grease\n+2 Attack", .useType = 1, .useAmount = 2, .useArea = 0, .staminaCost = 0, .bitmap = fireGrease_Map};

  // melee items (32-47)
  allItems[32] = (item){.id = 32, .type = 0, .message = "Broad Sword\n2 Melee Dmg", .useType = 2, .useAmount = 2, .useArea = 2, .staminaCost = 2, .bitmap = broadSword_Map};

  // ranged items (48-63)
  allItems[48] = (item){.id = 48, .type = 1, .message = "Fire Bomb\n3 Ranged Dmg", .useType = 3, .useAmount = 3, .useArea = 2, .staminaCost = 1, .bitmap = firebomb_Map};
  allItems[49] = (item){.id = 49, .type = 1, .message = "Bow\n2 Ranged Dmg", .useType = 3, .useAmount = 1, .useArea = 2, .staminaCost = 2, .bitmap = bow_Map};  

  // defense items (64-79)
  allItems[64] = (item){.id = 64, .type = 1, .message = "Shield\n+2 Defense", .useType = 4, .useAmount = 2, .useArea = 0, .staminaCost = 1, .bitmap = shield_Map};
}


void init()
{
  playerHealth=5;
  maxPlayerHealth=5;

  playerStrength=0;

  playerStamina=5;
  maxPlayerStamina=5;

  for (int i = 0; i < 14; i++)
  {
    inventory[i] = 0; // initialize inventory to empty
  }
  makeItems(); // create all the items in the game and store them in allItems array

  inventory[0] = &allItems[32]; // give player broad sword at start
  inventory[1] = &allItems[64]; // give player shield at start
  inventory[2] = &allItems[49]; // give player bow at start

  inventory[4] = &allItems[0]; // give player health potion at start
  inventory[5] = &allItems[48]; // give player fire bomb at start
  inventory[6] = &allItems[16]; // give player fire grease at start

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

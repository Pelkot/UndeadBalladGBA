#include "gba.h"
#include "textures.h"
#include "structs.h"

int timeout_timer = 0;
u8 fightState = 0; //0= inventory, 1= selecting move, 2= selecting ranged, 3= selecting melee, 4= waiting/enemy turn

// Player Stats
u8 playerHealth;
u8 maxPlayerHealth;

u8 playerStrength;

u8 playerStamina;
u8 maxPlayerStamina;

u8 damageBoost=0;
u8 defence=0;

// Inventory
item* inventory[14]; // slots 0-3 are permanent items, 4-13 are consumables

u8 inventoryYs[14] = {3, 3, 11, 11, 22, 22, 30, 30, 38, 38, 46, 46, 54, 54}; // y positions for inventory items
item allItems[256]; // all items in the game, indexed by item id

// Menus
u8 selected = 0; // which inventory slot is currently selected
u8 selectorTimeout = 0; // for controlling how fast the selector moves when holding a button
const u8 selectorSpeed = 2; // how many frames to wait before allowing the selector to move again when holding a button
char* currText;

u8 rangedSelection=0; // the selected tile that the player is attacking
u8 meleeSelection=0; // 0= left, 1= right

// Gameplay
enemy* tiles[7];
enemy enemy1;
enemy enemy2;
enemy enemy3;
enemy enemy4;

u8 playerLocation;
u8 playerFacingLeft = 0;
int tileXs[7] = {21, 35, 49, 63, 77, 91, 105};


void makeItems() // create all the items in the game and store them in allItems array for easy access when adding to inventory or shops
{
  // heal items (1-15)
  allItems[0] = (item){.type = 0, .message = "Health Potion\n+5 Health", .useType = 0, .useAmount = 5, .useArea = 0, .staminaCost = 1, .bitmap = healthPotion_Map};

  // boost items (16-31)
  allItems[16] = (item){.type = 0, .message = "Fire Grease\n+2 Attack", .useType = 1, .useAmount = 2, .useArea = 0, .staminaCost = 1, .bitmap = fireGrease_Map};

  // melee items (32-47)
  allItems[32] = (item){.type = 1, .message = "Broad Sword\n2 Melee Dmg", .useType = 2, .useAmount = 2, .useArea = 2, .staminaCost = 2, .bitmap = broadSword_Map};

  // ranged items (48-63)
  allItems[48] = (item){.type = 0, .message = "Fire Bomb\n3 Ranged Dmg", .useType = 3, .useAmount = 3, .useArea = 2, .staminaCost = 1, .bitmap = firebomb_Map};
  allItems[49] = (item){.type = 1, .message = "Bow\n1 Ranged Dmg", .useType = 3, .useAmount = 1, .useArea = 1, .staminaCost = 2, .bitmap = bow_Map};  

  // defense items (64-79)
  allItems[64] = (item){.type = 1, .message = "Shield\n+2 Defense", .useType = 4, .useAmount = 2, .useArea = 0, .staminaCost = 1, .bitmap = shield_Map};
}


void init()
{
  playerHealth=5;
  maxPlayerHealth=10;

  playerStrength=0;

  playerStamina=5;
  maxPlayerStamina=5;

  playerLocation=3;

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
  inventory[9] = &allItems[16]; // give player fire grease at start

  for (int i = 0; i<7; i++)
  {
    tiles[i] = 0;
  }
  enemy1 = (enemy){.attack=2, .health=4, .maxHealth=5, .bitmap=skeleton_Map, .facingLeft=0};
  tiles[0] = &enemy1;
  
  enemy2 = (enemy){.attack=2, .health=3, .maxHealth=5, .bitmap=skeleton_Map, .facingLeft=1};
  tiles[6] = &enemy2;
}

void drawHealthStamina()
{
  u8 yo=0, xo=17; // change these to change where the healthbars are
  u8 x = 0, y = 0;
  u8 hbWidth = (maxPlayerHealth * 3) - 1;

  // health bar
  int screen_y = y + yo;

  // Calculate the VRAM row start address once per row
  volatile u16* vram_row1 = &VRAM[(screen_y) * GBA_SW+ xo];
  volatile u16* vram_row2 = &VRAM[(screen_y + 1) * GBA_SW + xo];
  volatile u16* vram_row3 = &VRAM[(screen_y + 2) * GBA_SW + xo];
  volatile u16* vram_row4 = &VRAM[(screen_y + 3) * GBA_SW + xo];

  for(x = 0; x < hbWidth; x++) 
  {               
      vram_row1[x + 1] = barColors[0];
      vram_row2[x + 1] = barColors[2];
      vram_row3[x + 1] = barColors[2];
      vram_row4[x + 1] = barColors[0];
  }
  for (u8 i = 0; i < playerHealth; i++)
  {
    u8 off = (i * 3) + 1;
    vram_row2[off] = barColors[1];
    vram_row2[off + 1] = barColors[1];
    vram_row3[off] = barColors[1];
    vram_row3[off + 1] = barColors[1];
  }
  // cap off health bar
  vram_row2[0] = barColors[0];
  vram_row3[0] = barColors[0];
  vram_row2[hbWidth + 1] = barColors[0];
  vram_row3[hbWidth + 1] = barColors[0];

  // stamina bar
  u8 sbWidth = (maxPlayerStamina * 3) - 1;
  // Calculate the VRAM row start address once per row
  vram_row1 = &VRAM[(screen_y + 3) * GBA_SW+ xo];
  vram_row2 = &VRAM[(screen_y + 4) * GBA_SW + xo];
  vram_row3 = &VRAM[(screen_y + 5) * GBA_SW + xo];
  vram_row4 = &VRAM[(screen_y + 6) * GBA_SW + xo];

  for(x = 0; x < sbWidth; x++) 
  {                
      vram_row1[x + 1] = barColors[0];
      vram_row2[x + 1] = barColors[4];
      vram_row3[x + 1] = barColors[4];
      vram_row4[x + 1] = barColors[0];
  }

  for (u8 i = 0; i < playerStamina; i++)
  {
    u8 off = (i * 3) + 1;
    vram_row2[off] = barColors[3];
    vram_row2[off + 1] = barColors[3];
    vram_row3[off] = barColors[3];
    vram_row3[off + 1] = barColors[3];
  }
  // cap off stamina bar
  vram_row2[0] = barColors[0];
  vram_row3[0] = barColors[0];
  vram_row2[sbWidth + 1] = barColors[0];
  vram_row3[sbWidth + 1] = barColors[0];
}

void drawEnemyHealth(u8 xo, u8 yo, u8 maxHealth, u8 health)
{
  u8 x = 0, y = 0; // used for moving around the screen
  u8 markers = (health * 11 + maxHealth - 1) / maxHealth;
  if (markers == 11 && health != maxHealth) markers = 10;
  int screen_y = y + yo;

  // Calculate the VRAM row start address once per row
  volatile u16* vram_row1 = &VRAM[(screen_y) * GBA_SW+ xo];
  volatile u16* vram_row2 = &VRAM[(screen_y + 1) * GBA_SW + xo];
  volatile u16* vram_row3 = &VRAM[(screen_y + 2) * GBA_SW + xo];

  for(x = 0; x < 11; x++) 
  {               
      vram_row1[x + 1] = barColors[0];
      vram_row2[x + 1] = barColors[2];
      vram_row3[x + 1] = barColors[0];
  }
  for (u8 i = 0; i < markers; i++)
  {
    u8 off = (i) + 1;
    vram_row2[off] = barColors[1];
  }
  // cap off health bar
  vram_row2[0] = barColors[0];
  vram_row2[12] = barColors[0];
}

void drawEnemies()
{
  for (u8 i = 0; i<7; i++)
  {
    if (tiles[i] != 0) // there is an enemy on current tile
    {
      if (tiles[i]->facingLeft == 0)
      {
        drawImage(16, 16, tileXs[i], 35, tiles[i]->bitmap, 0 ,0); 
      }
      else 
      {
        drawImage(16, 16, tileXs[i]-6, 35, tiles[i]->bitmap, 0 ,1);
      }
      drawEnemyHealth(tileXs[i]-1, 32, tiles[i]->maxHealth, tiles[i]->health);
    }
  }
}

void drawInventory()
{
  for (int i = 0; i < 14; i++)
  {
    if (inventory[i] != 0)
    {
      if (i % 2 == 0) // left column
      {
        drawImage(8, 6, 1, inventoryYs[i], inventory[i]->bitmap, 0,0);
      }
      else // right column
      {
        drawImage(8, 6, 10, inventoryYs[i], inventory[i]->bitmap, 0,0);
      }
    }
  }
}

void displaySelector()
{
  if (fightState==0)
  {
    if (selected % 2 == 0) // left column
    {
      drawImage(8, 8, 0, inventoryYs[selected]-1, invSelector_Map, 0,0);
    }
    else // right column
    {
      drawImage(8, 8, 9, inventoryYs[selected]-1, invSelector_Map, 0,0);
    }
  }
}

void displayRangedSelector()
{
  for (char i = rangedSelection; i<7 && i<rangedSelection+inventory[selected]->useArea; i++)
  {
    drawImage(8,8, tileXs[i] + 3, 53, arrowUp_Map, 0,0);
  }
}

void displayMeleeSelector()
{
  if (meleeSelection == 0) // Attacking Left
  {
    int pLoc = (int)playerLocation;
    int uArea = (int)inventory[selected]->useArea;
    int minTile = pLoc - uArea;
    if (minTile < 0) minTile = 0;

    for (int i = pLoc - 1; i >= minTile; i--)
    {
      drawImage(8, 8, tileXs[i] + 3, 53, arrowUp_Map, 0, 0);
    }
  }
  else // Attacking Right
  {
    int pLoc = (int)playerLocation;
    int uArea = (int)inventory[selected]->useArea;
    int maxTile = pLoc + uArea;
    
    if (maxTile > 6) maxTile = 6;

    for (int i = pLoc + 1; i <= maxTile; i++)
    {
      drawImage(8, 8, tileXs[i] + 3, 53, arrowUp_Map, 0, 0);
    }
  }
}

void endTurn()
{
  // add function here for handling enemy attacks
  currText = "Waiting...";
  fightState=0;
  timeout_timer = 15;
}

void useHealItem()
{
  playerHealth += inventory[selected]->useAmount;
  if (playerHealth >= maxPlayerHealth) {playerHealth=maxPlayerHealth;}
  playerStamina -= inventory[selected]->staminaCost;
  if (inventory[selected]->type==0) {inventory[selected]=0;}
  endTurn();
}

void useBoostItem()
{
  damageBoost += inventory[selected]->useAmount;
  playerStamina -= inventory[selected]->staminaCost;
  if (inventory[selected]->type==0) {inventory[selected]=0;}
  endTurn();
}

void useRangedItem()
{
  for (u8 i = rangedSelection; i<7 && i<rangedSelection+inventory[selected]->useArea; i++)
  {
    if (tiles[i]!=0)
    {
      tiles[i]->health-=inventory[selected]->useAmount;
      if (tiles[i]->health<=0) {tiles[i]=0;}// kill enemy if dead
    }
  }
  playerStamina -= inventory[selected]->staminaCost;
  if (inventory[selected]->type==0) {inventory[selected]=0;}
  endTurn();
}

void useMeleeItem()
{
  short damage = inventory[selected]->useAmount + damageBoost;

  if (meleeSelection == 0) // Attacking Left
  {
    int pLoc = (int)playerLocation;
    int uArea = (int)inventory[selected]->useArea;
    int minTile = pLoc - uArea;
    
    if (minTile < 0) minTile = 0;

    for (int i = pLoc - 1; i >= minTile; i--)
    {
      if (tiles[i] != 0)
      {
        tiles[i]->health -= damage;
        if (tiles[i]->health <= 0) { tiles[i] = 0; }
      }
    }
  }
  else // Attacking Right
  {
    int pLoc = (int)playerLocation;
    int uArea = (int)inventory[selected]->useArea;
    int maxTile = pLoc + uArea;
    
    if (maxTile > 6) maxTile = 6;

    for (int i = pLoc + 1; i <= maxTile; i++)
    {
      if (tiles[i] != 0)
      {
        tiles[i]->health -= damage;
        if (tiles[i]->health <= 0) { tiles[i] = 0; }
      }
    }
  }
  damageBoost = 0;
  playerStamina -= inventory[selected]->staminaCost;
  if (inventory[selected]->type == 0) { inventory[selected] = 0; }
  endTurn();
}

void useDefenceItem()
{
  defence = inventory[selected]->useAmount;
  playerStamina -= inventory[selected]->staminaCost;
  if (inventory[selected]->type==0) {inventory[selected]=0;}
  endTurn();
}

void inventoryControls()
{
  if (selectorTimeout <= 0)
  {
    if(KEY_U && selected > 1){selected-=2; selectorTimeout=selectorSpeed;} // move selector up
    else if(KEY_D && selected < 12){selected+=2; selectorTimeout=selectorSpeed;} // move selector down
    else if(KEY_L && selected % 2 == 1){selected-=1; selectorTimeout=selectorSpeed;} // move selector left
    else if(KEY_R && selected % 2 == 0){selected+=1; selectorTimeout=selectorSpeed;} // move selector right
    else if(KEY_B){fightState=1; selectorTimeout=selectorSpeed;} // switch to move selection
    else if (KEY_A && inventory[selected] != 0)
    {
      if (playerStamina >= inventory[selected]->staminaCost)
      {
        if (inventory[selected]->useType == 0){useHealItem();}
        else if (inventory[selected]->useType == 1){useBoostItem();}
        else if (inventory[selected]->useType == 2){fightState=3;}
        else if (inventory[selected]->useType == 3){fightState=2;}
        else if (inventory[selected]->useType == 4){useDefenceItem();}
        selectorTimeout=3;
      }
      else
      {
        currText = "Not Enough Stamina!";
        timeout_timer=20;
      }
    }
    else if(KEY_ST) // temporary reset button
    {
      init();
    }
  }
  else
  {
    selectorTimeout--;
  }
}

void fightControls()
{
  if (selectorTimeout <= 0)
  {
    if(KEY_B){fightState=0; selectorTimeout=selectorSpeed;} // switch to inventory selection

    else if(KEY_D)
    {
        playerStamina = maxPlayerStamina;
        endTurn();
    }

    else if(KEY_L && playerLocation >= 1 && playerStamina >= 1)
    {
      if (tiles[playerLocation-1] == 0) // tile empty
      {
        playerLocation -= 1;
      }
      else // tile not empty
      {
        tiles[playerLocation] = tiles[playerLocation-1];
        tiles[playerLocation-1] = 0;
        playerLocation -= 1;
      }
      playerStamina -= 1;
      playerFacingLeft = 1;
      endTurn();
    }

    else if(KEY_R && playerLocation<=5 && playerStamina >= 1)
    {
      if (tiles[playerLocation+1] == 0) // tile empty
      {
        playerLocation += 1;
      }
      else // tile not empty
      {
        tiles[playerLocation] = tiles[playerLocation+1];
        tiles[playerLocation+1] = 0;
        playerLocation += 1;
      }
      playerStamina -= 1;
      playerFacingLeft = 0;
      endTurn();
    }
  }
  else
  {
    selectorTimeout--;
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
    drawImage(120,80, 0,0, fightBackground_Map, 0,0); //draw fight background
    drawInventory();
    drawHealthStamina();
    drawTiles();
    if (playerFacingLeft == 0)
    {
      drawImage(16,16, tileXs[playerLocation], 35, solaire_Map,0,0); //player
    }
    else
    {
      drawImage(16,16, tileXs[playerLocation]-6, 35, solaire_Map,0,1);
    }
    
    drawEnemies(); 
    drawText(2,64, currText);

    if (timeout_timer <= 0)
    {
      if (fightState==0) // player in inventory
      {
        displaySelector();
        if (inventory[selected] != 0)
        {
          currText = inventory[selected]->message;
        }
        else
        {
          currText = "";
        }
        inventoryControls();
      }
      else if (fightState==1) // player selecting move
      {
        drawImage(8,8, tileXs[playerLocation] + 3, 53, arrowUp_Map, 0,0);
        currText = "Make a move...";
        fightControls();
      }
      else if (fightState==2) // ranged attack selection
      {
        displayRangedSelector();
        if (selectorTimeout <= 0)
        {
          if (KEY_A) {useRangedItem();selectorTimeout=2;} // use item
          else if (KEY_B) {fightState=0;selectorTimeout=2;} // exit to inventory
          else if (KEY_L && rangedSelection > 0) {rangedSelection-=1; selectorTimeout=2;}
          else if (KEY_R && rangedSelection <= 6-inventory[selected]->useArea) {rangedSelection+=1; selectorTimeout=2;}
          // flip player accordingly
          if (rangedSelection < playerLocation) {playerFacingLeft=1;}
          else {playerFacingLeft=0;}
        }
        else
        {
          selectorTimeout--;
        }
        currText="Select Ranged Attack...";
      }
      else if (fightState==3) // melee attack selection
      {
        displayMeleeSelector();
        if (selectorTimeout <= 0)
        {
          if (KEY_A) {useMeleeItem();selectorTimeout=2;} // use item
          else if (KEY_B) {fightState=0;selectorTimeout=2;} // exit to inventory
          else if (KEY_L) {meleeSelection=0;selectorTimeout=2;}
          else if (KEY_R) {meleeSelection=1;selectorTimeout=2;}
          playerFacingLeft = !meleeSelection; // flip player accordingly
        }
        else
        {
          selectorTimeout--;
        }
        currText="Select Melee Attack...";
      }

    }
    else
    {
      timeout_timer -= 1;
    }
}

void shopLevelUp()
{
}

#include "gba.h"
#include "textures.h"
#include "audioVideo.c"

u16 frameCounter=1;//for frames per second / rng
u8 gameState=0; // title screen=0, game=1, end screen=2
u8 playState=0; // selecting level=0, fighting=1, shop/level up=2
u16 endScreenTimer=0;

void buttons()//buttons to press
{
 if(KEY_R ){ }  //move right
 if(KEY_L ){ }  //move left
 if(KEY_U ){ }  //move up
 if(KEY_D ){ }  //move down
 if(KEY_A ){ } 
 if(KEY_B ){ } 
 if(KEY_LS){ } 
 if(KEY_RS){ } 
 if(KEY_ST){ } 
 if(KEY_SL){ } 
}

void gameUpdates()
{
}

void init()
{
  
}

int main()
{
 *(u16*)0x4000000 = 0x405;
 REG_BG2PA=256/2; //256=normal 128=scale
 REG_BG2PD=256/2; //256=normal 128=scale 
  
 init();                                                             //init game variables

 while(1) 
 { 
    while(*(volatile u16*)0x04000006 < 160);

    if (DISPCNT & BACKB) 
    { 
       DISPCNT &= ~BACKB; VRAM = (u16*)VRAM_B; 
    } else 
    {
       DISPCNT |=  BACKB; VRAM = (u16*)VRAM_F; 
    }

    if(gameState==0)  //title screen
    {  
      playSoundMusic(0,1); //play title song
      // drawImage(120,80, 0,0, title_Map, 0); //draw title screen
      if(KEY_STATE != 0x03FF){ init(); gameState=1;} //any button pressed
    } 

    if(gameState==1)
    {
      playSoundMusic(1,1);
      buttons();
      gameUpdates();
    } 

    if(gameState==2)
    { 
      playSoundMusic(2,0); //play end song once
    }  	
    while(*(volatile u16*)0x04000006 < 160); 
    frameCounter+=1;
    if(frameCounter>1000){ frameCounter=1;}
  }
}

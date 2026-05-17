#include "gba.h"

extern u16 frameCounter; //for frames per second / rng

typedef struct
{
 u16* song;
 int tic;
 int spd;
 int size;
 int onOff;
}SoundMusic; 
SoundMusic M[3];

u16 notes[] = 
{
   44, 157,  263, 363,  458,  547, 631,  711, 786,  856, 923,  986,//C2,C2#, D2,D2#, E2, F2,F2#, G2,G2#, A2,A2#, B2 
 1046,1102, 1155,1205, 1253, 1297,1340, 1379,1417, 1452,1486, 1517,//C3,C3#, D3,D3#, E3, F3,F3#, G3,G3#, A3,A3#, B3 	
 1547,1575, 1602,1627, 1650, 1673,1694, 1714,1732, 1750,1767, 1783,//C4,C4#, D4,D4#, E4, F4,F4#, G4,G4#, A4,A4#, B4
 1798,1812, 1825,1837, 1849, 1860,1871, 1881,1890, 1899,1907, 1915,//C5,C5#, D5,D5#, E5, F5,F5#, G5,G5#, A5,A5#, B5
 1923,1930, 1936,1943, 1949, 1954,1959, 1964,1969, 1974,1978, 1982,//C6,C6#, D6,D6#, E6, F6,F6#, G6,G6#, A6,A6#, B6
 1985,1989, 1992,1995, 1998, 2001,2004, 2006,2009, 2011,2013 ,2015,//C7,C7#, D7,D7#, E7, F7,F7#, G7,G7#, A7,A7#, B7
};

IN_IWRAM void drawImage(int w,int h, int xo,int yo, const u16* map, int to)
{
  int x, y;
  if (yo >= SH || xo >= SW || xo + w <= 0 || yo + h <= 0) return;

  for(y = 0; y < h; y++) 
  {                
      int screen_y = y + yo;
      // Skip rows that are outside the visible game screen bounds
      if (screen_y < 0 || screen_y >= SH) continue;

      // Calculate the VRAM row start address once per row
      volatile u16* vram_row = &VRAM[screen_y * GBA_SW + xo];
      const u16* map_row = &map[(y + to * h) * w];

      for(x = 0; x < w; x++)
      { 
          int screen_x = x + xo;
          if (screen_x >= 0 && screen_x < SW) // Ensure we stay in game screen width
          {
              u16 c = map_row[x]; 
              if(c > 0) // Transparency check
              { 
                  vram_row[x] = c;
              }
          }
      }  
  }
}

void PlayNote( u16 frequency, unsigned char length ){

	unsigned char adjustedLength = 0xFF - length;

	REG_SOUNDCNT_X = 0x80;
	REG_SOUNDCNT_L=0xFF77;

	REG_SOUNDCNT_H = 2;

	REG_SOUND3CNT_L = SOUND3BANK32 | SOUND3SETBANK1;
	REG_WAVE_RAM0=0x10325476;
	REG_WAVE_RAM1=0x98badcfe;
	REG_WAVE_RAM2=0x10325476;
	REG_WAVE_RAM3=0x98badcfe;
	REG_SOUND3CNT_L = SOUND3BANK32 | SOUND3SETBANK0;

	REG_SOUND3CNT_L |= SOUND3PLAY;

	REG_SOUND3CNT_H = SOUND3OUTPUT1 | adjustedLength;
	REG_SOUND3CNT_X=SOUND3INIT|SOUND3PLAYONCE| frequency ;
}

void playSoundMusic(int s, int loop)
{
 if((frameCounter % (M[s].spd))==0 && M[s].onOff==1) 
 {
  int note=M[s].song[M[s].tic];
  if(note>0){ PlayNote(notes[note],64);}
  M[s].tic+=1; if(M[s].tic>M[s].size){ M[s].tic=0; if(loop==0){ M[s].onOff=0;}}
 }
}

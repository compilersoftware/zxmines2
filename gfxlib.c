/*

 Gfx functions - Santiago Romero - NoP/Compiler

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

int scan_convert[192] = {
   0,8,16,24,32,40,48,56,
   1,9,17,25,33,41,49,57,
   2,10,18,26,34,42,50,58,
   3,11,19,27,35,43,51,59,
   4,12,20,28,36,44,52,60,
   5,13,21,29,37,45,53,61,
   6,14,22,30,38,46,54,62,
   7,15,23,31,39,47,55,63,
                   
   64,72,80,88,96,104,112,120,
   65,73,81,89,97,105,113,121,
   66,74,82,90,98,106,114,122,
   67,75,83,91,99,107,115,123,
   68,76,84,92,100,108,116,124,
   69,77,85,93,101,109,117,125,
   70,78,86,94,102,110,118,126,
   71,79,87,95,103,111,119,127,

   128,136,144,152,160,168,176,184,
   129,137,145,153,161,169,177,185,
   130,138,146,154,162,170,178,186,
   131,139,147,155,163,171,179,187,
   132,140,148,156,164,172,180,188,
   133,141,149,157,165,173,181,189,
   134,142,150,158,166,174,182,190,
   135,143,151,159,167,175,183,191
};

#define GetRowAddress(row) (scan_convert[(row)])
#define SCREENSIZE 6912
unsigned char *VRAM;
unsigned char my_tmp_border;
void InitGFX( void );

void ClearScreen( int value );
void ClearScreenAttr( int x, int y, int width, int height, int value );
int DrawSpriteChar( unsigned char x, unsigned char y,  
                    unsigned char *sprite, unsigned char attrib );
void BORDER( unsigned char value );
void BEEP(unsigned int cycles, unsigned int tone);
   
//---------------------------------------------------------------------
// Init GFX stuff...
//---------------------------------------------------------------------
void InitGFX( void )
{
  int x;
  VRAM = 0x4000;
  
  for(x=0; x<192; x++)
    scan_convert[x] = (scan_convert[x] * 32) + (int) VRAM;
}


//---------------------------------------------------------------------
// Draws an 1x1 sprite/character at (x,y) (character coordinates)
//---------------------------------------------------------------------
int DrawSpriteChar( unsigned char x, unsigned char y,  
                     unsigned char *sprite, unsigned char attrib )
{
  int j, sproff;
  unsigned char *memoffset;

  y = y<<3;
  
  /* get the screen offset and draw */
  for( j=0, sproff=0; j<8; j++ )
  {
     memoffset = (unsigned char *) GetRowAddress(y+j) +x;
     *memoffset = sprite[sproff++];
  }

  // now put the attributes on the videomemory
  y = y>>3;
  memoffset = (unsigned char *) (0x4000 + 6144 + (y<<5) + x) ;
  *memoffset = attrib;
  
}


//---------------------------------------------------------------------
// Clear Memory Screen
//---------------------------------------------------------------------
void ClearScreen( int value )
{
#asm
   ld hl, 2
   add hl, sp
   ld a, (hl)
   ld hl, 16384
   ld (hl), a
   ld de, 16385
   ld bc, 6911
   ldir
#endasm
}

//---------------------------------------------------------------------
// Set Border
//---------------------------------------------------------------------
void BORDER( unsigned char value )
{
   my_tmp_border = value<<3;
#asm
   ld hl, 2
   add hl, sp
   ld a, (hl)
   ld c, 254
   out (c), a
   ld hl, 23624
   ld a, (_my_tmp_border)
   ld (hl), a
#endasm
}


//---------------------------------------------------------------------
void BEEP(unsigned int cycles, unsigned int tone)
{
#asm
   pop bc      ; bc = return address
   pop hl      ; hl = tone
   pop de      ; de = cycles
   push de
   push hl
   push bc
   jp 0x03b5   ; jump to ROM beep
#endasm
}


//---------------------------------------------------------------------
// Clear Memory Screen attributes
//---------------------------------------------------------------------
void ClearScreenAttrib( unsigned char *dest, int color )
{
  int x;
  unsigned char *pointer;

  pointer = (unsigned char *) VRAM + 6144;
  for( x=0; x<768; x++ )
     *pointer++ = color;
}




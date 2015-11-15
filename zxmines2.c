/*---------------------------------------------------------------------

   ZXMines2  -> C Version of the Minesweeper I created for
                Radastan's BASIC competition (only pure BASIC).
   v 2.0         (c) 2003 Santiago Romero AkA NoP / Compiler
                        sromero@escomposlinux.org
 
 Feel free to improve this game, solve bugs, and add whatever
 you want. It's free software, you can do it without asking me,
 but it would be very nice if you maintain my name in the
 credits.

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

---------------------------------------------------------------------*/
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"
#include "graphics.h"

#include "gfxlib.c"

#define xinit  1
#define yinit  1
#define levels 3

int  size[levels]  = { 8,  12, 15 };
int  mines[levels] = { 8, 18, 30 };
char A[12]     = { 0+56, 1+56, 3+56, 2+56, 2+56, 2+56, 2+56, 2+56, 2+56, 0+56, 0+56, 8 };

#define  board_width    16
#define  board_height   16
#define  board_size     256

unsigned char board  [ board_size+1 ];
unsigned char viewed [ board_size+1 ];
unsigned char mines_x[ board_size + 1 ];
unsigned char mines_y[ board_size + 1 ];
int to_open;

// Optimize the (y*board_width) multiplication
#define YBYWIDTH(y)  ((y)<<4)

#define T( x, y )       \
   (board[YBYWIDTH(y)+(x)])

#define V( x, y )       \
   (viewed[YBYWIDTH(y)+(x)])


// Game graphics (1 char)
char gfx[12*8] = {
// 0: Empty Cell, followed by numbers 1-8
0, 1, 0, 17, 16, 1, 0, 170,
0, 24, 40, 8, 8, 8, 62, 0,
0, 60, 66, 2, 60, 64, 126, 0,
0, 60, 66, 12, 2, 66, 60, 0,
0, 8, 24, 40, 72, 126, 8, 0,
0, 126, 64, 124, 2, 66, 60, 0, 
0, 60, 64, 124, 66, 66, 60, 0,
0, 126, 2, 4, 8, 16, 16, 0,
0, 60, 66, 60, 66, 66, 60, 0,
// 9: Mine
16, 186, 116, 254, 124, 186, 16, 0,
// 10: Board cell
1, 1, 1, 1, 1, 1, 1, 255,
// 11: empty cell
0, 0, 0, 0, 0, 0, 0, 0
};


#define CLICK()    BEEP(50, 10);
#define OPEN_SND() BEEP(25, 3);
#define DIE_SND()  BEEP(1000, 20);

#define WaitKey() getkey()

//---------------------------------------------------------------------
#define TextDefault()       printf( "\x1B[m" )
#define TextHiRes()         printf( "%c2J",155 )
#define TextBold()          printf( "\x1B[1m" )
#define TextDim()           printf( "\x1B[2m" )
#define TextUnderline()     printf( "\x1B[4m" )
#define TextUnunderline()   printf( "\x1B[24m" )
#define TextReverse()       printf( "\x1B[7m" )
#define TextUnreverse()     printf( "\x1B[27m" )
#define TextFG_Black()      printf( "\x1B[%um", 30 )
#define TextFG_Red()        printf( "\x1B[%um", 31 )
#define TextFG_Green()      printf( "\x1B[%um", 32 )
#define TextFG_Yellow()     printf( "\x1B[%um", 33 )
#define TextFG_Blue()       printf( "\x1B[%um", 34 )
#define TextFG_Magenta()    printf( "\x1B[%um", 35 )
#define TextFG_Cyan()       printf( "\x1B[%um", 36 )
#define TextFG_White()      printf( "\x1B[%um", 37 )
#define TextBG_Black()      printf( "\x1B[%um", 40 )
#define TextBG_Red()        printf( "\x1B[%um", 41 )
#define TextBG_Green()      printf( "\x1B[%um", 42 )
#define TextBG_Yellow()     printf( "\x1B[%um", 43 )
#define TextBG_Blue()       printf( "\x1B[%um", 44 )
#define TextBG_Magenta()    printf( "\x1B[%um", 45 )
#define TextBG_Cyan()       printf( "\x1B[%um", 46 )
#define TextBG_White()      printf( "\x1B[%um", 47 )
#define TextColor(x)        printf( "\x1B[%um", (x) )
#define TextGotoXY(x,y)     printf( "\x1B[%u;%uH",(y),(x))
#define TextClearScreen()   printf( "\x1B[2J" )
#define getakey() getchar()


int key;
int kup, kleft, kright, kdown, kfire;
int level = 0, score=0;
int language=0;

/*
 Spectrum COLORS
-----------------
   0  black
   1  blue
   2  red
   3  magenta
   4  green
   5  cyan
   6  yellow
   7  white
*/

int sleep(int secs);
int ticksleep(int ticks);
void Startup( int game );
void UpdateScore( int change_level );
int Game( void );
void DrawMenu( int newgame );
void GenerateBoard( void );
void DrawBoard( void );
void ClearBoard( void );
void ClearViewed( void );
void DiscoverCell( unsigned int x, unsigned int y );
void Die( void );
void Win( void );
void ClearGFXBoard( void );


#define DrawChar(x,y,c,a) \
DrawSpriteChar((x), (y), (unsigned char *) gfx+((c)<<3), (a) )

#define border 0
#define paper  1
#define ink    6

//--- Main function ---------------------------------------------------
int main( void )
{
   unsigned char done=0;
   int ret;

   Startup( 1 );

   DrawMenu(1);
   ret = 0;
   while(!done)
   {
      if( ret != 0 ) { key = ret; }
      else           { key = WaitKey(); }
      switch( key )
      {
         case '1':
            CLICK();
            level = score = 0;
            GenerateBoard();
            ret = Game();
            DrawMenu(0);
            break;
         case '2':
            CLICK();
            level = 1; score = 0;
            GenerateBoard();
            ret = Game();
            DrawMenu(0);
            break;
         case '3':
            CLICK();
            level = 2; score = 0;
            GenerateBoard();
            ret = Game();
            DrawMenu(0);
            break;
         case '4':
            CLICK();
            language = 1-language;
            DrawMenu(0);
            break;

      } // end switch

   } // end while

}


//---------------------------------------------------------------------
// Game loop function
//---------------------------------------------------------------------
int Game( void )
{
   unsigned int cx, cy, nmines, oldx, oldy;
   unsigned int width, height;
   unsigned char done, changed;

   width = height = size[level];
   nmines = mines[level];
   
   cx = cy = (width/2)-1;
   
   done = 0;
   DrawBoard();
   UpdateScore( 0 );
 
   while(!done)  // game loop
   {

      if( V(cx,cy) )  {  DrawChar(xinit+cx, yinit+cy, T(cx,cy), A[T(cx,cy)]+128); }
      else            {  DrawChar(xinit+cx, yinit+cy, 10, A[10]+128); }

      changed = 0;
      oldx = cx; oldy = cy;
      
      // check keyboard
      key = WaitKey();
      if( key == 'o' || key == 'O' || key == '6' )       kleft=1;
      else if( key == 'p' || key == 'P' || key == '7' )  kright=1;
      else if( key == 'a' || key == 'A' || key == '8' )  kdown=1;
      else if( key == 'q' || key == 'Q' || key == '9' )  kup=1;
      else if( key == ' ' || key == '0' )                kfire=1;
      else if( key == '1')  { UpdateScore(2); return('1'); }
      else if( key == '2')  { UpdateScore(2); return('2'); }
      else if( key == '3')  { UpdateScore(2); return('3'); }
      else if( key == '4')  { language=1-language; DrawMenu(0); }
      
      // if user pressed a valid key, act:
      if( kleft )
      {
         if( cx>0 ) { cx--; changed = 1; }
         key = kleft = kfire = 0;
      }
      
      if( kright )
      {
         if( cx<width-1 ) { cx++; changed = 1; }
         key = kright = kfire = 0;
      }

      if( kup )
      {
         if( cy>0 ) { cy--; changed = 1; }
         key = kup = kfire = 0;
      }
      
      if( kdown )
      {
         if( cy<height-1 ) { cy++; changed = 1; }
         key = kdown = kfire = 0;
      }
      if( kfire && V(cx,cy)==0)
      {
         kfire = 0;
         // discover the cell under the cursor
         OPEN_SND();
         DiscoverCell( cx, cy );
         UpdateScore(0);
         if( T(cx,cy) == 9 ) 
         {
            Die();
            done = 1;
            changed = 0;
            to_open = -1;
         }
         key = kfire = 0;

         // if there are not more cells to open (==mines), we won
         if( to_open == nmines  )
         {
            Win();
            done = 1;
         }
      }

      // something changed, redraw the cursor
      if( changed )
      {
         // restore the contents of the old cell
         if( V(oldx,oldy) )  
         {  DrawChar(xinit+oldx, yinit+oldy, T(oldx,oldy), A[T(oldx,oldy)]); }
         else                
         {  DrawChar(xinit+oldx, yinit+oldy, 10, A[10]); }
      }
     
   }
   UpdateScore(2);
   return(0);
  
}

//---------------------------------------------------------------------
// We found a mine
//---------------------------------------------------------------------
void Die( void )
{
   int n;

   TextFG_White();
   TextGotoXY(14, 21);    printf("GAME OVER! "); 
   TextGotoXY(14, 22);
   if( language == 0)          printf("Score  : %d\n", score );
   else if( language == 1)     printf("Puntos : %d\n", score );

   DIE_SND();
   for( n=0; n<5; n++)
   {
      BORDER(6);
      ticksleep(1);
      BORDER(2);
      ticksleep(1);
      BORDER(6);
   }
   BORDER(border);

   TextGotoXY(22,4);
   if( language==0 )      printf("  PRESS     ");
   else if( language==1 ) printf("  PULSA     ");
   TextGotoXY(22,5);
   if( language==0 )      printf("  SPACE       ");
   else if( language==1 ) printf("  LA TECLA    ");
   TextGotoXY(22,6);
   if( language==0 )      printf("  KEY...    ");
   else if( language==1 ) printf("  ESPACIO   ");
   TextGotoXY(22,7);
   if( language==0 )      printf("          ");
   else if( language==1 ) printf("          ");
 
   for( n=0; n<board_size; n++ )
   { viewed[n] = 1; }
   DrawBoard();

   while( WaitKey() != ' ' ) ;
   TextFG_Yellow();
   TextGotoXY(14, 21);    printf("           "); 
   TextGotoXY(14, 22);    printf("             \n");
}



//---------------------------------------------------------------------
// We won the match! :)
//---------------------------------------------------------------------
void Win( void )
{
   int n;

   TextFG_White();
   TextGotoXY(13, 21);    
   if( language == 0 ) printf("YOU WIN! "); 
   else if( language == 0 ) printf("GANASTE! "); 
   TextGotoXY(13, 22);
   if( language == 0)          printf("Score  : %d\n", score );
   else if( language == 1)     printf("Puntos : %d\n", score );

   DIE_SND();
   for( n=0; n<10; n++)
   {
      BORDER(6);
      ticksleep(3);
      BORDER(2);
      ticksleep(3);
      BORDER(6);
   }
   BORDER(border);
   
   TextGotoXY(22,4);
   if( language==0 )      printf("  PRESS     ");
   else if( language==1 ) printf("  PULSA     ");
   TextGotoXY(22,5);
   if( language==0 )      printf("  ANY         ");
   else if( language==1 ) printf("  UNA         ");
   TextGotoXY(22,6);
   if( language==0 )      printf("  KEY...    ");
   else if( language==1 ) printf("  TECLA...  ");
   TextGotoXY(22,7);
   if( language==0 )      printf("           ");
   else if( language==1 ) printf("           ");
 
   for( n=0; n<board_size; n++ )
   { viewed[n] = 1; }
   DrawBoard();

   while( WaitKey() != ' ' ) ;
   TextFG_Yellow();
   TextGotoXY(13, 21);    printf("        "); 
   TextGotoXY(13, 22);    printf("             \n");
}


//---------------------------------------------------------------------
// Clear the T array (board)
//---------------------------------------------------------------------
void ClearBoard( void )
{
   int n;
   for( n=0; n<board_size; n++ )
   { board[n] = 0; }
}


//---------------------------------------------------------------------
// Clear the V array (viewed)
//---------------------------------------------------------------------
void ClearViewed( void )
{
   int n;
   for( n=0; n<board_size; n++ )
   { viewed[n] = 0; }
}


//---------------------------------------------------------------------
// Generate the minefield, placing mines and calculating distances
//---------------------------------------------------------------------
void GenerateBoard( void )
{

   int x, y, n, nmines, mcenter;
   int width, height;
   char done;

   width = height = size[level];
   mcenter = (width/2)-1;
   to_open = width * height;
   nmines = mines[level];

   ClearBoard();
   ClearViewed();
   done = n = 0;

   // First obtain N pair of coordinates for our mines
   while( !done )
   {
      x = (unsigned char) (abs(rand()) % width);
      y = (unsigned char) (abs(rand()) % height);
      if( x!=mcenter && y!=mcenter && V(x,y) == 0 )
      {
         mines_x[n] = x;
         mines_y[n] = y;
         V(x,y) = 1;
         n++;
      }
      if( n >= nmines ) done=1;
   }

   // Now calculate neightbour distances to mines
   for( n=0; n<nmines; n++ )
   {
      x = mines_x[n];
      y = mines_y[n];
      if( x>0 )
      {
                              T(x-1,y)   = T(x-1,y)+1;
         if( y>0 )          { T(x-1,y-1) = T(x-1,y-1)+1 ; }
         if( y<height-1 )   { T(x-1,y+1) = T(x-1,y+1)+1 ; }
      }
      if( x<width-1 )
      {
                              T(x+1,y)   = T(x+1,y)+1;
         if( y>0 )          { T(x+1,y-1) = T(x+1,y-1)+1 ; }
         if( y<height-1 )   { T(x+1,y+1) = T(x+1,y+1)+1 ; }
      }
      if( y<height-1 )      { T(x,y+1)   = T(x,y+1)+1 ; }
      if( y>0 )             { T(x,y-1)   = T(x,y-1)+1 ; }
   }
   
   // now set the mines in the board itself
   for( n=0; n<nmines; n++ )
   {
      T(mines_x[n],mines_y[n]) = 9;
   }
   ClearViewed();
}


//---------------------------------------------------------------------
// Draw the board on the screen
//---------------------------------------------------------------------
void DrawBoard( void )
{
   int x, y;
   int width, height;
     
   width = height = size[level];
   for( y=0; y<height; y++ )
   {
     for( x=0; x<width; x++ )
     {
        if( V(x,y) )  {  DrawChar(xinit+x, yinit+y, T(x,y), A[T(x,y)]); }
        else          {  DrawChar(xinit+x, yinit+y, 10, A[10]); }
     }
   }
}


//---------------------------------------------------------------------
// Discover the current cell. If the cell is an empty cell, discover
// all empty cells neightbour of the current one.
// --------------------------------------------------------------------
void DiscoverCell( unsigned int x, unsigned int y )
{
   char item;
   int width, height;

   if( V(x,y) ) return;
   width = height = size[level];
   if( x<0 || y<0 || x>=width || y>=height ) return;

   // Open the cell
   item = T(x,y);
   to_open--;
   V(x,y)=1;
   score++;
 
   // draw the cursor in the new cell
   if( V(x,y) )    {  DrawChar(xinit+x, yinit+y, T(x,y), A[T(x,y)]); }
   else            {  DrawChar(xinit+x, yinit+y, 10, A[10]+128); }

   if( item != 0 ) return;

   // if it's an empty cell, open all the empty area
   if( item == 0 )
   {
      if( x>0 )
      {
         DiscoverCell(x-1,y);
         if( y>0 ) DiscoverCell(x-1,y-1);
         if( y<height-1 ) DiscoverCell(x-1,y+1);
      }
      if( x<width-1 )
      {
         DiscoverCell(x+1,y);
         if( y>0 ) DiscoverCell(x+1,y-1);
         if( y<height-1 ) DiscoverCell(x+1,y+1);
      }
      if( y<height-1 ) DiscoverCell(x,y+1);
      if( y>0 ) DiscoverCell(x,y-1);
   }
}


//---------------------------------------------------------------------
// Draw the game menu
//---------------------------------------------------------------------
void DrawMenu( int newgame )
{
   
   TextFG_Yellow();
   TextBG_Blue();

   // New game, draw all
   if( newgame )
   {
      ClearScreen( 0 );
      ClearScreenAttrib( VRAM, (paper<<3)+ink );
      BORDER(border);
      draw( 0, 0, 255, 0 );
      draw( 0, 0, 0, 191 );
      draw( 255, 0, 255, 191 );
      draw( 0, 191, 255, 191 );
      draw( 0, 161, 255, 161 );
      TextGotoXY(23,2);

      TextBG_Yellow();
      TextFG_Blue();
      printf("ZXMINES 2.0 ");
 
      TextBG_Blue();
      TextFG_White();
      TextGotoXY(22, 17);    printf("(c) 2003 NoP");
      TextGotoXY(22, 18);    printf("www.speccy.org "); 
   
      TextGotoXY(4, 22);    
      printf("http://www.speccy.org/compiler "); 

      TextFG_Yellow();
      TextGotoXY(22, 9);     printf("Keys/Joy:");
      TextGotoXY(22, 11);    printf("O,P,Q,A,SPACE");
      TextGotoXY(22, 12);    printf("6,7,9,8,0");
      TextGotoXY(22, 13);    printf("<,>,^,v,FIRE");

   }
  
   TextGotoXY(22,4);
   if( language==0 )      printf("1. Easy game");
   else if( language==1 ) printf("1. Facil    ");
   TextGotoXY(22,5);
   if( language==0 )      printf("2. Normal game");
   else if( language==1 ) printf("2. Normal     ");
   TextGotoXY(22,6);
   if( language==0 )      printf("3. Hard game");
   else if( language==1 ) printf("3. Dificil  ");
   TextGotoXY(22,7);
   if( language==0 )      printf("4. Spanish");
   else if( language==1 ) printf("4. English");
   
}


//---------------------------------------------------------------------
// Update the Cells, Mines and Scores
//---------------------------------------------------------------------
void UpdateScore( int change_level )
{
   int x, y;
   
   TextGotoXY(4, 22);    
   printf("                               "); 
   if( change_level >= 1 )
   {
      TextGotoXY(2, 17);
      printf("              ");
      TextGotoXY(2, 18);
      printf("              ");
      if( change_level == 1 ) return;
   }
   else if( change_level == 0 )
   {
      TextFG_White();
      TextGotoXY(2, 17);
      if( language == 0 )
      {
         printf("Mines : %d ", mines[level]);
         TextGotoXY(2, 18);
         printf("Cells : %d ", to_open );
      }
      else if( language == 1 )
      {
         printf("Minas : %d ", mines[level]);
         TextGotoXY(2, 18);
         printf("Celdas: %d ", to_open );
      }
      TextFG_Yellow();
   }

   // Clear the board :)
   if( change_level == 2 )
   {
     ClearGFXBoard();
     TextGotoXY(4, 22);    
     TextFG_White();
     printf("http://www.speccy.org/compiler "); 
   }
   
}


//---------------------------------------------------------------------
// Initialize variables
// game = 1 -> Startup the game itself
// game = 0 -> Startup just a new game
//
//---------------------------------------------------------------------
void Startup( int game )
{
   int y;

   // init randomize numbers seed
   srand(clock());
  
   // clear some variables
   level = 0;
   score = 0;
   kup = kdown = kleft = kright = kfire = 0;

   // First game, clear the screen
   if( game )
   {
      InitGFX();
      ClearScreenAttrib( VRAM, 0 );
   }
}


//---------------------------------------------------------------------
// Clear the board area 
//---------------------------------------------------------------------
void ClearGFXBoard( void )
{
  int y;
  
  for(y=yinit; y<yinit+size[level]; y++ )
  {
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+1)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+2)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+3)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+4)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+5)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+6)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+7)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+8)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+9)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+10)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+11)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+12)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+13)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+14)) = 9; 
     * ((unsigned char *) (0x4000 + 6144 + (y<<5) + xinit+15)) = 9; 
  }

}


//---------------------------------------------------------------------
// Pause execution (sleep) for N seconds
//---------------------------------------------------------------------
int sleep(int secs)
{
   long start = clock();
   secs *= CLOCKS_PER_SEC;
   while ( (clock() - start) < secs )
             ;
   return 0;
}


//---------------------------------------------------------------------
// Pause execution (sleep) for N clock ticks (where 50=1 second).
//---------------------------------------------------------------------
int ticksleep(int ticks)
{
   long start = clock();
   while ( (clock() - start) < ticks )
             ;
   return 0;
}


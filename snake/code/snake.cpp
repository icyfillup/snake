#include "snake_platform.h"

internal void RenderStuff(game_screen_buffer *Buffer, 
                          uint8 Red, uint8 Green, uint8 Blue)
{
    uint8 *Row = (uint8 *)Buffer->Memory;
    
    for(uint32 Y = 0; Y < Buffer->Height; Y++)
    {
        uint32 *Pixel = (uint32 *)Row;
        for(uint32 X = 0; X < Buffer->Width; X++)
        {
            *Pixel++ = (Red << 16) | (Green << 8) | Blue;
        }
        Row += Buffer->Pitch;
    }
}

global_variable int32 WorldWidth = 20;
global_variable int32 WorldHeight = 20;

struct game_state
{
    int32 BlockSize;
    int32 *WorldBuffer;
    
};

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    game_state *GameState = (game_state *)Memory;
    if(!Memory->IsInit)
    {
        GameState->BlockSize = 20;
        Memory->IsInit = true;
    }
    
    uint8 World[20 * 20];
    for(int32 i = 0; i < WorldHeight; i++)
    {
        for(int32 j = 0; j < WorldWidth; j++)
        {
            if(i % 2 == 0)
            {
                World[(WorldHeight * i) + j] = 'A';   
            }
            else
            {
                World[(WorldHeight * i) + j] = 'B';
            }             
        }
    }
    
    Assert("hello");
    
    //RenderStuff(Buffer, Color.Red, Color.Green, Color.Blue);
}


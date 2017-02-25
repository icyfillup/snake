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

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    
    local_persist int32 BlockSize = 36;
    
    uint8 World[20 * 20];
    for(int32 i = 0; i < WorldHeight; i++)
    {
        for(int32 j = 0; j < WorldWidth; j++)
        {
            if(i % 2 == 0)
            {
                World[(WorldWidth * i) + j] = 'A';   
            }
            else
            {
                World[(WorldWidth * i) + j] = 'B';
            }             
        }
    }
    
    for(uint8 RealY = 0; RealY < WorldHeight; RealY++)
    {
        for(uint8 RealX = 0; RealX < WorldWidth; RealX++)
        {
            uint8 chars = World[(RealY * WorldWidth) + RealX];
            uint32 Color;
            if(chars == 'A')
            {
                Color = (255 << 16);
            }
            else
            {
                Color = 255;
            }
            
            uint32 XOffset = BlockSize * RealX;
            uint32 YOffset = BlockSize * RealY;
            
            uint8 *Row = Buffer->Memory;
            for(int y = 0; y < BlockSize; y++)
            {
                uint32 *Pixel = (uint32 *)Row + (YOffset * Buffer->Width) + XOffset;
                for(int x = 0; x < BlockSize; x++)
                {
                    *Pixel++ = Color;
                }
                Row += Buffer->Pitch;
            }
        }
    }
    
    Assert("This line is using for only the debugger");
    
    //RenderStuff(Buffer, Color.Red, Color.Green, Color.Blue);
}


























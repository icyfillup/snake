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

    local_persist int32 BlockSize = 20;
    
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
#if 0
    uint8 *Row = Buffer->Memory;
    for(uint32 Y = 0; Y < Buffer->Height; Y++)
    {
        int32 YOffSet = (int32)(Y / BlockSize);
        uint32 *Pixel = (uint32 *)Row;
        for(uint32 X = 0; X < Buffer->Width; X++)
        {
            int32 XOffSet = (int32)(X / BlockSize);
            
            uint8 chars = World[(WorldHeight * YOffSet) + XOffSet];
            if(chars == 'A')
            {
                *Pixel = (255 << 16);
            }
            else if(chars == 'B')
            {
                *Pixel = 255;
            }
            Pixel++;
        }
        Row += Buffer->Pitch;
    }
#else
    uint8 *Row = Buffer->Memory;
    for(uint32 Y = 0; Y < BlockSize; Y++)
    {
        uint32 *Pixel = (uint32 *)Row;
        for(uint32 X = 0; X < BlockSize; X++)
        {
            *Pixel = (255 << 16);
        }
    }
#endif
    Assert("This line is using for only the debugger");
    
    //RenderStuff(Buffer, Color.Red, Color.Green, Color.Blue);
}

























#include "snake_platform.h"
#include "snake.h"

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

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    local_persist uint8 GAME_WALL = 'w';
    local_persist uint8 GAME_SPACE = 's';
    
    game_state *GameState = (game_state *) Memory->Storage;
    
    //PushStruct(game_state);
    
    if(!Memory->IsInit)
    {
        InitMemoryArrangement(&(GameState->MemoryArrangement), 
                              (uint8 *)Memory->Storage + sizeof(game_state), 
                              Memory->StorageSize - sizeof(game_state));
        
        GameState->World = PushStruct(&GameState->MemoryArrangement, world);
        GameState->World->Height = 21;
        GameState->World->Width = 38;
        GameState->World->BlockSize = 33;

        GameState->World->TileMap = 
            PushArray(&GameState->MemoryArrangement, 
                      GameState->World->Height * GameState->World->Width, 
                      uint8);
        
        Memory->IsInit = true;
    }
    
    memory_index Used = GameState->MemoryArrangement.Used;
    
    uint8 *World = GameState->World->TileMap;
    for(int32 i = 0; i < GameState->World->Height; i++)
    {
        for(int32 j = 0; j < GameState->World->Width; j++)
        {
            if(i == 0 || i == GameState->World->Height - 1 ||
               j == 0 || j == GameState->World->Width - 1)
            {
                World[(GameState->World->Width * i) + j] = 'w';   
            }
            else
            {
                World[(GameState->World->Width * i) + j] = 's';
            }             
        }
    }
    
    for(uint8 RealY = 0; RealY < GameState->World->Height; RealY++)
    {
        for(uint8 RealX = 0; RealX < GameState->World->Width; RealX++)
        {
            uint8 chars = World[(RealY * GameState->World->Width) + RealX];
            uint32 Color;
            
            if(chars == 'w')
            {
                Color = 255;
            }
            else if(chars == 's')
            {
                Color = (255 << 16) | (255 << 8) | 255;
            }
            else
            {
                Color = (255 << 16);
            }
            
            uint32 XOffset = GameState->World->BlockSize * RealX;
            uint32 YOffset = GameState->World->BlockSize * RealY;
            
            uint8 *Row = Buffer->Memory;
            for(int y = 0; y < GameState->World->BlockSize; y++)
            {
                uint32 *Pixel = (uint32 *)Row + (YOffset * Buffer->Width) + XOffset;
                for(int x = 0; x < GameState->World->BlockSize; x++)
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


























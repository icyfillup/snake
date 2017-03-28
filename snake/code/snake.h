#if !defined(SNAKE_H)

struct memory_arrangement
{
    memory_index Used;
    memory_index Size;
    uint8 *Base;
};

struct world
{
    int32 Width;
    int32 Height;
    int32 BlockSize;
};

struct game_state
{
    world *World;
    
    /*
    int32 BlockSize;
    int32 WorldWidth;
    int32 WorldHeight;
    */
    
    memory_arrangement MemoryArrangement; 
};

void * PushSize_(memory_arrangement *MemoryArrangement)
{
    
    return Result;
}

void InitMemoryArrangement(memory_arrangement *MemoryArrangement, 
                           uint8 *Base, 
                           memory_index Size)
{
    MemoryArrangement->Base = Base;
    MemoryArrangement->Size = Size;
    MemoryArrangement->Used = 0;
}

#define SNAKE_H
#endif
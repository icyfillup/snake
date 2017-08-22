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
    
    uint8 *TileMap;
};

struct game_state
{
    world *World;
        
    int32 BlockSize;
    int32 WorldWidth;
    int32 WorldHeight;
    
    memory_arrangement MemoryArrangement; 
};


#define PushStruct(MemoryArrangement, type) (type *)PushSize_(MemoryArrangement, sizeof(type))
#define PushArray(MemoryArrangement, Count, type) (type *)PushSize_(MemoryArrangement, Count * sizeof(type))
void * PushSize_(memory_arrangement *MemoryArrangement, memory_index Size)
{
    Assert((MemoryArrangement->Used + Size) <= MemoryArrangement->Size);
    
    void *Result = MemoryArrangement->Base + MemoryArrangement->Used;
    MemoryArrangement->Used = MemoryArrangement->Used + Size;
    
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
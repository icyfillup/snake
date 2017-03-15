#if !defined(SNAKE_H)

struct memory_arrangement
{
    uint8 *Base;
    memory_index Used;
    memory_index Size;
};

struct game_state
{
    memory_arrangement MemoryArrangement; 
    
    int32 WorldWidth;
    int32 WorldHeight;
    
    int32 BlockSize;
};

void InitMemoryArrangement(memory_arrangement *MemoryArrangement, uint8 *Base, memory_index Size)
{
    MemoryArrangement->Base = Base;
    MemoryArrangement->Size = Size;
    MemoryArrangement->Used = 0;
}

#define SNAKE_H
#endif
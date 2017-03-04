#if !defined(SNAKE_H)

struct memory_arrangement
{
    memory_index *Base;
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

void InitMemoryArrangement()
{

}

#define SNAKE_H
#endif
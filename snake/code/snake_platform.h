#if !defined(SNAKE_PLATFORM_H)

#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#define global_variable static
#define internal static
#define local_persist static

#define ArrayCount(Array) (sizeof((Array)) / sizeof(((Array)[0])))

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) ((Kilobytes(Value)) * 1024LL)
#define Gigabytes(Value) ((Megabytes(Value)) * 1024LL)
#define Terabytes(Value) ((Gigabytes(Value))* 1024LL)

struct game_screen_buffer
{
    uint8 *Memory;
    uint32 Width;
    uint32 Height;
    uint32 Pitch;
    uint32 BytesPerPixel;
};

struct game_button_state
{
    bool32 EndedDown;
};

struct game_controller
{
    union
    {
        game_button_state Buttons[5];
        struct
        {
            game_button_state Up;
            game_button_state Right;
            game_button_state Down;
            game_button_state Left;
            
            game_button_state Start;
        };
    };
};

struct game_memory
{
    bool32 IsInit;
    
    uint64 StorageSize;
    void *Storage;
};

#define GAME_UPDATE_AND_RENDER(name) void name(game_screen_buffer *Buffer, game_memory *Memory, game_controller *Controller)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{

}

#define SNAKE_PLATFORM_H
#endif
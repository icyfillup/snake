#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include "snake_platform.h"

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
    
    int OpenGLHandle;
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

struct win32_game_mode
{
    HMODULE GameCodeDLL;
    FILETIME LastWriteTime;
    
    game_update_and_render *UpdateAndRender;
    
    bool32 IsValid;
};

global_variable bool GlobalRunning;
global_variable GLuint GlobalBlitTextureHandle;
global_variable win32_offscreen_buffer GlobalBackBuffer;
global_variable int64 GlobalPerfCountFrequency;

win32_window_dimension Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    
    return Result;
}

internal void Win32DisplayBufferInWindow(HDC DeviceContext, 
                                         win32_offscreen_buffer *Buffer, 
                                         int WindowWidth, int WindowHeight)
{
#if 1
    StretchDIBits(DeviceContext, 
                  0, 0, WindowWidth, WindowHeight, 
                  0, 0, Buffer->Width, Buffer->Height, 
                  Buffer->Memory, &Buffer->Info, 
                  DIB_RGB_COLORS, SRCCOPY);
#else
    glViewport(0, 0, WindowWidth, WindowHeight);
    
#if 0
    // NOTE(Philip): did not work with RenderStuff()
    // check again later
    if(Buffer->OpenGLHandle)
    {
        glBindTexture(GL_TEXTURE_2D, Buffer->OpenGLHandle);
    }
    else
    {
        Buffer->OpenGLHandle = ++GlobalBlitTextureHandle;
        glBindTexture(GL_TEXTURE_2D, Buffer->OpenGLHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 
                     Buffer->Width, Buffer->Height, 0, 
                     GL_BGRA_EXT, GL_UNSIGNED_BYTE, Buffer->Memory);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
#else
    
    glBindTexture(GL_TEXTURE_2D, GlobalBlitTextureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 
                 Buffer->Width, Buffer->Height, 0, 
                 GL_BGRA_EXT, GL_UNSIGNED_BYTE, Buffer->Memory);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif
    glEnable(GL_TEXTURE_2D);
    
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glMatrixMode(GL_PROJECTION);
    real32 a = 2.0f / (real32)Buffer->Width;
    real32 b = 2.0f / (real32)Buffer->Height;
    
    real32 Proj[] = 
    {
         a,  0,  0,  0,
         0,  b,  0,  0,
         0,  0,  0,  0,
        -1, -1,  0,  1,
    };
    
    glLoadMatrixf(Proj);
    
    glBegin(GL_TRIANGLES);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    real32 P = 0.9f;
    
    //lower
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0, 0);
    
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f((real32)Buffer->Width, 0);
    
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f((real32)Buffer->Width, (real32)Buffer->Height);
    
    //upper
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0, 0);
    
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f((real32)Buffer->Width, (real32)Buffer->Height);
    
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0, (real32)Buffer->Height);
    
    glEnd();
    
    SwapBuffers(DeviceContext);
#endif
}

internal void Win32InitOffScreenBuffer(win32_offscreen_buffer *Buffer, 
                                       int Width, int Height)
{
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = Buffer->BytesPerPixel * Buffer->Width;
    
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    
    int BitmapMemorySize = (Buffer->Width * Buffer->Height) * Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void Win32InitOpenGL(HWND Window)
{
    HDC WindowDC = GetDC(Window);

    PIXELFORMATDESCRIPTOR DesiredPixelFormat = {0};
    DesiredPixelFormat.nSize = sizeof(DesiredPixelFormat);
    DesiredPixelFormat.nVersion = 1;
    DesiredPixelFormat.dwFlags = 
        PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    DesiredPixelFormat.cColorBits = 32;
    DesiredPixelFormat.cAlphaBits = 8;
    DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
    
    int SuggestedPixelFormatIndex = ChoosePixelFormat(WindowDC, &DesiredPixelFormat);
    PIXELFORMATDESCRIPTOR SuggestedPixelFormat = {0};
    DescribePixelFormat(WindowDC, 
                        SuggestedPixelFormatIndex, 
                        sizeof(SuggestedPixelFormat), 
                        &SuggestedPixelFormat);
    SetPixelFormat(WindowDC, SuggestedPixelFormatIndex, &SuggestedPixelFormat);
    
    HGLRC OpenGLRC = wglCreateContext(WindowDC);
    if(wglMakeCurrent(WindowDC, OpenGLRC))
    {
        glGenTextures(1, &GlobalBlitTextureHandle);
    }
    else
    {
        // cant go here
    }
    
    ReleaseDC(Window, WindowDC);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{
    LRESULT Result = 0;
    
    switch(Message)
    {
        case WM_CLOSE:
        {
            GlobalRunning = false;
        } break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        
        case WM_DESTROY:
        {
            // TODO(casey): Handle this as an error - recreate window?
            GlobalRunning = false;
        } break;
        
        case WM_PAINT: 
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBufferInWindow(DeviceContext, 
                                       &GlobalBackBuffer,
                                       Dimension.Width, 
                                       Dimension.Height);
            EndPaint(Window, &Paint);
        }
        
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    
    return Result;
}

internal void Win32ProcessController(game_button_state *Button, bool32 IsDown)
{
    if(Button->EndedDown != IsDown)
    {
        Button->EndedDown = IsDown;
    }
}

internal void Win32KeybroadMessageProcess(game_controller *NewController)
{
    MSG Message;
    
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT:
            {
                GlobalRunning = false;
            }break;
            
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                uint32 VKCode = (uint32)Message.wParam;
                bool32 WasDown = (Message.lParam & (1 << 30)) != 0;
                bool32 IsDown = (Message.lParam & (1 << 31)) == 0;
                
                if(WasDown != IsDown)
                {
                    if(VKCode == VK_LEFT)
                    {
                        Win32ProcessController(&NewController->Left, IsDown);
                    }
                    else if(VKCode == VK_RIGHT)
                    {
                        Win32ProcessController(&NewController->Right, IsDown);
                    }
                    else if(VKCode == VK_UP)
                    {
                        Win32ProcessController(&NewController->Up, IsDown);
                    }
                    else if(VKCode == VK_DOWN)
                    {
                        Win32ProcessController(&NewController->Down, IsDown);
                    }
                    else if(VKCode == VK_RETURN)
                    {
                        Win32ProcessController(&NewController->Start, IsDown);
                    }
                }
            }break;
            
            default:
            {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            }break;
        }
    }
}

internal FILETIME Win32GetLastFileWriteTime(char *FilePath)
{
    FILETIME LastWriteTime = {};

    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFileA(FilePath, &FindData);
    if(FindHandle != INVALID_HANDLE_VALUE)
    {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }
    
    return LastWriteTime;
}

internal win32_game_mode Win32LoadGameCode(char *SourceGameDLLFilePath, 
                                           char *TempGameDLLFilePath)
{
    win32_game_mode Result = {0};

    Result.LastWriteTime = Win32GetLastFileWriteTime(SourceGameDLLFilePath);
    
    CopyFile(SourceGameDLLFilePath, TempGameDLLFilePath, FALSE);
    Result.GameCodeDLL = LoadLibraryA(TempGameDLLFilePath);
    
    if(Result.GameCodeDLL)
    {
        Result.UpdateAndRender = 
            (game_update_and_render *)GetProcAddress(Result.GameCodeDLL, 
                                                     "GameUpdateAndRender");
        
        Result.IsValid = Result.GameCodeDLL != 0;
    }
    
    if(!Result.IsValid)
    {
        Result.UpdateAndRender = GameUpdateAndRenderStub;
    }
    
    return Result;
}

internal void Win32UnloadGameCode(win32_game_mode *GameCode)
{
    if(GameCode->GameCodeDLL)
    {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }
    
    GameCode->IsValid = false;
    GameCode->UpdateAndRender = GameUpdateAndRenderStub;
}

internal void CatString(size_t SourceASize, char *SourceA,
                        size_t SourceBSize, char *SourceB,
                        size_t DestSize, char *Dest)
{
    for(int Index = 0; Index < SourceASize; Index++)
    {
        *Dest++ = *SourceA++;
    }
    
    for(int Index = 0; Index < SourceBSize; Index++)
    {
        *Dest++ = *SourceB++;
    }
    
    *Dest++ = 0;
}

internal int64 Win32GetWallClock()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    
    return Result.QuadPart;
}

internal real32 Win32GetSecondElapsed(int64 Start, int64 End)
{
    real32 Result =(real32)(End - Start) / (real32)GlobalPerfCountFrequency;
    
    return Result;
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    char EXEFileName[MAX_PATH];
    GetModuleFileName(0, EXEFileName, sizeof(EXEFileName));
    
    char *LastBackSlashPlusOne = EXEFileName;
    for(char *at = EXEFileName; *at; at++)
    {
        if(*at == '\\')
        {
            LastBackSlashPlusOne = at + 1;
        }
    }
    
    char SourceGameDLLFilePath[MAX_PATH];
    char SourceGameDLLFileName[] = "snake.dll";
    CatString((LastBackSlashPlusOne - EXEFileName), EXEFileName,
              sizeof(SourceGameDLLFileName), SourceGameDLLFileName,
              sizeof(SourceGameDLLFilePath), SourceGameDLLFilePath);
    
    char TempGameDLLFilePath[MAX_PATH];
    char TempGameDLLFileName[] = "snake_temp.dll";
    CatString((LastBackSlashPlusOne - EXEFileName), EXEFileName,
              sizeof(TempGameDLLFileName), TempGameDLLFileName,
              sizeof(TempGameDLLFilePath), TempGameDLLFilePath);
    
    LARGE_INTEGER QueryPerfFrequencyResult;
    QueryPerformanceFrequency(&QueryPerfFrequencyResult);
    GlobalPerfCountFrequency = QueryPerfFrequencyResult.QuadPart;
    
    int32 MonitorRefreshHz = 60;
    int32 GameUpdateHz = MonitorRefreshHz / 2;
    real32 TargetSecondPerFrame = 1.0f / (real32)GameUpdateHz;
    
    UINT DesiredSchedulerMS = 1;
    bool32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);
    
    WNDCLASS WindowClass = {};
    
    WindowClass.style         = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc   = Win32MainWindowCallback;
    WindowClass.hInstance     = Instance;
    WindowClass.lpszClassName = "SnakeWindowClass";
    
    if(RegisterClassA(&WindowClass))
    {
        HWND Window = 
            CreateWindowExA(0, 
                            WindowClass.lpszClassName, 
                            "Snake", 
                            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                            CW_USEDEFAULT, 
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            0, 
                            0, 
                            Instance, 
                            0);
        
        if(Window)
        {
            Win32InitOpenGL(Window);
            Win32InitOffScreenBuffer(&GlobalBackBuffer, 1280, 720);
            GlobalRunning = true;
            
            HDC DeviceContext = GetDC(Window);

#if 1 //DEBUG
            LPVOID BaseAddress = (LPVOID) Terabytes(2);
#else
            LPVOID BaseAddress = 0;
#endif
            
            game_memory GameMemory;
            GameMemory.StorageSize = Megabytes(64);
            GameMemory.Storage = VirtualAlloc(BaseAddress, GameMemory.StorageSize, 
                                              MEM_COMMIT|MEM_RESERVE, 
                                              PAGE_READWRITE);
            GameMemory.IsInit = false;
            if(GameMemory.Storage)
            {
                win32_game_mode Game = Win32LoadGameCode(SourceGameDLLFilePath, TempGameDLLFilePath);
                
                game_controller GameControllers[2] = {0};
                
                game_controller *NewController = &GameControllers[0];
                game_controller *OldController = &GameControllers[1];
                
                int64 LastCounter = Win32GetWallClock();
               
                uint64 LastCycleCount = __rdtsc();
                
                while(GlobalRunning)
                {
                    FILETIME NewDLLWriteTime = Win32GetLastFileWriteTime(SourceGameDLLFilePath);
                    if(CompareFileTime(&NewDLLWriteTime, &Game.LastWriteTime) != 0)
                    {
                        Win32UnloadGameCode(&Game);
                        Game = Win32LoadGameCode(SourceGameDLLFilePath, TempGameDLLFilePath);
                    }
                    
                    *NewController = {0};
                    for(int ButtonIndex = 0; ButtonIndex < ArrayCount(NewController->Buttons); ButtonIndex++)
                    {
                        NewController->Buttons[ButtonIndex].EndedDown = OldController->Buttons[ButtonIndex].EndedDown;
                    }
                    
                    Win32KeybroadMessageProcess(NewController);
                    
                    game_screen_buffer Buffer = {0};
                    Buffer.Memory = (uint8 *)GlobalBackBuffer.Memory;
                    Buffer.Width = GlobalBackBuffer.Width;
                    Buffer.Height = GlobalBackBuffer.Height;
                    Buffer.Pitch = GlobalBackBuffer.Pitch;
                    Buffer.BytesPerPixel = GlobalBackBuffer.BytesPerPixel;
                    
                    Game.UpdateAndRender(&Buffer, &GameMemory, NewController);
                    
                    if(Win32GetSecondElapsed(LastCounter, Win32GetWallClock()) < TargetSecondPerFrame)
                    {
                        if(SleepIsGranular)
                        {
                            DWORD SleepMS = (DWORD)(1000.f * Win32GetSecondElapsed(LastCounter, Win32GetWallClock()));
                            
                            if(SleepMS > 0)
                            {
                                Sleep(SleepMS);
                            }
                        }
                        
                        while(Win32GetSecondElapsed(LastCounter, Win32GetWallClock()) < TargetSecondPerFrame){}
                    }
                    
                    win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                    Win32DisplayBufferInWindow(DeviceContext, &GlobalBackBuffer, 
                                               Dimension.Width, Dimension.Height);
                    
                    
                    game_controller *Temp = NewController;
                    NewController = OldController;
                    OldController = Temp;
                    
                    int64 EndCounter = Win32GetWallClock();
                    uint64 EndCycleCount = __rdtsc();
                    
                    int64 CounterElapsed = EndCounter - LastCounter;
                    uint64 CycleElapsed = EndCycleCount -LastCycleCount;
                    
                    real32 MSPerFrame = (1000.0f) * Win32GetSecondElapsed(LastCounter, EndCounter);
                    real32 FPS = (real32)GlobalPerfCountFrequency / (real32)CounterElapsed;
                    real32 MCPF = (real32)((real32)CycleElapsed / (1000.0f * 1000.0f));
                    
                    char FPSBuffer[256];
                    _snprintf_s(FPSBuffer, sizeof(FPSBuffer), "MSPerFrame: %f, FPS: %f, MCPF: %f\n", MSPerFrame, FPS, MCPF);
                    OutputDebugStringA(FPSBuffer);
                    
                    LastCounter = EndCounter;
                    LastCycleCount = EndCycleCount;
                }
            }
            
            
        }
        else// !Window
        {
            
        }
    }
    else// !RegisterClass(&WindowClass)
    {
        
    }
    return 0;
}
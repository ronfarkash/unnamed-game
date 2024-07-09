#include <dsound.h>
#include <stdint.h>
#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

typedef int32_t bool32;

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuideDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;

struct win32_window_dimension
{
    int Width;
    int Height;
};

win32_window_dimension Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return (Result);
}

internal void Win32InitDSound(HWND Window, int32_t SamplesPerSecond, int32_t BufferSize)
{
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if (DSoundLibrary)
    {
        direct_sound_create *DirectSoundCreate =
            (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");
        LPDIRECTSOUND DirectSound;
        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
        {
            WAVEFORMATEX WaveFormat = {};
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = SamplesPerSecond;
            WaveFormat.wBitsPerSample = 16;
            WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
            WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
            WaveFormat.cbSize = 0;

            if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
            {
                DSBUFFERDESC BufferDescription = {};
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
                {
                    if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat)))
                    {
                    }
                    else
                    {
                    }
                }
                else
                {
                }
            }
            else
            {
            }

            DSBUFFERDESC BufferDescription = {};
            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = 0;
            BufferDescription.dwBufferBytes = BufferSize;
            BufferDescription.lpwfxFormat = &WaveFormat;
            if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0)))
            {
            }
        }
        else
        {
        }
    }
}

internal void Win32RenderGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset)
{
    uint8_t *Row = (uint8_t *)Buffer->Memory;
    for (int Y = 0; Y < Buffer->Height; ++Y)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for (int X = 0; X < Buffer->Width; ++X)
        {
            uint8_t Blue = (uint8_t)(X + XOffset);
            uint8_t Green = (uint8_t)(Y + YOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }
        Row += Buffer->Pitch;
    }
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    if (Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    int BytesPerPixel = 4;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    /* each pixel is RGB, 8 bits for
     each color, and an extra byte is
     added for padding to optimize x86
     architecture need to read more
     about it*/
    int BitmapMemorySize = (Buffer->Width * Buffer->Height) * BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    Buffer->Pitch = Width * BytesPerPixel;
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, win32_offscreen_buffer *Buffer)
{
    StretchDIBits(
        DeviceContext, 0, 0, WindowWidth, WindowHeight, 0, 0, Buffer->Width, Buffer->Height, Buffer->Memory,
        &Buffer->Info, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        case WM_DESTROY:
        {
            GlobalRunning = false;
        }
        break;

        case WM_CLOSE:
        {
            GlobalRunning = false;
        }
        break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }
        break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32_t VKCode = WParam;
            bool WasDown = ((LParam & (1 << 30)) != 0);
            bool IsDown = ((LParam & (1 << 31)) == 0);
            if (WasDown == IsDown)
                break;
            if (VKCode == VK_UP)
            {
            }
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

            win32_window_dimension Dimension = Win32GetWindowDimension(Window);

            Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, &GlobalBackbuffer);
            EndPaint(Window, &Paint);
        }
        break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }
        break;
    }

    return (Result);
}

int CALLBACK WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
    WNDCLASSA WindowClass = {};

    Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    // WindowClass.hIcon
    WindowClass.lpszClassName = "GameWindowClass";

    if (RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowExA(
            0, WindowClass.lpszClassName, "Unnamed Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);
        if (Window)
        {
            HDC DeviceContext = GetDC(Window);
            GlobalRunning = true;
            int XOffset = 0;
            int YOffset = 0;

            int SamplesPerSecond = 48000;
            int ToneHz = 256;
            int16_t ToneVolume = 1000;
            uint32_t RunningSampleIndex = 0;
            int SquareWavePeriod = 48000 / ToneHz;
            int HalfSquareWavePeriod = SquareWavePeriod / 2;
            int BytesPerSample = sizeof(int16_t) * 2;
            int SecondaryBufferSize = SamplesPerSecond * BytesPerSample;

            Win32InitDSound(Window, SamplesPerSecond, SecondaryBufferSize);

            GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

            while (GlobalRunning)
            {
                MSG Message;
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                Win32RenderGradient(&GlobalBackbuffer, XOffset, YOffset);

                DWORD PlayCursor;
                DWORD WriteCursor;
                if (SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
                {
                    DWORD ByteToLock = RunningSampleIndex * BytesPerSample % SecondaryBufferSize;
                    DWORD BytesToWrite;
                    if (ByteToLock > PlayCursor)
                    {
                        BytesToWrite = (SecondaryBufferSize - ByteToLock);
                        BytesToWrite += PlayCursor;
                    }
                    else
                    {
                        BytesToWrite = PlayCursor - ByteToLock;
                    }

                    VOID *Region1;
                    DWORD Region1Size;
                    VOID *Region2;
                    DWORD Region2Size;

                    if (SUCCEEDED(GlobalSecondaryBuffer->Lock(
                            ByteToLock, BytesToWrite, &Region1, &Region1Size, &Region2, &Region2Size, 0)))
                    {
                        int16_t *SampleOut = (int16_t *)Region1;
                        DWORD Region1SampleCount = Region1Size / BytesPerSample;

                        for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex)
                        {
                            int16_t SampleValue =
                                ((RunningSampleIndex++ / HalfSquareWavePeriod) % 2) ? ToneVolume : -ToneVolume;
                            *SampleOut++ = SampleValue;
                            *SampleOut++ = SampleValue;
                        }

                        SampleOut = (int16_t *)Region2;
                        DWORD Region2SampleCount = Region2Size / BytesPerSample;
                        for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex)
                        {
                            int16_t SampleValue =
                                ((RunningSampleIndex++ / HalfSquareWavePeriod) % 2) ? ToneVolume : -ToneVolume;
                            *SampleOut++ = SampleValue;
                            *SampleOut++ = SampleValue;
                        }

                        GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
                    }
                }

                win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, &GlobalBackbuffer);
                ReleaseDC(Window, DeviceContext);

                ++XOffset;
            }
        }
        else
        {
        }
    }
    else
    {
    }

    return (0);
}

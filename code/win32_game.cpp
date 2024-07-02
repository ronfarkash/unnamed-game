#include <stdint.h>
#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;

internal void
Win32RenderGradient(int XOffset, int YOffset)
{
    int Width = BitmapWidth;

    int Pitch = Width * 4;
    uint8_t *Row = (uint8_t *)BitmapMemory;
    for (int Y = 0; Y < BitmapHeight; ++Y)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for (int X = 0; X < BitmapWidth; ++X)
        {
            uint8_t Blue = (uint8_t)(X + XOffset);
            uint8_t Green = (uint8_t)(Y + YOffset);

            // Memory: BB GG RR xx
            // Register: xx RR GG BB
            // 00 00 00 00
            // BB
            *Pixel++ = ((Green << 8) | Blue);
        }
        Row += Pitch;
    }
}

internal void
Win32ResizeDIBSection(int Width, int Height)
{
    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    // each pixel is RGB, 8 bits for each color, and an extra byte is added for
    // padding to optimize x86 architecture need to read more about it
    int BytesPerPixel = 4;
    int BitmapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
    BitmapMemory =
        VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(
    HDC DeviceContext,
    RECT *ClientRect,
    int X,
    int Y,
    int Width,
    int Height)
{
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;
    StretchDIBits(
        DeviceContext,
        /*X,
        Y,
        Width,
        Height,
        X,
        Y,
        Width,
        Height,*/
        0,
        0,
        BitmapWidth,
        BitmapHeight,
        0,
        0,
        WindowWidth,
        WindowHeight,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
        }
        break;
        case WM_DESTROY:
        {
            Running = false;
        }
        break;

        case WM_CLOSE:
        {
            Running = false;
        }
        break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
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

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);

            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
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

int CALLBACK WINAPI
WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    int ShowCode)
{
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    // WindowClass.hIcon
    WindowClass.lpszClassName = "GameWindowClass";

    if (RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowExA(
            0,
            WindowClass.lpszClassName,
            "Unnamed Game",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            Instance,
            0);
        if (Window)
        {
            Running = true;
            int XOffset = 0;
            int YOffset = 0;
            MSG Message;
            while (Running)
            {
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                Win32RenderGradient(XOffset, YOffset);

                HDC DeviceContext = GetDC(Window);
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);

                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(
                    DeviceContext,
                    &ClientRect,
                    0,
                    0,
                    WindowWidth,
                    WindowHeight);
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

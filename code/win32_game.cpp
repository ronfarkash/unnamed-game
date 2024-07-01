#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;


internal void
Win32ResizeDIBSection(int Width, int Height)
{
    if(BitmapMemory)
    {
        VirtualFree();
    }

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    // each pixel is RGB, 8 bits for each color, and an extra byte is added for padding to optimize x86 architecture
    // need to read more about it
    int BytesPerPixel = 4;
    int BitmapMemorySize = (Width * Height) * BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, sizeof(BitmapMemorySize), MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
    StretchDIBits(
        DeviceContext,
        X,
        Y,
        Width,
        Height,
        X,
        Y,
        Width,
        Height,
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
            Win32UpdateWindow(DeviceContext, X, Y, Width, Height);

            EndPaint(Window, &Paint);
        }
        break;
        default:
        {
            // OutputDebugStringA("default\n");
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
        HWND WindowHandle = CreateWindowExA(
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
        if (WindowHandle)
        {
            Running = true;
            MSG Message;
            while (Running)
            {
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                if (MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                else
                {
                    break;
                }
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

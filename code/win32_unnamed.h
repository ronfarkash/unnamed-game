#ifndef WIN32_UNNAMED_H
#define WIN32_UNNAMED_H

struct win32_window_dimension
{
    int Width;
    int Height;
};

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct win32_sound_output
{
    int SamplesPerSecond;
    uint32_t RunningSampleIndex;
    int BytesPerSample;
    DWORD SecondaryBufferSize;
    float tSine;
    int LatencySampleCount;
};

struct win32_debug_time_marker
{
    DWORD PlayCursor;
    DWORD WriteCursor;
};

#endif /* WIN32_UNNAMED_H */

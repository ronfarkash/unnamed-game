#ifndef UNNAMED_H
#define UNNAMED_H

// timing, input, bitmap to render, sound

#if UNNAMED_SLOW
#define Assert(Expression) \
    if (!(Expression))     \
    {                      \
        *(int *)0 = 0;     \
    }
#else
#define Assert(Expression)
#endif

#if UNNAMED_INTERNAL
struct debug_read_file_result
{
    uint32_t ContentsSize;
    void *Contents;
};
internal debug_read_file_result DEBUGPlatformReadEntireFile(char *Filename);
internal void DEBUGPlatformFreeFileMemory(void *Memory);
internal bool32 DEBUGPlatformWriteEntireFile(char *Filename, uint32_t MemorySize, void *Memory);
#endif

inline uint32_t SafeTruncateUInt64(uint64_t Value)
{
    Assert(Value <= 0xFFFFFF);
    uint32_t Result = (uint32_t)Value;
    return (Result);
}

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define Kilobytes(Value) ((Value) * 1024)
#define Megabytes(Value) (Kilobytes(Value) * 1024)
#define Gigabytes(Value) (Megabytes(Value) * 1024)
#define Terabytes(Value) (Gigabytes(Value) * 1024)

struct game_offscreen_buffer
{
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    int16_t *Samples;
};

struct game_button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
};

struct game_controller_input
{
    union
    {
        game_button_state Buttons[5];
        struct
        {
            game_button_state Up;
            game_button_state Down;
            game_button_state Left;
            game_button_state Right;

            // this is ugly
            game_button_state Terminator;
        };
    };
};

struct game_input
{
    game_controller_input Controllers[5];
};

struct game_memory
{
    bool32 IsInitialized;
    uint64_t PermanentStorageSize;
    void *PermanentStorage;
    uint64_t TransientStorageSize;
    void *TransientStorage;
};

internal void GameUpdateAndRender(
    game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer, game_sound_output_buffer *SoundBuffer);

struct game_state
{
    int ToneHz;
    int GreenOffset;
    int BlueOffset;
};
#endif

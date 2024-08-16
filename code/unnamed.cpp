#include "unnamed.h"

internal void RenderGradient(game_offscreen_buffer *Buffer, int XOffset, int YOffset)
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

internal void GameOutputSound(game_sound_output_buffer *SoundBuffer, int ToneHz)
{
    local_persist float tSine;
    int16_t ToneVolume = 1000;
    int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz;

    int16_t *SampleOut = SoundBuffer->Samples;

    for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex)
    {
        float SineValue = sinf(tSine);
        int16_t SampleValue = (int16_t)(SineValue * ToneVolume);
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;

        tSine += 2.0f * Pi32 * 1.0f / (float)WavePeriod;
    }
}

internal void GameUpdateAndRender(
    game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer, game_sound_output_buffer *SoundBuffer)
{
    Assert(
        (&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) ==
        (ArrayCount(Input->Controllers[0].Buttons) - 1));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
        debug_read_file_result File = DEBUGPlatformReadEntireFile(__FILE__);
        if (File.Contents)
        {
            DEBUGPlatformWriteEntireFile("w:/unnamed-game/test.out", File.ContentsSize, File.Contents);
            DEBUGPlatformFreeFileMemory(File.Contents);
        }
        GameState->ToneHz = 256;

        Memory->IsInitialized = true;
    }
    game_controller_input *Input0 = &Input->Controllers[0];
    GameState->GreenOffset += (int)(2.0f * Input0->Down.EndedDown);
    GameState->GreenOffset -= (int)(2.0f * Input0->Up.EndedDown);
    GameState->BlueOffset += (int)(2.0f * Input0->Right.EndedDown);
    GameState->BlueOffset -= (int)(2.0f * Input0->Left.EndedDown);

    GameOutputSound(SoundBuffer, GameState->ToneHz);
    RenderGradient(Buffer, GameState->BlueOffset, GameState->GreenOffset);
}
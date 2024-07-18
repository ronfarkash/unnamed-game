#ifndef UNNAMED_H
#define UNNAMED_H

struct game_offscreen_buffer
{
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

// timing, input, bitmap to render, sound
void GameUpdateAndRender(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset);

#endif
#include <GL/gl.h>

#include "plots.h"

void plot(int col, int row)
{
    glVertex2f(
        (float)col + 0.5f,
        (float)row + 0.5f
    );
}

void plotPure(int col, int row)
{
    (void)col;
    (void)row;
}
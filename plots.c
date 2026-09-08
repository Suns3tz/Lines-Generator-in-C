#include <GL/gl.h>

// Dibujar pixeles
void plot(int col, int row) {
    glVertex2f(col + 0.5f, row + 0.5f);
}

void plotPure(int col, int row) {
    return;
}
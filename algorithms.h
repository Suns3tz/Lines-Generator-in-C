#ifndef ALGORITHMS_H
#define ALGORITHMS_H

// Fuerza Bruta
void Brute(
    int x0,
    int y0,
    int x1,
    int y1
);

void BrutePure(
    int x0,
    int y0,
    int x1,
    int y1
);

// Incremental versión 1
void IncrVerOne(
    int x0,
    int y0,
    int x1,
    int y1
);

void IncrVerOnePure(
    int x0,
    int y0,
    int x1,
    int y1
);

// Incremental versión 2
void IncrVerTwo(
    int x0,
    int y0,
    int x1,
    int y1
);

void IncrVerTwoPure(
    int x0,
    int y0,
    int x1,
    int y1
);

// Bresenham C
int BresenhamC(
    int x0,
    int y0,
    int x1,
    int y1
);

int BresenhamCPure(
    int x0,
    int y0,
    int x1,
    int y1
);

// Clasificación compartida
int classify_octant(
    int x0,
    int y0,
    int x1,
    int y1
);

// Bresenham NASM
int BresenhamNASM(
    int x0,
    int y0,
    int x1,
    int y1
);

int BresenhamNASMPure(
    int x0,
    int y0,
    int x1,
    int y1
);

#endif
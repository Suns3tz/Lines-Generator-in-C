#include <stdlib.h>
#include <math.h>

#include "plots.h"
#include "algorithms.h"

// Funciones NASM
extern void oct1(int, int, int, int);
extern void oct2(int, int, int, int);
extern void oct3(int, int, int, int);
extern void oct4(int, int, int, int);
extern void oct5(int, int, int, int);
extern void oct6(int, int, int, int);
extern void oct7(int, int, int, int);
extern void oct8(int, int, int, int);

// Algoritmo de Fuerza Bruta
void Brute(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    // Si solo es un punto
    if (dx == 0 && dy == 0) {
        plot(x0, y0);
        return;
    }

    // Si existe mayor cambio horizontal se recorre x
    if (abs(dx) >= abs(dy)) {
        // Se ve si se va a subir o bajar
        int step_x = (dx > 0) ? 1 : -1;
        //Se obtienen m y b
        long double m = (long double)dy / (long double)dx;
        long double b = (long double)y0 - m * (long double)x0;
        // Se pintan los puntos
        for (int x = x0; ; x += step_x) {
            long double y = m * x + b;

            plot(x, (int)roundl(y));

            if (x == x1) {
                break;
            }
        }
    }

    // Si existe mayor cambio vertical se recorre y
    else {
        // Se ve si se va a subir o bajar
        int step_y = (dy > 0) ? 1 : -1;
        // Se calcula la inversa de m
        long double inverse_m =(long double)dx / (long double)dy;
        // Se pintan los puntos
        for (int y = y0; ; y += step_y) {
            long double x = x0 + inverse_m * (y - y0);

            plot((int)roundl(x), y);

            if (y == y1) {
                break;
            }
        }
    }
}

// Algoritmo Incremental Version 1
static void swapPoints(int *x0, int *y0, int *x1, int *y1) {
    int temporary;

    temporary = *x0;
    *x0 = *x1;
    *x1 = temporary;

    temporary = *y0;
    *y0 = *y1;
    *y1 = temporary;
}

void IncrVerOne(int x0, int y0, int x1, int y1){
  if (x0 == x1) {
    if (y1 < y0){
      int tmp = y1;
      y1 = y0;
      y0 = tmp;
    }
    for (int y = y0 ; y <= y1; y++){
      plot(x0,y);
    }
    return;
  }
  if (x1 < x0){
    swapPoints(&x0, &y0, &x1, &y1);
  }
  
  long double m = (long double)(y1-y0)/(x1-x0);
  
  if (fabsl(m) <= 1.0) {
    long double y = y0;
    int x = x0;
    for (x = x0; x<= x1 ; x++){
      plot(x, roundl(y));
      y += m;
    }
  }
  else{
    if(y0 > y1) {
      swapPoints(&x0, &y0, &x1, &y1);
    }
    long double x = x0;
    m = (long double)(x1-x0)/(y1-y0);
    for (int y = y0; y<= y1 ; y++){
      plot(roundl(x), y);
      x += m;
    }
  }
}

// Algoritmo Incremental Version 2
static int max(int a, int b) {
    return (a >= b) ? a : b;
}

void IncrVerTwo(int x0, int y0, int x1, int y1) {
  long double x,y, paso_x, paso_y;
  int ancho;
  ancho = max(abs(x1-x0), abs(y1-y0));
  paso_x = (x1-x0)/ancho;
  paso_y = (y1-y0)/ancho;
  if (ancho == 0) {
    plot(x0,y0);
    return;
  }
  paso_x = (long double)(x1-x0)/ancho;
  paso_y = (long double)(y1-y0)/ancho;
  x = x0;
  y = y0;
  for (int i=0; i <= ancho; i++){
    plot(roundl(x),roundl(y));
    x += paso_x;
    y += paso_y;
  }
}

// Bresenham en C
void oct_1(int x0, int y0, int x1, int y1){
  int Delta_E = 2*(y1 - y0);
  int Delta_Ne = 2*((y1 - y0) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;
  plot(xp,yp);
  
  int d = 2* (y1-y0) - (x1 - x0);
  while(xp < x1){
    if(d<=0){
      xp++;
      d = d + Delta_E;
    }else{
      xp++;
      yp++;
      d = d + Delta_Ne;
    }
    plot(xp,yp);
  } 
}

void oct_2(int x0, int y0, int x1, int y1){
  int Delta_N = 2*(x0 - x1);
  int Delta_Ne = 2*((y1 - y0) - (x1 - x0));

  int xp = x0;
  int yp = y0;
  plot(xp,yp);

  int d = (y1-y0) - 2*(x1 - x0);
  while(yp < y1){
    if(d<=0){
      xp++;
      yp++;
      d = d + Delta_Ne;
    }else{
      yp++;
      d = d + Delta_N;
    }
    plot(xp,yp);
  }
}

void oct_3(int x0, int y0, int x1, int y1){
  int Delta_N = 2*(x0 - x1);
  int Delta_NO = 2*((y0 - y1) + (x0 - x1));

  int xp = x0;
  int yp = y0;
  plot(xp,yp);

  int d = (y0-y1) - 2*(x1 - x0);
  while(yp < y1){
    if(d<=0){
      yp++;
      d = d + Delta_N;
    }else{
      xp--;
      yp++;
      d = d + Delta_NO;
    }
    plot(xp,yp);
  }
}

void oct_4(int x0, int y0, int x1, int y1){
  int Delta_O = 2*(y0 - y1);
  int Delta_NO = 2*((y0 - y1) + (x0 - x1));

  int xp = x0;
  int yp = y0;
  plot(xp,yp);

  int d = 2*(y0-y1) - (x1 - x0);
  while(xp > x1){
    if(d<=0){
      xp--;
      yp++;
      d = d + Delta_NO;
    }else{
      xp--;
      d = d + Delta_O;
    }
    plot(xp,yp);
  }
}

void oct_5(int x0, int y0, int x1, int y1){
  int Delta_O = 2*(y1 - y0);
  int Delta_SO =  2*((y1 - y0) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;
  plot(xp,yp);
  
  int d = 2 * (y1-y0) - (x1 - x0);
  while(xp > x1){
    if(d>=0){
      xp--;
      d = d + Delta_O;
    }else{
      xp--;
      yp--;
      d = d + Delta_SO;
    }
    plot(xp,yp);
  } 
}

void oct_6(int x0, int y0, int x1, int y1){
  int Delta_S = 2*(x0 - x1);
  int Delta_SO =  2*((x0 - x1) - (y0 - y1));
  
  int xp = x0;
  int yp = y0;
  plot(xp,yp);
  
  int d = 2 * (x0 - x1) - (y0-y1);
  while(yp > y1){
    if(d>=0){
      yp--;
      xp--;
      d = d + Delta_SO;
    }else{
      yp--;
      d = d + Delta_S;
    }
    plot(xp,yp);
  } 
}

void oct_7(int x0, int y0, int x1, int y1){
  int Delta_S = 2*(x1-x0);
  int Delta_SE = 2*((x1-x0) - (y0 - y1));
  
  int xp = x0;
  int yp = y0;
  
  plot(xp,yp);

  int d = 2 * (x1 - x0) - (y0-y1);
  while(yp > y1){
    if(d>=0){
      yp--;
      xp++;
      d = d + Delta_SE;
    }else{
      yp--;
      d = d + Delta_S;
    }
    plot(xp,yp);
  } 
}

void oct_8(int x0, int y0, int x1, int y1){
  int Delta_E = 2*(y0 - y1);
  int Delta_SE = 2*((y0-y1) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;

  plot(xp,yp);

  int d = 2 * (y0-y1) - (x1 - x0);

  while(xp < x1){
    if(d <= 0){
      xp++;
      d = d + Delta_E;
    }else{
      xp++;
      yp--;
      d = d + Delta_SE;
    }
    plot(xp,yp);
  } 
}

int BresenhamC(int x0, int y0, int x1, int y1) {
    int octant = classify_octant(x0, y0, x1, y1);

    switch (octant) {
        case 1:
            oct_1(x0, y0, x1, y1);
            return 1;

        case 2:
            oct_2(x0, y0, x1, y1);
            return 1;

        case 3:
            oct_3(x0, y0, x1, y1);
            return 1;

        case 4:
            oct_4(x0, y0, x1, y1);
            return 1;

        case 5:
            oct_5(x0, y0, x1, y1);
            return 1;
        
        case 6:
            oct_6(x0, y0, x1, y1);
            return 1;
        
        case 7:
            oct_7(x0, y0, x1, y1);
            return 1;

        case 8:
            oct_8(x0, y0, x1, y1);
            return 1;

        default:
            return 0;
    }
}

// Bresenham en NASM
// Ver cual es el octante que se usa
int classify_octant(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    // Si solo es un punto se asigna al octante 1
    if (dx == 0 && dy == 0) {
        return 1;
    }

    // En la mitad derecha x aumenta
    if (dx >= 0) {
        if (dy >= 0) {
            if (dx >= dy) {
                return 1;
            }

            return 2;
        }
        if (dx >= -dy) {
            return 8;
        }

        return 7;
    }

    // En la mitad izquierda x disminuye
    if (dy >= 0) {
        if (dy >= -dx) {
            return 3;
        }

        return 4;
    }
    if (-dx >= -dy) {
        return 5;
    }

    return 6;
}

// Correr el octante necesario en NASM
int BresenhamNASM(int x0, int y0, int x1, int y1) {
    int octant = classify_octant(x0, y0, x1, y1);

    switch (octant) {
        case 1:
            oct1(x0, y0, x1, y1);
            return 1;

        case 2:
            oct2(x0, y0, x1, y1);
            return 1;

        case 3:
            oct3(x0, y0, x1, y1);
            return 1;

        case 4:
            oct4(x0, y0, x1, y1);
            return 1;

        case 5:
            oct5(x0, y0, x1, y1);
            return 1;
        
        case 6:
            oct6(x0, y0, x1, y1);
            return 1;
        
        case 7:
            oct7(x0, y0, x1, y1);
            return 1;

        case 8:
            oct8(x0, y0, x1, y1);
            return 1;

        default:
            return 0;
    }
}
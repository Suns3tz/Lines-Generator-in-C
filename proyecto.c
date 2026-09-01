
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Funciones implementadas en NASM
extern void oct1(int x0, int y0, int x1, int y1);


typedef struct {
  double r;
  double g;
  double b;
} COLOR;

void draw_scene(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  glFlush(); 
}


void main(int argc, char *argv[]){
  int H_SIZE = atoi(argv[1]);
  int V_SIZE = atoi(argv[1]);
  int linesNum = atoi(argv[2]);
  int quantity = atoi(argv[3]);
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(H_SIZE,V_SIZE);
  glutCreateWindow("Lol");
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, H_SIZE, 0, V_SIZE);
  glLoadIdentity();
  glutDisplayFunc(draw_scene);
  
  glutMainLoop();
}

int max(int a, int b){
  if (a>=b) return a;
  else return b;
}

void BrutePure (int x0, int y0, int x1, int y1) {
  long double m, b, y;
  int i;
  m = (y1 - y0) / (x1 - x0);
  b = y0 - m*x0;
  
  for(int i = x0; i <= x1; i++){
    y = m*i + b;
    //plot(i, round(y));
  }
}

void IncrVerOnePure (int x0, int y0, int x1, int y1){
  long double m,y;
  int i;
  m = (y1-y0)/(x1-x0);
  y = y0;
  
  for(int i=x0; i<= x1; i++){
    //plot(i, round(y));
    y = y + m;
  }
}

void IncrVerTwoPure (int x0, int y0, int x1, int y1) {
  long double x,y, paso_x, paso_y;
  int ancho;
  ancho = max(abs(x1-x0), abs(y1-y0));
  paso_x = (x1-x0)/ancho;
  paso_y = (y1-y0)/ancho;
  x = x0;
  y = y0;
  for (int i=0; i <= ancho; i++)
  {
  //plot(round(x), round(y));
  x+=paso_x;
  y+=paso_y;
  }
}

void bresenham_nasm(int x0, int y0, int x1, int y1){
    int dx = x1 - x0;
    int dy = y1 - y0;

    if (dx >= 0 && dy >= 0) {

        if (dx >= dy)
            oct1(x0, y0, x1, y1);
        else
            oct2(x0, y0, x1, y1);

    } else if (dx < 0 && dy >= 0) {

        if (-dx <= dy)
            oct3(x0, y0, x1, y1);
        else
            oct4(x0, y0, x1, y1);

    } else if (dx < 0 && dy < 0) {

        if (-dx >= -dy)
            oct5(x0, y0, x1, y1);
        else
            oct6(x0, y0, x1, y1);

    } else {

        if (dx <= -dy)
            oct7(x0, y0, x1, y1);
        else
            oct8(x0, y0, x1, y1);
    }
}


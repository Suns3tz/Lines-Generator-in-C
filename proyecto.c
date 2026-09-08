
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Funciones implementadas en NASM
extern void oct1(int x0, int y0, int x1, int y1);
extern void oct2(int x0, int y0, int x1, int y1);
extern void oct3(int x0, int y0, int x1, int y1);
extern void oct4(int x0, int y0, int x1, int y1);
extern void oct5(int x0, int y0, int x1, int y1);
extern void oct6(int x0, int y0, int x1, int y1);
extern void oct7(int x0, int y0, int x1, int y1);
extern void oct8(int x0, int y0, int x1, int y1);

extern void oct1_pure(int x0, int y0, int x1, int y1);
extern void oct2_pure(int x0, int y0, int x1, int y1);
extern void oct3_pure(int x0, int y0, int x1, int y1);
extern void oct4_pure(int x0, int y0, int x1, int y1);
extern void oct5_pure(int x0, int y0, int x1, int y1);
extern void oct6_pure(int x0, int y0, int x1, int y1);
extern void oct7_pure(int x0, int y0, int x1, int y1);
extern void oct8_pure(int x0, int y0, int x1, int y1);

typedef struct {
    int x0;
    int y0;
    int x1;
    int y1;
} LINE;

// Vars para dibujar las escenas
static int resolution;
static int lines_num;
static int quantity;
static LINE *lines;

static int display_stage = 0;
static int brute_measured = 0;
static int inc_ver_one_measured = 0;
static int inc_ver_two_measured = 0;
static int bresenham_c_measured = 0;
static int nasm_measured = 0;

// Prototipos
void cleanup(void);
void generate_lines(void);
void reshape(int width, int height);
void plot(int col, int row);
void plotPure(int col, int row);

void Brute(int x0, int y0, int x1, int y1);
void BrutePure(int x0, int y0, int x1, int y1);
void IncrVerOne(int x0, int y0, int x1, int y1);
void IncrVerOnePure(int x0, int y0, int x1, int y1);
void IncrVerTwo(int x0, int y0, int x1, int y1);
void IncrVerTwoPure(int x0, int y0, int x1, int y1);

int classify_octant(int x0, int y0, int x1, int y1);
int BresenhamNASM(int x0, int y0, int x1, int y1);

void emit_brute_lines(void);
void emit_nasm_lines(void);
void emit_inc_ver1_lines(void);
void emit_inc_ver2_lines(void);
void advance_stage(int value);

double elapsed_time(
    struct timespec start,
    struct timespec end
);

double benchmark_brute_graphics(void);
double benchmark_inc_ver1_graphics(void);
double benchmark_inc_ver2_graphics(void);
double benchmark_nasm_graphics(void);

void draw_scene(void);

int main(int argc, char *argv[]) {
    // Revisar que los argumentos sean válidos
    if (argc != 4) {
        fprintf(
            stderr,
            "Uso: %s <resolucion> <numero_lineas> <repeticiones>\n",
            argv[0]
        );

        return EXIT_FAILURE;
    }

    resolution = atoi(argv[1]);
    lines_num = atoi(argv[2]);
    quantity = atoi(argv[3]);

    if (resolution <= 0 || lines_num <= 0 || quantity <= 0) {
        fprintf(
            stderr,
            "Todos los argumentos deben ser enteros positivos.\n"
        );

        return EXIT_FAILURE;
    }

    // La función que liberará lines cuando el programa termine
    if (atexit(cleanup) != 0) {
        fprintf(
            stderr,
            "No se pudo registrar la función de limpieza.\n"
        );

        return EXIT_FAILURE;
    }

    generate_lines();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(resolution, resolution);
    glutCreateWindow("Lineas Rectas");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glPointSize(1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0.0, (double)resolution, 0.0, (double)resolution);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutDisplayFunc(draw_scene);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return EXIT_SUCCESS;
}

// Limpiar memoria
void cleanup(void) {
    free(lines);
    lines = NULL;
}

// Generar las líneas
void generate_lines(void) {
    lines = malloc((size_t)lines_num * sizeof(LINE));

    if (lines == NULL) {
        fprintf(stderr, "No se pudo reservar memoria para las líneas.\n");
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)time(NULL));

    for (int i = 0; i < lines_num; i++) {
        lines[i].x0 = rand() % resolution;
        lines[i].y0 = rand() % resolution;
        lines[i].x1 = rand() % resolution;
        lines[i].y1 = rand() % resolution;
    }
}

// Dibujar pixeles
void plot(int col, int row) {
    glVertex2f(col + 0.5f, row + 0.5f);
}

void plotPure(int col, int row) {
    return;
}

// Redimensionar
void reshape(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0.0, (double)resolution, 0.0, (double)resolution);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Medir el tiempo
double elapsed_time(struct timespec start, struct timespec end) {
    double seconds = (double)(end.tv_sec - start.tv_sec);

    double nanoseconds = (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;

    return seconds + nanoseconds;
}

// Esperar cierto tiempo antes de mostrar el siguiente algoritmo
void advance_stage(int value) {
    (void)value;

    if (display_stage < 4) {
        display_stage++;

        glutPostRedisplay();
    }
}

// - - - - - - - - -  - ALGORITMOS - - - - - - - - - - -
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

void BrutePure(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    // Si solo es un punto
    if (dx == 0 && dy == 0) {
        plotPure(x0, y0);
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

            plotPure(x, (int)roundl(y));

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

            plotPure((int)roundl(x), y);

            if (y == y1) {
                break;
            }
        }
    }
}

// Algoritmos Incrementales
int max(int a, int b){
  if (a>=b) return a;
  else return b;
}

void swapPoints(int *x0, int *y0, int *x1, int *y1){
  int tmp_x = *x1;
  *x1 = *x0;
  *x0 = tmp_x;
  int tmp_y = *y1;
  *y1 = *y0;
  *y0 = tmp_y;
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


void IncrVerOnePure(int x0, int y0, int x1, int y1){
  if (x0 == x1) {
    if (y1 < y0){
      int tmp = y1;
      y1 = y0;
      y0 = tmp;
    }
    for (int y = y0 ; y <= y1; y++){
      plotPure(x0,y);
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
      plotPure(x, roundl(y));
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
      plotPure(roundl(x), y);
      x += m;
    }
  }
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


void IncrVerTwoPure(int x0, int y0, int x1, int y1) {
  long double x,y, paso_x, paso_y;
  int ancho;
  ancho = max(abs(x1-x0), abs(y1-y0));
  paso_x = (x1-x0)/ancho;
  paso_y = (y1-y0)/ancho;
  if (ancho == 0) {
    plotPure(x0,y0);
    return;
  }
  paso_x = (long double)(x1-x0)/ancho;
  paso_y = (long double)(y1-y0)/ancho;
  x = x0;
  y = y0;
  for (int i=0; i <= ancho; i++){
    plotPure(roundl(x),roundl(y));
    x += paso_x;
    y += paso_y;
  }
}

// Algoritmos de Bresenham

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

void oct_1Pure(int x0, int y0, int x1, int y1){
  int Delta_E = 2*(y1 - y0);
  int Delta_Ne = 2*((y1 - y0) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;
  plotPure(xp,yp);
  
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
    plotPure(xp,yp);
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

void oct_2Pure(int x0, int y0, int x1, int y1){
  int Delta_N = 2*(x0 - x1);
  int Delta_Ne = 2*((y1 - y0) - (x1 - x0));

  int xp = x0;
  int yp = y0;
  plotPure(xp,yp);

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
    plotPure(xp,yp);
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

void oct_3Pure(int x0, int y0, int x1, int y1){
  int Delta_N = 2*(x0 - x1);
  int Delta_NO = 2*((y0 - y1) + (x0 - x1));

  int xp = x0;
  int yp = y0;
  plotPure(xp,yp);

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
    plotPure(xp,yp);
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

void oct_4Pure(int x0, int y0, int x1, int y1){
  int Delta_O = 2*(y0 - y1);
  int Delta_NO = 2*((y0 - y1) + (x0 - x1));

  int xp = x0;
  int yp = y0;
  plotPure(xp,yp);

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
    plotPure(xp,yp);
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

void oct_5Pure(int x0, int y0, int x1, int y1){
  int Delta_O = 2*(y1 - y0);
  int Delta_SO =  2*((y1 - y0) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;
  plotPure(xp,yp);
  
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
    plotPure(xp,yp);
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

void oct_6Pure(int x0, int y0, int x1, int y1){
  int Delta_S = 2*(x0 - x1);
  int Delta_SO =  2*((x0 - x1) - (y0 - y1));
  
  int xp = x0;
  int yp = y0;
  plotPure(xp,yp);
  
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
    plotPure(xp,yp);
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

void oct_7Pure(int x0, int y0, int x1, int y1){
  int Delta_S = 2*(x1-x0);
  int Delta_SE = 2*((x1-x0) - (y0 - y1));
  
  int xp = x0;
  int yp = y0;
  
  plotPure(xp,yp);

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
    plotPure(xp,yp);
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

void oct_8Pure(int x0, int y0, int x1, int y1){
  int Delta_E = 2*(y0 - y1);
  int Delta_SE = 2*((y0-y1) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;

  plotPure(xp,yp);

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
    plotPure(xp,yp);
  } 
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

int BresenhamNASMPure(int x0, int y0, int x1, int y1) {
    int octant = classify_octant(x0, y0, x1, y1);

    switch (octant) {
        case 1:
            oct1_pure(x0, y0, x1, y1);
            return 1;

        case 2:
            oct2_pure(x0, y0, x1, y1);
            return 1;

        case 3:
            oct3_pure(x0, y0, x1, y1);
            return 1;

        case 4:
            oct4_pure(x0, y0, x1, y1);
            return 1;

        case 5:
            oct5_pure(x0, y0, x1, y1);
            return 1;
        
        case 6:
            oct6_pure(x0, y0, x1, y1);
            return 1;
        
        case 7:
            oct7_pure(x0, y0, x1, y1);
            return 1;

        case 8:
            oct8_pure(x0, y0, x1, y1);
            return 1;

        default:
            return 0;
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

int BresenhamCPure(int x0, int y0, int x1, int y1) {
    int octant = classify_octant(x0, y0, x1, y1);

    switch (octant) {
        case 1:
            oct_1Pure(x0, y0, x1, y1);
            return 1;

        case 2:
            oct_2Pure(x0, y0, x1, y1);
            return 1;

        case 3:
            oct_3Pure(x0, y0, x1, y1);
            return 1;

        case 4:
            oct_4Pure(x0, y0, x1, y1);
            return 1;

        case 5:
            oct_5Pure(x0, y0, x1, y1);
            return 1;
        
        case 6:
            oct_6Pure(x0, y0, x1, y1);
            return 1;
        
        case 7:
            oct_7Pure(x0, y0, x1, y1);
            return 1;

        case 8:
            oct_8Pure(x0, y0, x1, y1);
            return 1;

        default:
            return 0;
    }
}

// Mostrar lineas - Fuerza bruta
void emit_brute_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        Brute(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

void emit_brutePure_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        BrutePure(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

// Mostrar lineas - Inc Ver 1
void emit_inc_ver1_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        IncrVerOne(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

void emit_inc_ver1Pure_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        IncrVerOnePure(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

// Mostrar lineas - Inc Ver 2
void emit_inc_ver2_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        IncrVerTwo(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

void emit_inc_ver2Pure_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        IncrVerTwoPure(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

// Mostrar lineas - Bresenham NASM
void emit_bresenham_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        BresenhamC(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

void emit_bresenhamPure_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        BresenhamCPure(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

// Mostrar lineas - Bresenham NASM
void emit_nasm_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        BresenhamNASM(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

void emit_nasmPure_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        BresenhamNASMPure(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

// Medir tiempo - Fuerza Bruta
double benchmark_brute_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0; repetition < quantity; repetition++) {

        emit_brute_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

double benchmark_brutePure_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0; repetition < quantity; repetition++) {

        emit_brutePure_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

// Medir tiempo - Inc Ver1


// Medir tiempo - Inc Ver 1
double benchmark_inc_ver1_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0; repetition < quantity; repetition++) {

        emit_inc_ver1_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

double benchmark_inc_ver1Pure_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0; repetition < quantity; repetition++) {

        emit_inc_ver1Pure_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

// Medir tiempo - Inc Ver 2
double benchmark_inc_ver2_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0; repetition < quantity; repetition++) {

        emit_inc_ver2_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

double benchmark_inc_ver2Pure_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0; repetition < quantity; repetition++) {

        emit_inc_ver2Pure_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

// Medir tiempo - Bresenham NASM
double benchmark_bresenham_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0;
         repetition < quantity;
         repetition++) {

        emit_bresenham_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

double benchmark_bresenhamPure_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0;
         repetition < quantity;
         repetition++) {

        emit_bresenhamPure_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

// Medir tiempo - Bresenham NASM
double benchmark_nasm_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0;
         repetition < quantity;
         repetition++) {

        emit_nasm_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

double benchmark_nasmPure_graphics(void) {
    struct timespec start;
    struct timespec end;

    // Espera cualquier trabajo anterior antes de comenzar a medir
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &start);

    glBegin(GL_POINTS);

    for (int repetition = 0;
         repetition < quantity;
         repetition++) {

        emit_nasmPure_lines();
    }

    glEnd();
    // Garantiza que OpenGL terminó de procesar todos los puntos
    glFinish();

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

// Pintar los algoritmos
void draw_scene(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    /*
     * ETAPA 0: Fuerza Bruta
     * Siempre debe reconstruirse porque glClear()
     * borró la ventana.
     */
    // Rojo
    glColor3f(1.0f, 0.0f, 0.0f);

    if (!brute_measured) {
        double brute_time =
            benchmark_brute_graphics();

        printf(
            "Fuerza Bruta con dibujo: %.9f segundos\n",
            brute_time
        );

        brute_measured = 1;

        // Tiempo de espera
        glutTimerFunc(2000, advance_stage, 0);
    }
    else {
        glBegin(GL_POINTS);
        emit_brute_lines();
        glEnd();
    }

    // Si todavía estamos en la etapa 0,mostrar únicamente Fuerza Bruta
    if (display_stage == 0) {
        glFlush();
        return;
    }

    // ETAPA 1: Incremental versión 1
    // Cyan
    glColor3f(0.0f, 1.0f, 1.0f);

    if (!inc_ver_one_measured) {
        double inc_ver1_time =
            benchmark_inc_ver1_graphics();

        printf(
            "Incremental Version 1 con dibujo: %.9f segundos\n",
            inc_ver1_time
        );

        double inc_ver1Pure_time =
            benchmark_brutePure_graphics();
        printf(
            "Incremental Version 1 sin dibujo(Puro): %.9f segundos\n",
            inc_ver1Pure_time
        );

        inc_ver_one_measured = 1;

        

        // Tiempo de espera
        glutTimerFunc(2000, advance_stage, 0);
    }
    else {
        glBegin(GL_POINTS);
        emit_inc_ver1_lines();
        glEnd();
    }

    if (display_stage == 1) {
        glFlush();
        return;
    }
    
    // ETAPA 2: Incremental versión 2
    // Amarillo
    glColor3f(1.0f, 1.0f, 0.0f);

    if (!inc_ver_two_measured) {
        double inc_ver2_time =
            benchmark_inc_ver2_graphics();

        printf(
            "Incremental Version 2 con dibujo: %.9f segundos\n",
            inc_ver2_time
        );

        double inc_ver2Pure_time =
            benchmark_inc_ver2Pure_graphics();

        printf(
            "Incremental Version 2 sin dibujo(Pure): %.9f segundos\n",
            inc_ver2Pure_time
        );

        inc_ver_two_measured = 1;

        // Tiempo de espera
        glutTimerFunc(2000, advance_stage, 0);
    }
    else {
        glBegin(GL_POINTS);
        emit_inc_ver2_lines();
        glEnd();
    }

    if (display_stage == 2) {
        glFlush();
        return;
    }

    // ETAPA 3: Bresenham escrito en C
    // Magenta
    glColor3f(1.0f, 0.0f, 1.0f);

    if (!bresenham_c_measured) {
        double bresenham_c_time =
            benchmark_bresenham_graphics();

        printf(
            "Bresenham C con dibujo: %.9f segundos\n",
            bresenham_c_time
        );

        double bresenhamPure_c_time =
            benchmark_bresenhamPure_graphics();

        printf(
            "Bresenham C sin dibujo (Pure): %.9f segundos\n",
            bresenhamPure_c_time
        );


        bresenham_c_measured = 1;

        // Tiempo de espera
        glutTimerFunc(2000, advance_stage, 0);
    }
    else {
        glBegin(GL_POINTS);
        emit_bresenham_lines();
        glEnd();
    }

    if (display_stage == 3) {
        glFlush();
        return;
    }

    // ETAPA 4: Bresenham escrito en NASM
    // Verde
    glColor3f(0.0f, 1.0f, 0.0f);
    if (!nasm_measured) {
        double nasm_time =
            benchmark_nasm_graphics();

        printf(
            "Bresenham NASM con dibujo: %.9f segundos\n",
            nasm_time
        );

        double nasmPure_time =
            benchmark_nasmPure_graphics();

        printf(
            "Bresenham NASM sin dibujo(Pure): %.9f segundos\n",
            nasmPure_time
        );

        nasm_measured = 1;
    }
    else {
        glBegin(GL_POINTS);
        emit_nasm_lines();
        glEnd();
    }

    glFlush();
}

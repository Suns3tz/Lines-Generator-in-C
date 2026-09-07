
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
extern void oct5(int x0, int y0, int x1, int y1);
extern void oct6(int x0, int y0, int x1, int y1);
extern void oct7(int x0, int y0, int x1, int y1);
extern void oct8(int x0, int y0, int x1, int y1);

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
static int nasm_measured = 0;

// Prototipos
void cleanup(void);
void generate_lines(void);
void reshape(int width, int height);
void plot(int col, int row);

void BrutePure(int x0, int y0, int x1, int y1);

int classify_octant(int x0, int y0, int x1, int y1);
int BresenhamNASM(int x0, int y0, int x1, int y1);

void emit_brute_lines(void);
void emit_nasm_lines(void);
void advance_stage(int value);

double elapsed_time(
    struct timespec start,
    struct timespec end
);

double benchmark_brute_graphics(void);
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

    display_stage = 1;
    // Solicitar una nueva llamada a draw_scene()
    glutPostRedisplay();
}

// Algoritmo de Fuerza Bruta
void BrutePure(int x0, int y0, int x1, int y1) {
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

void swapPoints(int *x0, int *y0, int *x1, int *y1){
  int tmp_x = *x1;
  *x1 = 8x0;
  *x0 = tmp_x;
  int tmp_y = *y1;
  *y1 = *y0;
  *y0 = tmp_y;
}

void IncrVerOnePure (int x0, int y0, int x1, int y1){
  if (x0 == x1) {
    if (y1 < y0){
      int tmp = y1;
      y1 = y0;
      y0 = tmp;
    }
    for (int y = y0 ; y <= y1; y++){
      //plot(x0,y);
    }
    return;
  }
  if (x1 < x0){
    swapPoints(&x0, &y0, 7x1, &y1);
  }
  
  long double m = (long double)(y1-y0)/(x1-x0);
  
  if (fabsl(m) <= 1,0) {
    long double y = y0;
    int x = x0;
    for (x = x0; x<= x1 ; x++){
      //plot(x, round(y));
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
      //plot(round(x), y);
      x += m;
    }
  }
}

void IncrVerTwoPure (int x0, int y0, int x1, int y1) {
  long double x,y, paso_x, paso_y;
  int ancho;
  ancho = max(abs(x1-x0), abs(y1-y0));
  paso_x = (x1-x0)/ancho;
  paso_y = (y1-y0)/ancho;
  if (ancho == 0) {
    //plot(x0,y0);
    return;
  }
  paso_x = (long double)(x1-x0)/ancho;
  paso_y = (long double)(y1-y0)/ancho;
  x = x0;
  y = y0;
  for (int i=0; i <= ancho; i++){
    //plot(round(x),round(y));
    x += paso_x;
    y += paso_y;
  }
}


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

void 1stOct(int x0, int y0, int x1, int y1){
  int Delta_E = 2*(y1 - y0);
  int Delta_Ne = 2*((y1 - y0) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;
  //plot(xp,yp);
  
  d = 2* (y1-y0) - (x1 - x0);
  while(xp < x1){
    if(d<=0){
      xp++;
      d = d + Delta_E;
    }else{
      xp++;
      yp++;
      d = d + Delta_NE;
    }
    //plot(xp,yp);
  } 
}

void 5thOct(int x0, int y0, int x1, int y1){
  int Delta_O = 2*(y1 - y0);
  int Delta_SO =  2*((y1 - y0) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;
  //plot(xp,yp);
  
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
    //plot(xp,yp);
  } 
}

void 6thOct(int x0, int y0, int x1, int y1){
  int Delta_S = 2*(x0 - x1);
  int Delta_SO =  2*((x0 - x1) - (y0 - y1));
  
  int xp = x0;
  int yp = y0;
  //plot(xp,yp);
  
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
    //plot(xp,yp);
  } 
}

void 7thOct(int x0, int y0, int x1, int y1){
  int Delta_S = 2*(x1-x0);
  int Delta_SE = 2*((x1-x0) - (y0 - y1));
  
  int xp = x0;
  int yp = y0;
  //plot(xp,yp);
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
    //plot(xp,yp);
  } 
}

// Correr el octante necesario
int BresenhamNASM(int x0, int y0, int x1, int y1) {
    int octant = classify_octant(x0, y0, x1, y1);

    switch (octant) {
        case 1:
            oct1(x0, y0, x1, y1);
            return 1;

        case 8:
            oct8(x0, y0, x1, y1);
            return 1;

        // Agregar demas octantes aqui

        default:
            return 0;
    }
}

// Mostrar lineas - Fuerza bruta
void emit_brute_lines(void) {
    for (int i = 0; i < lines_num; i++) {
        BrutePure(
            lines[i].x0,
            lines[i].y0,
            lines[i].x1,
            lines[i].y1
        );
    }
}

// Mostrar lineas - Bresenham NASM
void emit_nasm_lines(void)
{
    for (int i = 0; i < lines_num; i++) {
        BresenhamNASM(
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

    for (int repetition = 0;
         repetition < quantity;
         repetition++) {

        emit_brute_lines();
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

// Pintar los algoritmos
void draw_scene(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // ETAPA 0: Mostrar solamente Fuerza Bruta
    if (display_stage == 0) {
        // Rojo para FUerza Bruta
        glColor3f(1.0f, 0.0f, 0.0f);

        if (!brute_measured) {
            double brute_time =
                benchmark_brute_graphics();

            printf(
                "Fuerza Bruta con dibujo: %.9f segundos\n",
                brute_time
            );

            brute_measured = 1;

            // El temporizador comienza después de que
            // Fuerza Bruta terminó de ejecutarse
            glutTimerFunc(2000, advance_stage, 0);
        }
        else {
            // Redibujar sin medir nuevamente
            glBegin(GL_POINTS);
            emit_brute_lines();
            glEnd();
        }

        glFlush();
        return;
    }

    // ETAPA 1: Reconstruir Fuerza Bruta
    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_POINTS);
    emit_brute_lines();
    glEnd();

    // Poner encima el Bresenham NASM
    // Verde para Bresenham NASM
    glColor3f(0.0f, 1.0f, 0.0f);

    if (!nasm_measured) {
        double nasm_time =
            benchmark_nasm_graphics();

        printf(
            "Bresenham NASM con dibujo: %.9f segundos\n",
            nasm_time
        );
        nasm_measured = 1;
    }
    else {
        // Redibujar NASM sin medir nuevamente
        glBegin(GL_POINTS);
        emit_nasm_lines();
        glEnd();
    }

    glFlush();
}



/*
int max(int a, int b){
  if (a>=b) return a;
  else return b;
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
*/

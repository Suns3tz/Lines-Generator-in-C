
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

// Prototipos
void plot(int col, int row);
void BrutePure(int x0, int y0, int x1, int y1);
void draw_scene(void);
void reshape(int width, int height);
void generate_lines(void);
double elapsed_time(struct timespec start, struct timespec end);


int main(int argc, char *argv[])
{
    // Validación de argumentos dados
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

    if (resolution <= 0 ||
        lines_num <= 0 ||
        quantity <= 0) {

        fprintf(
            stderr,
            "Todos los argumentos deben ser enteros positivos.\n"
        );

        return EXIT_FAILURE;
    }

    generate_lines();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(resolution, resolution);
    glutCreateWindow("Lineas Rectas");

    glClearColor(
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    glPointSize(1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(
        0.0,
        (double)resolution,
        0.0,
        (double)resolution
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutDisplayFunc(draw_scene);
    glutReshapeFunc(reshape);

    glutMainLoop();

    free(lines);

    return EXIT_SUCCESS;
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

    gluOrtho2D(
        0.0,
        (double)resolution,
        0.0,
        (double)resolution
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Medir el tiempo
double elapsed_time(struct timespec start, struct timespec end) {
    double seconds = (double)(end.tv_sec - start.tv_sec);

    double nanoseconds = (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;

    return seconds + nanoseconds;
}

// Algoritmo de Fuerza Bruta
void BrutePure(int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    // La línea es de un solo punto
    if (dx == 0 && dy == 0) {
        plot(x0, y0);
        return;
    }

    // Si el cambio horizontal es mayor, se recorre x
    if (abs(dx) >= abs(dy)) {
        int step_x = (dx > 0) ? 1 : -1;

        long double m = (long double)dy / (long double)dx;

        long double b = (long double)y0 - m * (long double)x0;

        for (int x = x0; ; x += step_x) {
            long double y = m * x + b;

            plot(x, (int)roundl(y));

            if (x == x1) {
                break;
            }
        }
    }

    // Si el cambio vertical es mayor, se recorre y
    else {
        int step_y = (dy > 0) ? 1 : -1;

        long double inverse_m = (long double)dx / (long double)dy;

        for (int y = y0; ; y += step_y) {
            long double x = x0 + inverse_m * (y - y0);

            plot((int)roundl(x), y);

            if (y == y1) {
                break;
            }
        }
    }
}

// Dibujar los algoritmos
void draw_scene(void) {
    static int first_execution = 1;
    glClear(GL_COLOR_BUFFER_BIT);

    // Fuerza Bruta: color rojo
    glColor3f(1.0f, 0.0f, 0.0f);

    if (first_execution) {
        struct timespec start;
        struct timespec end;

        clock_gettime(CLOCK_MONOTONIC, &start);

        glBegin(GL_POINTS);

        for (int repetition = 0; repetition < quantity; repetition++) {

            for (int i = 0; i < lines_num; i++) {
                BrutePure(
                    lines[i].x0,
                    lines[i].y0,
                    lines[i].x1,
                    lines[i].y1
                );
            }
        }

        glEnd();

        // Espera a que OpenGL termine de procesar las
        // operaciones antes de detener el reloj.
        glFinish();

        clock_gettime(CLOCK_MONOTONIC, &end);

        printf(
            "Fuerza Bruta con dibujo: %.9f segundos\n",
            elapsed_time(start, end)
        );

        first_execution = 0;
    } else {
        // Si GLUT solicita volver a mostrar la ventana,
         // se dibujan las líneas una sola vez
        glBegin(GL_POINTS);

        for (int i = 0; i < lines_num; i++) {
            BrutePure(
                lines[i].x0,
                lines[i].y0,
                lines[i].x1,
                lines[i].y1
            );
        }

        glEnd();
        glFlush();
    }
}

/*
int max(int a, int b){
  if (a>=b) return a;
  else return b;
}

// Algoritmo de Fuerza Bruta
void BrutePure (int x0, int y0, int x1, int y1) {
  long double m, b, y;
  int i;
  m = (long double)(y1 - y0) / (long double)(x1 - x0);
  b = y0 - m*x0;
  
  for(int i = x0; i <= x1; i++){
    y = m*i + b;
    plot(i, round(y));
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
*/


#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "line.h"
#include "plots.h"
#include "algorithms.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

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
void draw_scene(void);

double elapsed_time(
    struct timespec start,
    struct timespec end
);

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

// - - - - - MOSTRAR LAS LINEAS - - - - -

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

// - - - - - MEDIR TIEMPOS - - - - -

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

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int repetition = 0; repetition < quantity; repetition++) {

        emit_brutePure_lines();
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

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

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int repetition = 0; repetition < quantity; repetition++) {

        emit_inc_ver1Pure_lines();
    }

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

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int repetition = 0; repetition < quantity; repetition++) {

        emit_inc_ver2Pure_lines();
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

// Medir tiempo - Bresenham en C
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

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int repetition = 0;
         repetition < quantity;
         repetition++) {

        emit_bresenhamPure_lines();
    }

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

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int repetition = 0;
         repetition < quantity;
         repetition++) {

        emit_nasmPure_lines();
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_time(start, end);
}

// - - - - - PINTAR - - - - -
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
        double brutePure_time =
            benchmark_brutePure_graphics();
        printf(
            "Fuerza Bruta sin dibujo(Puro): %.9f segundos\n",
            brutePure_time
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
            benchmark_inc_ver1Pure_graphics();
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

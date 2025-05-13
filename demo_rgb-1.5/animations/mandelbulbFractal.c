#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include <stdio.h>
#include <math.h>

#define MAX_ITER 20
#define N 8
#define POINTS_OFFSET 100
#define DIM 128
#define MAX_POINTS (DIM * DIM * DIM)

extern GLfloat _dim[];

static void init(void);
static void draw(void);
static void sortie(void);

void initPoints();

typedef struct {
  float x, y, z;
  int depth;
} MandelPosition;

static GLuint _pId = 0;
static GLuint _vao = 0;
static GLuint _vbo = 0;

static MandelPosition points[MAX_POINTS];
static int point_count = 0;

static float zTranslation = -400.0f;

void mandelbulbFractal(int state) {
  switch(state) {
    case GL4DH_INIT:
      init();
      return;
    case GL4DH_FREE:
      sortie();
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      return;
    default:
      draw();
      return;
  }
}

void init(void) {  
  _pId = gl4duCreateProgram("<vs>shaders/mandelbulbFractal.vs", "<fs>shaders/mandelbulbFractal.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelView");

  // Appliquer une matrice de projection, elle sera envoyé lors de l'appel à sendMatrices()
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _dim[1]) / _dim[0];
  GLfloat top    = (1.0f * _dim[1]) / _dim[0];
  gl4duFrustumf(-1.0f, 1.0f, bottom, top, 1.0f, 1000.0f);

  initPoints();

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, point_count * sizeof(MandelPosition), points, GL_STATIC_DRAW);

  // Lire les 3 positions xyz dans l'attribut location = 0 (pos) du vertex shader 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MandelPosition), (const void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, sizeof(MandelPosition), (const void*)(3 * sizeof(int)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

static double get_dt(void) {
  static double t0 = 0.0f;
  double t = gl4dGetElapsedTime();
  double dt = (t - t0) / 60.0f;
  t0 = t;
  return dt;
}

void draw(void) {
  GLboolean wasDepthEnabled = glIsEnabled(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_TEST);
  
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(_pId);
  
  double dt = get_dt();
  zTranslation += dt;

  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.0f, zTranslation);
  gl4duSendMatrices();
  
  glBindVertexArray(_vao);
  glPointSize(2.0f);
  glDrawArrays(GL_POINTS, 0, point_count);
  glBindVertexArray(0);

  glUseProgram(0);

  if (!wasDepthEnabled) {
    glDisable(GL_DEPTH_TEST);
  }
}

void sortie(void) {
  if(_vao) {
    glDeleteVertexArrays(1, &_vao);
  }
  if(_vbo) {
    glDeleteBuffers(1, &_vbo);
  }
}

int map(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void initPoints() {
  for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
      int edge = 0;
      int lastIteration = 0;
      for (int k = 0; k < DIM; k++) {
        // Normaliser les positions à l'echelle -1 1
        float x = 2.0f * ((float)i / DIM) - 1.0f;
        float y = 2.0f * ((float)j / DIM) - 1.0f;
        float z = 2.0f * ((float)k / DIM) - 1.0f;

        float zetaX = 0, zetaY = 0, zetaZ = 0;
        int iteration = 0;

        while (1) {
          // transformer zeta en coordonnées polaires
          float zetaRadius = sqrtf(zetaX * zetaX + zetaY * zetaY + zetaZ * zetaZ);
          float zetaTheta  = atan2f(sqrtf(zetaX * zetaX + zetaY * zetaY), zetaZ);
          float zetaPhi    = atan2f(zetaY, zetaX);

          float newX = powf(zetaRadius, N) * sinf(zetaTheta * N) * cosf(zetaPhi * N);
          float newY = powf(zetaRadius, N) * sinf(zetaTheta * N) * sinf(zetaPhi * N);
          float newZ = powf(zetaRadius, N) * cosf(zetaTheta * N);
          
          zetaX = newX + x;
          zetaY = newY + y;
          zetaZ = newZ + z;
          
          iteration++;
          
          if (zetaRadius > 2.0f) {
            lastIteration = iteration;
            if (edge == 1) {
              edge = 0;
            }
            break;
          }

          if (iteration == MAX_ITER) {
            if (edge == 0 && point_count < MAX_POINTS) {
              edge = 1;
              int depth = map(lastIteration, 0, MAX_ITER, 0, 255);
              points[point_count++] = (MandelPosition) {
                x * POINTS_OFFSET,
                y * POINTS_OFFSET,
                z * POINTS_OFFSET,
                depth 
              };
            }
            break;
          }
        }
      }
    }
  }
}
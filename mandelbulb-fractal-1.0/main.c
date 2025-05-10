#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>

#define DIM 128
#define MAX_ITER 20
#define N 8
#define MAX_POINTS (DIM * DIM * DIM)

static void init(void);
static void draw(void);
static void sortie(void);

void initPoints();
void keyboard(int key);

static GLuint _wW = 1080, _wH = 720;
static GLuint _pId = 0;
static GLuint _vao = 0;
static GLuint _vbo = 0;

typedef struct {
  float x, y, z;
  int iteration;
} MandelPosition;

static MandelPosition points[MAX_POINTS];
static int point_count = 0;

static float cameraZ = -170.0f;

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Fractal Mandelbulb", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init();
  atexit(sortie);
  gl4duwDisplayFunc(draw);
  gl4duwKeyDownFunc(keyboard);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelView");

  // Appliquer une matrice de projection, elle sera envoyé lors de l'appel à sendMatrices()
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _wH) / _wW;
  GLfloat top    = (1.0f * _wH) / _wW;
  gl4duFrustumf(-1.0f, 1.0f, bottom, top, 1.0f, 1000.0f);

  initPoints();

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, point_count * sizeof(MandelPosition), points, GL_STATIC_DRAW);

  // lire les 3 positions xyz dans l'attribut location = 0 (pos) du vertex shader 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MandelPosition), (const void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);
  
  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.0f, cameraZ);
  gl4duSendMatrices();
  
  glBindVertexArray(_vao);
  glPointSize(2.0f); // pour que les points soient visibles
  glDrawArrays(GL_POINTS, 0, point_count);
  glBindVertexArray(0);

  glUseProgram(0);
}

void sortie(void) {
  if(_vao) {
    glDeleteVertexArrays(1, &_vao);
  }
  if(_vbo) {
    glDeleteBuffers(1, &_vbo);
  }
  gl4duClean(GL4DU_ALL);
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

          if (iteration > MAX_ITER) {
            if (edge == 0 && point_count < MAX_POINTS) {
              edge = 1;
              points[point_count++] = (MandelPosition) {
                x * 100,
                y * 100,
                z * 100,
                lastIteration 
              };
            }
            break;
          }
        }
      }
    }
  }
}

void keyboard(int key) {
  switch (key) {
    case SDLK_UP:
    case SDLK_w:
      cameraZ += 5.0f; // se rapproche
      break;
    case SDLK_DOWN:
    case SDLK_s:
      cameraZ -= 5.0f; // s’éloigne
      break;
  }
}
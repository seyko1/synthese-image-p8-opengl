#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 1920, _wH = 1080;
static GLuint _quadId = 0;
static GLuint _pId = 0;

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Spherical Inversion", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init();
  atexit(sortie);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  
  _pId = gl4duCreateProgram("<vs>shaders/inversion.vs", "<fs>shaders/inversion.fs", NULL);
  _quadId = gl4dgGenQuadf();
  
  gl4duGenMatrix(GL_FLOAT, "modelView");
  gl4duGenMatrix(GL_FLOAT, "projection");
  
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _wH) / _wW;
  GLfloat top    = (1.0f * _wH) / _wW;
  gl4duFrustumf(-1.0f, 1.0f, bottom, top, 1.0f, 100.0f);
}

void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.0f, -1.0f);
  gl4duSendMatrices();

  float t = gl4dGetElapsedTime() / 1000.0f;
  glUniform1f(glGetUniformLocation(_pId, "elapsed"), t);
  glUniform2f(glGetUniformLocation(_pId, "resolution"), (float)_wW, (float)_wH);

  gl4dgDraw(_quadId);

  glUseProgram(0);
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
}

#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>

static void init(void);
static void draw(void);
static void sortie(void);

extern void initNoiseTextures(void);
extern void useNoiseTextures(GLuint pid, int shift);
extern void unuseNoiseTextures(int shift);
extern void freeNoiseTextures(void);
static void keydown(int keycode);

static GLuint _wW = 1920, _wH = 1080;
static GLuint _gridId = 0;
static GLuint _pId = 0;
static GLenum _polygonMode = GL_FILL;
static GLfloat _speed = 3.0f;

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Flying Trip", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init();
  atexit(sortie);
  gl4duwDisplayFunc(draw);
  gl4duwKeyDownFunc(keydown);
  gl4duwMainLoop();
  return 0;
}

static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_DOWN:
    _speed -= 0.1;
    break;
  case SDLK_UP:
    _speed += 0.1;
    break;
    case ' ':
    _polygonMode = (_polygonMode == GL_FILL) ? GL_LINE : GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, _polygonMode);
    break;
  default:
    break;
  }
}

void init(void) {
  initNoiseTextures();
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  _gridId = gl4dgGenGrid2df(_wW / 5, _wH / 5);
  // Créer un programme shader à partir de hello.vs et hello.fs, qui pourra s'occuper du rendu.
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelView");

  // Appliquer une matrice de projection, elle sera envoyé lors de l'appel à sendMatrices()
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _wH) / _wW;
  GLfloat top    = (1.0f * _wH) / _wW;
  gl4duFrustumf(-1.0f * 0.1f, 1.0f * 0.1f, bottom * 0.1f, top * 0.1f, 0.1f, 100.0f);

  // Afficher le cube en file de fer
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void draw(void) {
  float time = gl4dGetElapsedTime() / 1000.0f;
  // printf("elapsed %f\n", time);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);
  glUniform1f(glGetUniformLocation(_pId, "time"), time);
  glUniform1f(glGetUniformLocation(_pId, "speed"), _speed);

  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duLookAtf(0.0f, 1.0f, 1.0f, 0.0f, 0.7f, 0.0f, 0.0f, 1.0f, 0.0f);
  gl4duScalef(7.0f, 1.0f, 6.0f);
  gl4duSendMatrices();
  
  useNoiseTextures(_pId, 0);
  gl4dgDraw(_gridId);
  unuseNoiseTextures(0);
  
  glUseProgram(0);
}

void sortie(void) {
  freeNoiseTextures();
  gl4duClean(GL4DU_ALL);
}

#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>

#define NB_CUBES 10

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 640, _wH = 480;
static GLuint _cubeId = 0;
static GLuint _pId = 0;

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Ateliers API8 - modélisation", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
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
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
  _cubeId = gl4dgGenCubef();
  // Créer un programme shader à partir de hello.vs et hello.fs, qui pourra s'occuper du rendu.
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelView");

  // Appliquer une matrice de projection, elle sera envoyé lors de l'appel à sendMatrices()
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _wH) / _wW;
  GLfloat top    = (1.0f * _wH) / _wW;
  gl4duFrustumf(-1.0f, 1.0f, bottom, top, 1.0f, 100.0f);

  // Afficher le cube en file de fer
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  
  // -2 sur les z car notre frustum a un near à 1 donc on commence à voir non plus à 0 mais -1 (éloignement)
  // on éloigne à -3 quand on passe à un cube car il a une profondeur (+1 en avant -1 en arrière)
  gl4duTranslatef(0.0f, 0.0f, -3.0f);
  
  for (int i = 0; i < NB_CUBES; ++i) {
    // Ramener i dans la plage [-1, 1] 
    GLfloat x = 2.0f * (i / (NB_CUBES - 1.0f)) - 1.0f;
    gl4duPushMatrix();
    gl4duTranslatef(x, 0.0f, 0.0f);
    
    for (int j = 0; j < NB_CUBES; ++j) {
      GLfloat y  = 2.0f * (j / (NB_CUBES - 1.0f)) - 1.0f;
      gl4duPushMatrix();
      gl4duTranslatef(0.0, y, 0.0f);
      
      for (int k = 0; k < NB_CUBES; ++k) {
        GLfloat z = 2.0f * (k / (NB_CUBES - 1.0f)) - 1.0f;
        gl4duPushMatrix();
        gl4duTranslatef(0.0f, 0.0f, z);
        // placer le scale en bout de chaine pour éviter qu'il n'agissse sur les translations précédentes
        gl4duScalef(0.02f, 0.02f, 0.02f);
        gl4duSendMatrices();
        gl4duPopMatrix();
        gl4dgDraw(_cubeId);
      }
      gl4duPopMatrix();
    }
    gl4duPopMatrix();
  }
  gl4duPopMatrix();

  gl4duSendMatrices();

  glUseProgram(0);
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
}

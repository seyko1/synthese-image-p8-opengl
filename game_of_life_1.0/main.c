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

static GLuint _wW = 1024, _wH = 768;
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
  glEnable(GL_DEPTH_TEST);

  _cubeId = gl4dgGenCubef();
  // Créer un programme shader à partir de hello.vs et hello.fs, qui pourra s'occuper du rendu.
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "view");
  gl4duGenMatrix(GL_FLOAT, "model");

  // Appliquer une matrice de projection, elle sera envoyé lors de l'appel à sendMatrices()
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _wH) / _wW;
  GLfloat top    = (1.0f * _wH) / _wW;
  gl4duFrustumf(-1.0f, 1.0f, bottom, top, 1.0f, 100.0f);

  // Afficher le cube en file de fer
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

static double get_dt(void) {
  static double t0 = 0.0f;
  // Récupérer le temps écoulé en ms entre le démarrage du programme et la frame en cours
  double t = gl4dGetElapsedTime();
  // Calculer le temps écoulé depuis la frame précédente en seconde
  double dt = (t - t0) / 1000.0f;
  t0 = t;
  return dt;
}

void draw(void) {
  static GLfloat rot = 0.0f;
  static const GLfloat rouge[] = { 0.6f, 0.0f, 0.0f , 1.0f }, blanc[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0.0f, 2.5f, -7.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  gl4duBindMatrix("model");
  gl4duLoadIdentityf();

  gl4duTranslatef(0.0f, 1.0f, -3.0f);
  gl4duRotatef(rot, 0.0f, 1.0f, 0.0f);
  GLfloat angle = 180.0f * rot / M_PI;
  
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
        gl4duScalef(0.08f, 0.08f, 0.08f);
        gl4duSendMatrices();
        gl4duPopMatrix();
        GLfloat * couleur = (i + j + k) % 2 ? rouge : blanc;
        glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, couleur);
        gl4dgDraw(_cubeId);
      }
      gl4duPopMatrix();
    }
    gl4duPopMatrix();
  }
  gl4duPopMatrix();

  gl4duSendMatrices();

  glUseProgram(0);
  rot += 3.0f * M_PI * get_dt();
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
}

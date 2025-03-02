#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 640, _wH = 480;
static GLuint _quadId = 0;
static GLuint _coneId = 0;
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

  SDL_GL_SetSwapInterval(1);
  glEnable(GL_DEPTH_TEST);

  _coneId = gl4dgGenConef(3, GL_TRUE);
  _quadId = gl4dgGenQuadf();
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

}

// Renvoyer le delta temps écoulé entre deux appels
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
  static const GLfloat rouge[] = { 0.6f, 0.0f, 0.0f , 1.0f }, bleu[] = { 0.0f, 0.0f, 0.6f, 1.0f };

  static GLfloat rot = 0.0f;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  gl4duBindMatrix("view");
  gl4duLoadIdentityf(); // matrice neutre utilisée pour l'initialisation 
  // Où on est / où on regarde / orientation de la tête
  gl4duLookAtf(0.0f, 2.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  gl4duBindMatrix("model");
  gl4duLoadIdentityf();
  // placer le cone un peu en hauteur
  gl4duTranslatef(0.0f, 1.5f, 0.0f);
  // Convertir rot de radian à degré
  GLfloat angle = 180.0f * rot / M_PI;
  // Rotation autour de l'axe des y 
  gl4duRotatef(angle, 0.0f, 1.0f, 0.0f); 
  // Les modifications de matrices doivent être envoyées au GPU avant de dessinner
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, bleu);
  gl4dgDraw(_coneId);

  // réinitaliser les matrices pour modeliser le quad à part
  gl4duLoadIdentityf();
  // coucher le quad avec une rotation
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  // agrandir le quad pour un effet de sol
  gl4duScalef(5.0f, 5.0f, 5.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, rouge);
  gl4dgDraw(_quadId);

  glUseProgram(0);

  rot += 0.5f * M_PI * get_dt();
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
}

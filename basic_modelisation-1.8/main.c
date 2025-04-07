#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>
#include <SDL_image.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);
static void apply_default_tex(void);

static GLuint _wW = 1024, _wH = 768;
static GLuint _quadId = 0;
static GLuint _coneId = 0;
static GLuint _pId = 0;
static GLuint _texId[3] = { 0 };

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
  SDL_Surface * s = NULL;

  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

  SDL_GL_SetSwapInterval(1);
  glEnable(GL_DEPTH_TEST);

  _coneId = gl4dgGenConef(3, GL_TRUE);
  _quadId = gl4dgGenQuadf();
  // Créer un programme shader à partir de hello.vs et hello.fs, qui pourra s'occuper du rendu.
  _pId = gl4duCreateProgram("<vs>shaders/phong.vs", "<fs>shaders/phong.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "view");
  gl4duGenMatrix(GL_FLOAT, "model");

  // Appliquer une matrice de projection, elle sera envoyé lors de l'appel à sendMatrices()
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _wH) / _wW;
  GLfloat top    = (1.0f * _wH) / _wW;
  gl4duFrustumf(-1.0f, 1.0f, bottom, top, 1.0f, 100.0f);

  /* 8 (taille en octet du tableau) / 4 (taille du premier élément) */
  glGenTextures(sizeof _texId / sizeof *_texId, _texId);
  assert(_texId[0] && _texId[1]);

  // texture pyramide
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  s = IMG_Load("images/pyramide.jpg");
  if (s != NULL) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
  } else {
    apply_default_tex();
  }

  // texture sol
  glBindTexture(GL_TEXTURE_2D, _texId[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  s = IMG_Load("images/sol.jpg");
  if (s != NULL) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
  } else {
    apply_default_tex();
  }

  // texture sol normal map
  glBindTexture(GL_TEXTURE_2D, _texId[2]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  s = IMG_Load("images/sol_nm.jpg");
  if (s != NULL) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
  } else {
    apply_default_tex();  
  }

  glBindTexture(GL_TEXTURE_2D, 0);
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

static void apply_default_tex(void) {
  GLuint p = { (0xFF << 24) | 0xFF }; 
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &p);
}

void draw(void) {
  static const GLfloat rouge[] = { 0.6f, 0.0f, 0.0f , 1.0f }, bleu[] = { 0.0f, 0.0f, 0.6f, 1.0f };
  
  static GLfloat rot = 0.0f;
  GLfloat lumpos[] = { -4.0f, 4.0f, 0.0f, 1.0f };
  // variation entre 3.9 et 0.1
  lumpos[1] = 2.0f + 1.9f * sin(rot);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);

  gl4duBindMatrix("view");
  gl4duLoadIdentityf(); // matrice neutre utilisée pour l'initialisation 
  // Où on est / où on regarde / orientation de la tête
  gl4duLookAtf(0.0f, 2.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

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
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  glUniform1i(glGetUniformLocation(_pId, "useTex"), 1);
  glUniform1i(glGetUniformLocation(_pId, "useNm"), 0);
  gl4dgDraw(_coneId);

  // réinitaliser les matrices pour modeliser le quad à part
  gl4duLoadIdentityf();
  // coucher le quad avec une rotation
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  // agrandir le quad pour un effet de sol
  gl4duScalef(5.0f, 5.0f, 5.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, rouge);
  
  // utilisation de la texture sol et de sa normal map
  glActiveTexture(GL_TEXTURE0 + 1);
  glBindTexture(GL_TEXTURE_2D, _texId[2]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId[1]);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform1i(glGetUniformLocation(_pId, "nm"), 1);
  glUniform1i(glGetUniformLocation(_pId, "useTex"), 1);
  glUniform1i(glGetUniformLocation(_pId, "useNm"), 1);
  
  gl4dgDraw(_quadId);
  
  glBindTexture(GL_TEXTURE_2D, 0);

  glUseProgram(0);

  rot += 0.5f * M_PI * get_dt();
}

void sortie(void) {
  if (_texId[0]) {
    glDeleteTextures(sizeof _texId / sizeof *_texId, _texId);
    _texId[0] = 0;
  }
  gl4duClean(GL4DU_ALL);
}

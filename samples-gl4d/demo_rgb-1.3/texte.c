#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4dh.h>
#include <stdio.h>
#include <math.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 800, _wH = 460;
static GLuint _quadId = 0;
static GLuint _pId = 0;
static GLuint _texId[1] = { 0 };

void texte(int state) {
  /* INITIALISEZ VOS VARIABLES */
  /* ... */
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    init();
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    sortie();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    draw();
    return;
  }
}

void init(void) {
  SDL_Surface * gl_surface = NULL, * ttf_surface = NULL;
  TTF_Font * font = NULL;
  SDL_Color color = { 105, 55, 0, 255 };

  _quadId = gl4dgGenQuadf();
  // Créer un programme shader à partir de hello.vs et hello.fs, qui pourra s'occuper du rendu.
  _pId = gl4duCreateProgram("<vs>shaders/texte.vs", "<fs>shaders/texte.fs", NULL);
  
  // Initialisation de la lib SDL2 ttf
  if (TTF_Init() == -1) {
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
    exit(2);
  }

  // Chargement de la font
  if (!(font = TTF_OpenFont("DejaVuSans-Bold.ttf", 128))) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    exit(2);
  }

  // Création d'une surface SDL avec le texte
  ttf_surface = TTF_RenderUTF8_Blended_Wrapped(font, "Hello World", color, 2048);
  if (ttf_surface == NULL) {
    TTF_CloseFont(font);
    fprintf(stderr, "Erreur lors de la création de la surface de rendu de texte\n");
    exit(2);
  }

  // Création d'une seconde surface aux spécifications qui correspondent au format OpenGL
  gl_surface = SDL_CreateRGBSurface(0, ttf_surface->w, ttf_surface->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  assert(gl_surface);

  // Copie de s vers d
  SDL_BlitSurface(ttf_surface, NULL, gl_surface, NULL);
  SDL_FreeSurface(ttf_surface);

  /* ici commence le transfert vers GL */

  /* taille du tableau en octet / taille du premier élément */
  glGenTextures(sizeof _texId / sizeof *_texId, _texId);
  assert(_texId[0]);

  // texture
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  // transfert effectué ci-dessous
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gl_surface->w, gl_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, gl_surface->pixels);

  glBindTexture(GL_TEXTURE_2D, 0);

  // Libérer
  SDL_FreeSurface(gl_surface);
  TTF_CloseFont(font);

  gl4duGenMatrix(GL_FLOAT, "view");
  gl4duGenMatrix(GL_FLOAT, "model");
  gl4duGenMatrix(GL_FLOAT, "projection_ttf");
  
  gl4duBindMatrix("projection_ttf");
  gl4duLoadIdentityf();
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
  static GLfloat rot = 0.0f;
  
  glClearColor(0.1f, 0.7f, 0.7f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);


  gl4duBindMatrix("view");
  gl4duLoadIdentityf();

  gl4duBindMatrix("model");
  gl4duLoadIdentityf();
  
  gl4duSendMatrices();
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  glUniform1i(glGetUniformLocation(_pId, "useTex"), 1);

  gl4dgDraw(_quadId);

  glUseProgram(0);
 
  rot += 0.5f * M_PI * get_dt();
}

void sortie(void) {
  if (_texId[0]) {
    glDeleteTextures(sizeof _texId / sizeof *_texId, _texId);
    _texId[0] = 0;
  }
}

#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include <stdio.h>
#include <math.h>

#define NB_CUBES 20

/* Seuils de voisinnage pour le jeu de la vie 3D */
#define SURVIVE_MIN 1
#define SURVIVE_MAX 10
#define BIRTH_MIN 1
#define BIRTH_MAX 10

extern GLfloat _dim[];

static void init(void);
static void draw(void);
static void sortie(void);

static void updateGrid();
static int neighboringStates(int x, int y, int z);

static GLuint _cubeId = 0;
static GLuint _pId = 0;

static GLboolean grid[NB_CUBES][NB_CUBES][NB_CUBES];
static GLfloat colorAlive[]       = { 0.6f, 1.0f, 0.5f, 1.0f };
static const GLfloat colorDead[]  = { 1.0f, 1.0f, 1.0f, 0.5f };

static GLint colorUniformLocation;
static GLint isAliveUniformLocation;

void gameoflife3D(int state) {
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
  /* Activer l'alpha pour le mode fil de fer */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const float middle = NB_CUBES / 2;
  const int margin = 2;
  const int min = middle - margin;
  const int max = middle + margin;

  for (int i = 0; i < NB_CUBES; ++i) {
    for (int j = 0; j < NB_CUBES; ++j) {
      for (int k = 0; k < NB_CUBES; ++k) {
        if (
          i >= min && i <= max &&
          j >= min && j <= max &&
          k >= min && k <= max
        ) {
          grid[i][j][k] = GL_TRUE;
        } else {
          grid[i][j][k] = GL_FALSE;
        }
      }
    }
  }
  _cubeId = gl4dgGenCubef();
  // Créer un programme shader à partir de hello.vs et hello.fs, qui pourra s'occuper du rendu.
  _pId = gl4duCreateProgram("<vs>shaders/gameoflife3D.vs", "<fs>shaders/gameoflife3D.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "view");
  gl4duGenMatrix(GL_FLOAT, "model");

  // Appliquer une matrice de projection, elle sera envoyé lors de l'appel à sendMatrices()
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _dim[1]) / _dim[0];
  GLfloat top    = (1.0f * _dim[1]) / _dim[0];
  gl4duFrustumf(-1.0f, 1.0f, bottom, top, 1.0f, 100.0f);

  colorUniformLocation = glGetUniformLocation(_pId, "color");
  isAliveUniformLocation = glGetUniformLocation(_pId, "isAlive");
}

static double get_dt(void) {
  static double t0 = 0.0f;
  // Récupérer le temps écoulé en ms entre le démarrage du programme et la frame en cours
  double t = gl4dGetElapsedTime();
  // Calculer le temps écoulé depuis la frame précédente en seconde
  double dt = (t - t0) / 100.0f;
  t0 = t;
  return dt;
}

void drawCube(int i, int j, int k) {
  static const float scale = 0.08f;

  // Ramener à l'échelle [-1;1]
  float x  = 2.0f * (i / (NB_CUBES - 1.0f)) - 1.0f;
  float y  = 2.0f * (j / (NB_CUBES - 1.0f)) - 1.0f;
  float z  = 2.0f * (k / (NB_CUBES - 1.0f)) - 1.0f;

  GLboolean isAlive = grid[i][j][k] == GL_TRUE;
  const GLfloat * color = isAlive ? colorAlive : colorDead;
  GLenum mode = isAlive ? GL_FILL : GL_LINE;

  gl4duPushMatrix();
  gl4duTranslatef(x, y, z);
  gl4duScalef(scale, scale, scale);
  gl4duSendMatrices();
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glUniform1i(isAliveUniformLocation, isAlive);
  glUniform4fv(colorUniformLocation, 1, color);
  gl4dgDraw(_cubeId);
  gl4duPopMatrix();
}

void draw(void) {
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  float dt = get_dt();
  static GLfloat rot = 0.0f;
  static float nextGenTimer = 0.0f;

  // Modifier la hue value
  colorAlive[0] += dt * 0.5f;
  if (colorAlive[0] > 1.0f) colorAlive[0] -= 1.0f;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0.0f, 1.5f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  gl4duBindMatrix("model");
  gl4duLoadIdentityf();

  gl4duRotatef(rot, 0.0f, 1.0f, 0.0f);

  for (int i = 0; i < NB_CUBES; ++i)
    for (int j = 0; j < NB_CUBES; ++j)
      for (int k = 0; k < NB_CUBES; ++k)
       drawCube(i, j, k);

  glUseProgram(0);

  nextGenTimer += dt;
  if (nextGenTimer >= 0.05f) {
    updateGrid();
    nextGenTimer = 0.0f;
  }

  rot += 2.0f * M_PI * get_dt();
}

void updateGrid() {
  GLboolean nextGen[NB_CUBES][NB_CUBES][NB_CUBES];

  for (int i = 0; i < NB_CUBES; ++i) {
    for (int j = 0; j < NB_CUBES; ++j) {
      for (int k = 0; k < NB_CUBES; ++k) {
        int n = neighboringStates(i, j, k);

        if (grid[i][j][k] == GL_TRUE) {
          if (n >= SURVIVE_MIN && n <= SURVIVE_MAX) {
            nextGen[i][j][k] = GL_TRUE;
          } else {
            nextGen[i][j][k] = GL_FALSE;
          }
        }
        else {
          if (n >= BIRTH_MIN && n <= BIRTH_MAX) {
            nextGen[i][j][k] = GL_TRUE;
          } else {
            nextGen[i][j][k] = GL_FALSE;
          }
        }
      }
    }
  }
  memcpy(grid, nextGen, sizeof(grid));
}

int neighboringStates(int x, int y, int z) {
  int sum = 0;
  int xIndex = 0, yIndex = 0, zIndex = 0;

  for (int i = -1; i < 2; ++i) {
    for (int j = -1; j < 2; ++j) {
      for (int k = -1; k < 2; ++k) {
        /* Sauter à l'index de départ ou de fin en cas de dépassement */
        /* (-1 + 10) % 10 = 9 */
        xIndex = (x + i + NB_CUBES) % NB_CUBES;
        yIndex = (y + j + NB_CUBES) % NB_CUBES;
        zIndex = (z + k + NB_CUBES) % NB_CUBES;
        sum += grid[xIndex][yIndex][zIndex];
      }
    }
  }

  sum -= grid[x][y][z];
  return sum;
}

void sortie(void) { }

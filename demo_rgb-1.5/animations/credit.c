#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>

#include <SDL_ttf.h>
#include <SDL_image.h>

static void init(void);
static void draw(void);
static void quit(void);

extern GLfloat _dim[];
extern GLfloat _dimBottom;
extern GLfloat _dimTop;

static GLuint _pId = 0;
static GLuint _quad = 0;
static GLuint _texId = 0;

GLint _texDim[2] = { 0 };

void credit(int state)
{
    switch (state)
    {
        case GL4DH_INIT:
            init();
            return;
        case GL4DH_FREE:
        quit();
            return;
            case GL4DH_UPDATE_WITH_AUDIO:
        return;
            default:
            draw();
            return;
    }
}

static void initText(GLuint * texId, const char * text)
{
    static int firstTime = 1;
    SDL_Color c = { 255, 179, 186, 255 };
    SDL_Surface *d, *s;
    TTF_Font * font = NULL;

    if (firstTime) {
        /* initialisation de la bibliothèque SDL2 ttf */
        if (TTF_Init() == -1) {
            fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
            exit(2);
        }
        firstTime = 0;
    }

    if (*texId == 0) {
        glGenTextures(1, texId);
        glBindTexture(GL_TEXTURE_2D, *texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    
    /* chargement de la font */
    if (!(font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf", 72))) {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }
    
    /* création d'une surface SDL avec le texte */
    d = TTF_RenderUTF8_Blended_Wrapped(font, text, c, 2048);
    if (d == NULL) {
        TTF_CloseFont(font);
        fprintf(stderr, "Erreur lors du TTF_RenderText\n");
        return;
    }
    
    /* copie de la surface SDL vers une seconde aux spécifications qui correspondent au format OpenGL */
    s = SDL_CreateRGBSurface(0, d->w, d->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
    assert(s);
    
    SDL_BlitSurface(d, NULL, s, NULL);

    _texDim[0] = s->w;
    _texDim[1] = s->h;

    SDL_FreeSurface(d);
    
    /* transfert vers la texture OpenGL */
    glBindTexture(GL_TEXTURE_2D, *texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
    TTF_CloseFont(font);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static void init(void)
{
    glClearColor(1.0f, 0.7f, 0.7f, 1.0f);
    _pId  = gl4duCreateProgram("<vs>shaders/credits.vs", "<fs>shaders/credits.fs", NULL);
    _quad = gl4dgGenQuadf();
    gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
    gl4duGenMatrix(GL_FLOAT, "projectionMatrix");

    initText(&_texId, 
        "Merci à Farès Belhadj pour son enseignement.\n\n\n\n\n"
        "Merci à Rodolphe Peccatte pour ses idées et son aide.\n\n\n\n\n"
        "Libraries :\n\n"
        "  - GL4Dummies (Farès Belhadj)\n"
        "  - OpenGL\n"
        "  - SDL2\n\n\n"
        "Musique : Psychosi (modarchive.org)\n\n\n"
        "Inspirations :\n\n"
        "  - Patt Vira         (Jeu de la vie 3D)\n"
        "  - Daniel Shiffman   (Fractal Mandelbulb)\n"
        "  - Rodolphe Peccatte (Inversion sphérique en damier)\n\n\n\n\n\n"
        "Démo 64ko par François Godin\n\n\n");
}

static void draw(void)
{
    const GLfloat inclinaison = 0.0;
    static GLfloat t0 = -1;
    GLfloat t, d;

    if (t0 < 0.0f) {
        t0 = SDL_GetTicks();
    }

    t = (SDL_GetTicks() - t0) / 1000.0f;
    d = -1.1f /* du retard pour commencer en bas */ + 0.15f /* vitesse */ * t;

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(_pId);
    
    gl4duBindMatrix("projectionMatrix");
    gl4duLoadIdentityf();
    gl4duOrthof(-1.0f, 1.0f, _dimBottom, _dimTop, 1.0f, -1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texId);
    glUniform1i(glGetUniformLocation(_pId, "inv"), 0);
    glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
    gl4duBindMatrix("modelViewMatrix"); 
    gl4duLoadIdentityf();
    //gl4duScalef(1, 3278/2048, 1);
    gl4duScalef(1, -((float) _texDim[1])/_texDim[0], 1);
    gl4duTranslatef(
        0.0f,
        d * cos(inclinaison * M_PI / 180.0f),
        0.0f
    );
    gl4duRotatef(inclinaison, 1, 0, 0);
    gl4duSendMatrices();
    gl4dgDraw(_quad);
    glUseProgram(0);
}

static void quit(void)
{
    if (_texId) {
      glDeleteTextures(1, &_texId);
      _texId = 0;
    }
}
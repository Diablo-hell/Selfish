#ifndef VARIABLES_H_INCLUDED
#define VARIABLES_H_INCLUDED

#define FPS 60
#define DELAI 10    //en ms, c'est en attendant que les FPS marchent xD

#define NB_ADVERSAIRES_MAX 7

//#define VITESSE_MAX 4
#define VITESSE_PETIT 1
#define VITESSE_MOYEN 2
#define VITESSE_GRAND 3

#define POIDS_GAIN 120
#define TAILLE_INITIALE 10
#define TAUX_GROSSEUR 1.3

#define SON_MUSIQUE 0.0

#define NB_CHOIX 2

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include <SDL/SDL_rotozoom.h>
#include <FMOD/fmod.h>

enum{FENETRE,FULLSCREEN};

int ECRAN_X,ECRAN_Y;
double ZOOM_X,ZOOM_Y;
int etatEcran;
const SDL_VideoInfo* vidinfo;

enum{BOUGEPAS,BOUGE};/*
enum{LG_NOIR,LG_BLEU,LG_VERT,LG_VIOLET,LG_ROUGE,LG_JAUNE,
    HT_BLEU,HT_VERT,HT_ORANGE,HT_VIOLET,HT_ROUGE,HT_JAUNE};//sortes de poisson*/
enum{LG_NOIR=0,HT_BLEU=1};
enum{POISSON_LG,POISSON_HT};
                            /***       LG = LonG _ HT = HauT        ___     12 sortes avec 6 Longs et 6 hauts    ***/
enum{DROITE,GAUCHE,BAS,HAUT,ACCELERE}; // mouvement[5]
/*  AVANCE  = de gauche à droite
    RECULE  = de droite à gauche
    DESCEND = de haut en bas
    MONTE   = de bas en haut    */
enum{PERDU,RIEN,GAGNE};//rien = 1 du coup on utilise ça pour continuer = 1 (la boucle while)

enum{MUSIQUE,CROC,MEURT,SELECT};
enum{JOUER,OPTIONS};//selections
enum{BLANC,ORANGE,BOOST};



typedef struct Point Point;
struct Point
{
  float x,y;
};

typedef struct Vecteur Vecteur;
struct Vecteur
{
  float x,y;
};

typedef struct EqDroite EqDroite;
struct EqDroite
{
  float a,b;
  Point A,B;
  int verticale;
  float x;
};

typedef struct Polygone Polygone;
struct Polygone
{
    Point *sommet;
    EqDroite *droites;
    int nbPoints;
    Point *sommetBase;
    EqDroite *droitesBase;
};


typedef struct Anim Anim;
struct Anim
{
    SDL_Surface **animationGauche;
    SDL_Surface **animationDroite;
    int anims_totales;
};
typedef struct AnimPoissons AnimPoissons;
struct AnimPoissons
{
    Anim animPoissons[2];//BOUGE / BOUGEPAS
};



/*
typedef struct Anim Anim;
struct Anim
{
    SDL_Surface **anim;
    SDL_Surface **animBase;
    //int anims_X;
    //int anims_Y;
    int anims_totales;
    //int taille_X;
    //int taille_Y;
};*/
/*
typedef struct Sprits Sprits;
struct Sprits*/

typedef struct Poissons Poissons;
struct Poissons
{
    Anim animsEntite[2];
    SDL_Surface *rectangle;
    SDL_Rect position;
    SDL_Rect oldPos;
    Polygone polyPoisson[2];//gauche et droite
    double grosseur;
    int poids;
    int etat; // bouge ou pas
    int temps;//temps pour update mouvement
    int tempsActuel;
    int tempsPasse;
    int animActuelle;
    int mouvement[5];
    int lastMouvement[5];
    int vitesse;//on place 6 pour nous et 4 pour adversaire ou 5
    int vivant;
    int vivantAvant;
    int framesEffacement;    //quand le poisson meurt, il s'efface (animation)
    int typePoisson;//utile pour le spawn();
    int orientation;//GAUCHE ou DROITE ( utilisé pour déterminer l'anim à afficher)
};

typedef struct Audio Audio;
struct Audio
{
    FMOD_SYSTEM *system;
    FMOD_CHANNEL *canal;
    FMOD_SOUND *musique;
    FMOD_SOUND *croc;
    FMOD_SOUND *meurt;
    FMOD_SOUND *selection;
};

typedef struct Infos Infos;
struct Infos
{
    Poissons monAnim;
    Poissons *adversaires;
    AnimPoissons mesPoissonsBase[2];
    Polygone polyBase[2][2];
    SDL_Surface *ecran;
    Audio *sons;
    int continuer;
    int pointsBoost,boostTotal,consoBoost,regenBoost,boostActif;

};

#endif // VARIABLES_H_INCLUDED

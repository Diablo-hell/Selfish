#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "lib_local/SDL/SDL_rotozoom.h"
#include "lib_local/FMOD/fmod.h"
#include "lib_local/SDL/SDL_image.h"
#include "lib_local/SDL/SDL_ttf.h"
#include "lib_local/SDL/SDL.h"

#include "variables.h"
#include "graphique.h"
#include "jeu.h"
#include "accueil.h"
#include "collision.h"

int main(int argc, char *argv[])
{

    SDL_Surface *ecran = NULL;
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Audio sons;

    vidinfo = SDL_GetVideoInfo();
//
    etatEcran=FENETRE;

    ECRAN_X = vidinfo->current_w;
    ECRAN_Y = vidinfo->current_h;

    if(etatEcran==FENETRE)//
    {
        ECRAN_X/=2;
        ECRAN_Y/=2;
    }
    ZOOM_X = (double)ECRAN_X/1920;
    ZOOM_Y = (double)ECRAN_Y/1080;

    FMOD_System_Create(&sons.system);
    FMOD_System_Init(sons.system, 8, FMOD_INIT_NORMAL, NULL);

    SDL_putenv ("SDL_VIDEO_CENTERED=center");       //centrer la fenêtre :D
    ecran = SDL_SetVideoMode(ECRAN_X, ECRAN_Y, 32, SDL_HWSURFACE|SDL_DOUBLEBUF/*|SDL_FULLSCREEN*/);
    SDL_WM_SetCaption("Poissons qui se miament entre eux", NULL);
    srand(time(NULL));
    SDL_ShowCursor(SDL_DISABLE);
    menu(ecran,&sons);

    FMOD_System_Close(sons.system);
    FMOD_System_Release(sons.system);
    TTF_Quit();
    SDL_Quit();
    printf("programme terminé !");
    return EXIT_SUCCESS;
}

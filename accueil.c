#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include <SDL/SDL_rotozoom.h>
#include <FMOD/fmod.h>

#include "variables.h"
#include "graphique.h"
#include "jeu.h"
#include "accueil.h"

void menu (SDL_Surface *ecran,Audio *sons)
{
    int continuer = 1,tempsMaintenant=0;
    int selection = JOUER;
    SDL_Surface *fond=NULL,*texte=NULL,*temp=NULL;
    TTF_Font *police = NULL,*policePetite=NULL;
    SDL_Color couleur = {0, 174, 255},couleur_2 = {0,0,0};


    FMOD_System_CreateSound(sons->system, "Sons/accueil.mp3", FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &sons->musique);
    FMOD_System_CreateSound(sons->system, "Sons/select.mp3", FMOD_CREATESAMPLE, 0, &sons->selection);

    FMOD_Sound_SetLoopCount(sons->musique, -1);
    FMOD_System_PlaySound(sons->system, MUSIQUE, sons->musique, 0, NULL);

    FMOD_System_GetChannel(sons->system, MUSIQUE, &sons->canal);
    FMOD_Channel_SetVolume(sons->canal,SON_MUSIQUE);


    fond=IMG_Load("Images/accueil.png");
    temp=SDL_DisplayFormat(fond);
    SDL_FreeSurface(fond);
    fond=temp;
    fond=zoomSurface(fond,ZOOM_X,ZOOM_Y,0);
    SDL_FreeSurface(temp);

    police = TTF_OpenFont("Police/righteous.ttf", 100*ZOOM_X);
    policePetite = TTF_OpenFont("Police/righteous.ttf", 30*ZOOM_X);
    TTF_SetFontStyle(police, TTF_STYLE_BOLD);
    SDL_BlitSurface(fond,NULL,ecran,0);

    affichageMenu(ecran,fond,police,couleur,selection);

//rgb : 0 174 255
    while (continuer == 1)
    {
        tempsMaintenant = SDL_GetTicks();


        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    continuer = 0;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            continuer = 0;
                            break;

                        case SDLK_DOWN:
                            if(selection+1<NB_CHOIX)
                                selection++;
                            //SDL_BlitSurface(fond,NULL,ecran,0);
                            break;

                        case SDLK_UP:
                            if(selection-1>=0)
                                selection--;
                            //SDL_BlitSurface(fond,NULL,ecran,0);
                            break;

                        case SDLK_RETURN:
                            FMOD_System_PlaySound(sons->system, SELECT, sons->selection, 0, NULL);
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                            if(selection==JOUER)
                            {
                                jeu(ecran,sons);
                                continuer=0;
                            }

                            if(selection==OPTIONS)
                            {

                                if(etatEcran==FENETRE)
                                {
                                    etatEcran=FULLSCREEN;
                                    ECRAN_X*=2;
                                    ECRAN_Y*=2;
                                }
                                else if (etatEcran==FULLSCREEN)
                                {
                                    etatEcran=FENETRE;
                                    ECRAN_X/=2;
                                    ECRAN_Y/=2;
                                }

                                ZOOM_X = (double)ECRAN_X/1920;
                                ZOOM_Y = (double)ECRAN_Y/1080;

                                printf("X = %d et Y = %d\n",ECRAN_X,ECRAN_Y);

                                ZOOM_X = (double)ECRAN_X/1920;

                                Uint32 flags = (ecran->flags^SDL_FULLSCREEN);
                                SDL_FreeSurface(ecran);
                                SDL_SetVideoMode(ECRAN_X, ECRAN_Y, 0, flags);
                                SDL_FreeSurface(fond);
                                fond=IMG_Load("Images/accueil.png");
                                temp=SDL_DisplayFormat(fond);
                                SDL_FreeSurface(fond);
                                fond=temp;
                                fond=zoomSurface(fond,ZOOM_X,ZOOM_Y,0);
                                SDL_FreeSurface(temp);


                                TTF_CloseFont(police);
                                TTF_CloseFont(policePetite);
                                police = TTF_OpenFont("Police/righteous.ttf", 100*ZOOM_X);
                                policePetite = TTF_OpenFont("Police/righteous.ttf", 30*ZOOM_X);
                                TTF_SetFontStyle(police, TTF_STYLE_BOLD);
                                SDL_BlitSurface(fond,NULL,ecran,0);
                            }
                            break;

                        default:
                            break;

                    }
                    break;
            }
        }
        if(continuer == 0)
            break;
        SDL_BlitSurface(fond,NULL,ecran,0);
        affichageMenu(ecran,fond,police,couleur,selection);

        afficherFPS(couleur_2,policePetite,ecran);

        SDL_Flip(ecran);
        if(1000/FPS>SDL_GetTicks()-tempsMaintenant)
            SDL_Delay((1000/FPS)-(SDL_GetTicks()-tempsMaintenant)); //33 images par secondes !!!! (1000/30 = 33)          30      1


    }
    TTF_CloseFont(police);

    SDL_FreeSurface(fond);
    SDL_FreeSurface(texte);

}

void affichageMenu (SDL_Surface *ecran,SDL_Surface *fond,TTF_Font *police,SDL_Color couleur,int selection)
{
    SDL_Surface *texte=NULL;
    SDL_Rect position;
    static int distance=0,mvmt = 0,aBouge = 0,oldSelect;
    static int tempsMaintenant=0,tempsPrecedent=0;
    static SDL_Rect positionPrec/*,clipper*/;

    char ecriture[50];

    if(mvmt==0)
    {
        distance = 6*ZOOM_X;
        mvmt=distance;
        oldSelect=selection;
    }


    position.x=0;
    position.y=0;

    //SDL_BlitSurface(fond,NULL,ecran,&positionFond);

    sprintf(ecriture, "- Jouer -");
    texte = TTF_RenderText_Blended(police, ecriture, couleur);

    position.x=ECRAN_X/2-texte->w/2;
    position.y=ECRAN_Y/2-texte->h/2;
    if(positionPrec.x==0||(oldSelect!=JOUER&&selection==JOUER))
    {/*
        SDL_SetClipRect(ecran,&clipper); // Clipper sur zozor

        positionFond.x=0;
        positionFond.y=0;

        SDL_BlitSurface(fond,NULL,ecran,&positionFond);

        SDL_SetClipRect(ecran,NULL); // Enlever le clipper  _   Répeter autant de fois que l'on a de trucs derrière lui

*/
        positionPrec.x=position.x;
        positionPrec.y=position.y;

        oldSelect = selection;
    }

    if(selection==JOUER)
    {

        tempsMaintenant = SDL_GetTicks();

        if(tempsMaintenant - tempsPrecedent > 10)//bouge toute les x sec
        {
            aBouge+=mvmt;
            position.x+=aBouge;
            tempsPrecedent = tempsMaintenant;
        }
        else
        {
            position.x=positionPrec.x;
        }/*

        clipper.x = positionPrec.x;
        clipper.y = positionPrec.y;
        clipper.h = texte->h;
        clipper.w = texte->w;

        SDL_SetClipRect(ecran,&clipper); // Clipper sur zozor

        positionFond.x=0;
        positionFond.y=0;

        SDL_BlitSurface(fond,NULL,ecran,&positionFond);

        SDL_SetClipRect(ecran,NULL); // Enlever le clipper  _   Répeter autant de fois que l'on a de trucs derrière lui
*/
        positionPrec.x = position.x;
        positionPrec.y = position.y;
    }
    SDL_BlitSurface(texte,NULL,ecran,&position);
    SDL_FreeSurface(texte);





    sprintf(ecriture, "- Options -");
    texte = TTF_RenderText_Blended(police, ecriture, couleur);
    position.x=ECRAN_X/2-texte->w/2;
    position.y=ECRAN_Y/2+140*ZOOM_X-texte->h/2;
    if(positionPrec.x==0||(oldSelect!=OPTIONS&&selection==OPTIONS))
    {/*
        SDL_SetClipRect(ecran,&clipper); // Clipper sur zozor

        positionFond.x=0;
        positionFond.y=0;

        SDL_BlitSurface(fond,NULL,ecran,&positionFond);

        SDL_SetClipRect(ecran,NULL); // Enlever le clipper  _   Répeter autant de fois que l'on a de trucs derrière lui
*/
        positionPrec.x=position.x;
        positionPrec.y=position.y;
        oldSelect = selection;
    }

    if(selection==OPTIONS)
    {

        tempsMaintenant = SDL_GetTicks();

        if(tempsMaintenant - tempsPrecedent > 10)//bouge toute les x sec
        {
            aBouge+=mvmt;
            position.x+=aBouge;
            tempsPrecedent = tempsMaintenant;
        }
        else
        {
            position.x=positionPrec.x;
        }
/*
        clipper.x = positionPrec.x;
        clipper.y = positionPrec.y;
        clipper.h = texte->h;
        clipper.w = texte->w;

        SDL_SetClipRect(ecran,&clipper);

        positionFond.x=0;
        positionFond.y=0;

        SDL_BlitSurface(fond,NULL,ecran,&positionFond);

        SDL_SetClipRect(ecran,NULL);
*/
        positionPrec.x = position.x;
        positionPrec.y = position.y;

    }
    SDL_BlitSurface(texte,NULL,ecran,&position);
    SDL_FreeSurface(texte);



    if(mvmt>0 && aBouge>30)
        mvmt=-distance;
    if(mvmt<0 && aBouge<-30)
        mvmt=distance;

}


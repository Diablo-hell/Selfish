#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include <SDL/SDL_rotozoom.h>
#include <FMOD/fmod.h>

#include "variables.h"
#include "jeu.h"
#include "graphique.h"

/*
void grossirPoisson(Poissons *monAnim,AnimPoissons monPoisson[2],int mange);
*/
void grossirPoisson(Poissons *monAnim,AnimPoissons monPoisson[2],int mange)
{
    int j=0,i=0;

    if(mange<=10)
        mange = 1;
    else if(mange<=20)
        mange = 2;
    else if(mange<=30)
        mange = 3;
    else if(mange<=40)
        mange = 4;
    else if(mange<=50)
        mange = 5;
    else if(mange<=75)
        mange = 8;
    else if(mange<=100)
        mange = 10;
    else if(mange>100)
        mange = 15;

    printf("\n\tcalcul du poids ...\t ");
    monAnim->poids += mange;
    monAnim->grosseur = (double)(monAnim->poids)/100;
    printf("grosseur : %f (= %d/%d)",monAnim->grosseur,monAnim->poids,100);
    definirVitesse(monAnim);

    for(j=0;j<2;j++)
    {
        for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
        {
            SDL_FreeSurface(monAnim->animsEntite[j].animation[i]);  //on libère
            monAnim->animsEntite[j].animation[i] = (SDL_Surface*)malloc(sizeof(SDL_Surface*));  //on réalloue de la mémoire

            monAnim->animsEntite[j].animation[i] = zoomSurface(monPoisson[monAnim->typePoisson].animPoissons[j].animation[i],monAnim->grosseur,monAnim->grosseur,0);
        }
    }
}


/*
    Ici, c'est une fonction qui détermine la vitesse de notre poisson en fction de sa taille.
    Plus le pichon est gros, plus il va vite ... logique en soi ; une petite bécane va pas plus vite qu'un bon quatre quatre, bien qu'il soit fat au possible !
*/
void definirVitesse (Poissons *monAnim)
{
    if(monAnim->poids<=30)
        monAnim->vitesse=(int)(VITESSE_PETIT/**ZOOM_X*/);//la vitesse est de 2 et s'adapte en fonction de ZOOM_X donc de la taille de la fenêtre
    else if(monAnim->poids<=75)
        monAnim->vitesse=(int)(VITESSE_MOYEN/**ZOOM_X*/);//la vitesse est de 3 et s'adapte en fonction de ZOOM_X donc de la taille de la fenêtre
    else if(monAnim->poids>=75)
        monAnim->vitesse=(int)(VITESSE_GRAND/**ZOOM_X*/);//la vitesse est de 5 et s'adapte en fonction de ZOOM_X donc de la taille de la fenêtre

}
/*

void grossir(Poissons *monAnim,SDL_Surface *ecran,int mange)
{
    int i=0,j=0;
    SDL_Rect position;
    position.x=0;
    position.y=0;

    for(i=0;i<monAnim->animsEntite[0].anims_totales;i++)
        SDL_FreeSurface(monAnim->animsEntite[0].anim[i]);
    for(i=0;i<monAnim->animsEntite[1].anims_totales;i++)
        SDL_FreeSurface(monAnim->animsEntite[1].anim[i]);
    SDL_FreeSurface(monAnim->rectangle);

    if(mange<=10)
        mange = 1;
    else if(mange<=20)
        mange = 2;
    else if(mange<=30)
        mange = 3;
    else if(mange<=40)
        mange = 4;
    else if(mange<=50)
        mange = 5;
    else if(mange<=75)
        mange = 8;
    else if(mange<=100)
        mange = 10;
    else if(mange>100)
        mange = 15;

    printf("\n\tcalcul du poids ...\t ");
    monAnim->poids += mange;
    monAnim->grosseur = (double)(monAnim->poids)/100;
    printf("grosseur : %f (= %d/%d)",monAnim->grosseur,monAnim->poids,100);


    //printf("libéré !");

    for(j=0;j<2;j++)
    {
        for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
        {
            monAnim->animsEntite[j].anim[i] = zoomSurface(monAnim->animsEntite[j].animBase[i],monAnim->grosseur,monAnim->grosseur,0);

            //monAnim->animsEntite[j].anim[i] = SDL_DisplayFormatAlpha(monAnim->animsEntite[j].anim[i]);
            if(i==0 && j==0)
            {
                monAnim->animsEntite[BOUGEPAS].taille_X = monAnim->animsEntite[BOUGEPAS].anim[0]->w;
                monAnim->animsEntite[BOUGEPAS].taille_Y = monAnim->animsEntite[BOUGEPAS].anim[0]->h;
            }
            if(i==0 && j==1)
            {
                monAnim->animsEntite[BOUGE].taille_X = monAnim->animsEntite[BOUGE].anim[0]->w;
                monAnim->animsEntite[BOUGE].taille_Y = monAnim->animsEntite[BOUGE].anim[0]->h;
            }
            //printf("\n\n\tanim %d : [%d;%d]\n\n\n",i,monAnim->animsEntite[j].anim[i]->w,monAnim->animsEntite[j].anim[i]->h);
        }
    }
//SDL_Delay(1000);

    monAnim->rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, monAnim->animsEntite[0].taille_X, monAnim->animsEntite[0].taille_Y, 32, 0, 0, 0, 0);
    SDL_FillRect(monAnim->rectangle, NULL, SDL_MapRGB(ecran->format, 255, 145, 7));
}*/

void afficherFPS (SDL_Color couleur,TTF_Font *policePetite,SDL_Surface *ecran)
{
    static int frame=0,fpsActuels=0,lasttime=0,newtime=0;
    SDL_Rect position;
    SDL_Surface *texte=NULL;
    char chaine[50];


    frame++;
    sprintf(chaine, "FPS : %d",fpsActuels);
    texte = TTF_RenderText_Blended(policePetite, chaine, couleur);

    position.x=0;
    position.y=ECRAN_Y-texte->h;

    SDL_BlitSurface(texte, NULL, ecran, &position);

    newtime = SDL_GetTicks();
    if(newtime - lasttime > 1000)  //mouvement
    {
        printf("\n\tFPS : %d",fpsActuels);

        fpsActuels=frame;
        frame = 0;
        lasttime = newtime;
    }

    SDL_FreeSurface(texte);
}

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include <SDL/SDL_rotozoom.h>
#include <FMOD/fmod.h>
#include <pthread.h>

#include "variables.h"
#include "jeu.h"
#include "graphique.h"
#include "collision.h"

void jeu(SDL_Surface *ecran,Audio *sons)
{
    /**VARIABLES**/
    int continuer = 1,alpha=0;
    int tempsDepart = 0,tempsActuel=0; //pour les fps
    int i=0,j=0,k=0;
    int descente = 0;
    int boostTotal = 300,pointsBoost = boostTotal,consoBoost = 3,regenBoost = 2,boostActif = 0;

    char score[100],message[100];


    SDL_Surface *fond=NULL,*texte[4]={NULL},*temp=NULL,*imgTemp = NULL,*bande=NULL;//
    SDL_Surface *fondBarre=NULL, *barre = NULL;
    SDL_Color couleur[3];
    SDL_Rect position,positionFond/*,clipper*/;
    SDL_Rect partieBarre;

    Polygone polyBase[2];

    /*clipper.x=0;
    clipper.y=0;
    clipper.w=0;
    clipper.h=0;*/

    TTF_Font *police = NULL,*policePetite = NULL,*policeMessageFin[2] = {NULL},*policeBoost=NULL;

    policeBoost = TTF_OpenFont("Police/acme.ttf", 20*ZOOM_X);
/***/
    AnimPoissons mesPoissonsBase[2];
/***/
    Poissons monAnim;
    Poissons *adversaires;
    adversaires = malloc(NB_ADVERSAIRES_MAX *sizeof(Poissons));//crée un tableau dynamique de la taille du nombre d'adversaires

    /*monAnim.charge=0;
    for(i=0;i<NB_ADVERSAIRES_MAX;i++)
        adversaires[i].charge=0;*/

    FMOD_System_CreateSound(sons->system, "Sons/mange.mp3", FMOD_CREATESAMPLE, 0, &sons->croc);
    FMOD_System_CreateSound(sons->system, "Sons/scream.mp3", FMOD_CREATESAMPLE, 0, &sons->meurt);
    FMOD_System_CreateSound(sons->system, "Sons/jeu.mp3", FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &sons->musique);


    /**INITIALISATION**/

    fond=IMG_Load("Images/fond.jpg");
    temp=SDL_DisplayFormat(fond);
    SDL_FreeSurface(fond);
    fond=temp;
    fond=zoomSurface(fond,ZOOM_X,ZOOM_Y,0);
    SDL_FreeSurface(temp);

    bande = SDL_CreateRGBSurface(SDL_HWSURFACE, 1920*ZOOM_X, 300*ZOOM_Y, 32, 0, 0, 0, 0);
    SDL_FillRect(bande, NULL, SDL_MapRGB(ecran->format, 174, 174, 174));
    SDL_SetAlpha(bande,SDL_SRCALPHA,(int)(0*255/100));//30% de transparence

    initPoly(polyBase);

    attribuerPoly(polyBase,&monAnim.polyPoisson,POISSON_LG);
    for(i=0;i<NB_ADVERSAIRES_MAX;i++)
        attribuerPoly(polyBase,&adversaires[i].polyPoisson,POISSON_HT);
/*
    printf("\n\n\n\tGEOMETRIE DE MON PICHON\n");
    afficherPts(monAnim.polyPoisson);*/



    fondBarre = IMG_Load("Images/barreFond.png");
    barre = IMG_Load("Images/boostBarre.png");

    temp=SDL_DisplayFormatAlpha(fondBarre);
    SDL_FreeSurface(fondBarre);
    fondBarre=temp;
    fondBarre=zoomSurface(fondBarre,ZOOM_X,ZOOM_Y,0);
    SDL_FreeSurface(temp);

    temp=SDL_DisplayFormatAlpha(barre);
    SDL_FreeSurface(barre);
    barre=temp;
    barre=zoomSurface(barre,ZOOM_X,ZOOM_Y,0);
    SDL_FreeSurface(temp);

                        /***METTRE -1 A LA FIN***/


    /**JEU**/

    FMOD_Sound_SetLoopCount(sons->musique, -1);
    FMOD_System_PlaySound(sons->system, MUSIQUE, sons->musique, 0, NULL);

    FMOD_System_GetChannel(sons->system, MUSIQUE, &sons->canal);
    FMOD_Channel_SetVolume(sons->canal,SON_MUSIQUE);

    /***/
    initAnim(mesPoissonsBase,ecran);
    /***/

    Infos mesInfos;



    pthread_t thread_boucle;// On crée un thread

    couleur[BLANC].r = 255;
    couleur[BLANC].b = 255;
    couleur[BLANC].g = 255;

    couleur[ORANGE].r = 250;
    couleur[ORANGE].g = 164;
    couleur[ORANGE].b = 1;

    couleur[BOOST].r = 180;
    couleur[BOOST].g = 0;
    couleur[BOOST].b = 204;

do
{
//                    poly->morceauPoly = realloc(poly->morceauPoly,poly->nbTriangles * sizeof(Triangle));
    alpha = 0;
    descente = 5*ECRAN_Y/16;
    continuer = 1;
    positionFond.x = 0;
    positionFond.y = 0;
    boostTotal = 300;
    pointsBoost=boostTotal;
    boostActif=0;

    police = TTF_OpenFont("Police/acme.ttf", 45*ZOOM_X);
    policePetite = TTF_OpenFont("Police/acme.ttf", 20*ZOOM_X);
    policeMessageFin[0] = TTF_OpenFont("Police/righteous.ttf", 100*ZOOM_X);
    policeMessageFin[1] = TTF_OpenFont("Police/righteous.ttf", 50*ZOOM_X);
    policeBoost = TTF_OpenFont("Police/acme.ttf", 20*ZOOM_X);

    reinitAnimPoisson(&monAnim,mesPoissonsBase,ecran,0);
    for(i=0;i<NB_ADVERSAIRES_MAX;i++)
        reinitAnimPoisson(&adversaires[i],mesPoissonsBase,ecran,1);


    spawnPoissons(&monAnim,mesPoissonsBase,ecran,monAnim.poids);


    adaptPoly(polyBase,&monAnim.polyPoisson,POISSON_LG,monAnim.grosseur);
    avance(&monAnim);
    printf("\n\n\n\tGEOMETRIE DE MON PICHON\n");
    afficherPts(monAnim.polyPoisson);
    //printf("Droite(,monAnim.polyPoisson.droites[i])
    mesInfos.adversaires = adversaires;
    mesInfos.continuer = continuer;
    mesInfos.ecran = ecran;
    mesInfos.mesPoissonsBase[0] = mesPoissonsBase[0];
    mesInfos.mesPoissonsBase[1] = mesPoissonsBase[1];
    mesInfos.monAnim = monAnim;
    mesInfos.polyBase[0] = polyBase[0];
    mesInfos.polyBase[1] = polyBase[1];
    mesInfos.sons = sons;
    mesInfos.pointsBoost = pointsBoost;
    mesInfos.boostTotal = boostTotal;
    mesInfos.consoBoost = consoBoost;
    mesInfos.regenBoost = regenBoost;
    mesInfos.boostActif = boostActif;
    printf("\npassoire");

    pthread_create(&thread_boucle, NULL, boucle, (void*)&mesInfos);// Permet d'exécuter le fonction maFonction en parallèle


//SDL_Delay(1000);
    //initAnim(&monAnim,ecran,LG_NOIR,0);
//SDL_Delay(1000);

    sprintf(score, "%d",monAnim.poids);
    texte[0] = TTF_RenderText_Blended(police, score, couleur[BLANC]);
    SDL_BlitSurface(fond,NULL,ecran,0);

    //SDL_Flip(ecran);

    printf("tout chargé !! passoire");
//    /****/
//    continuer = 1;
//    while (continuer == 1)
//    {
//        SDL_Event event;
//        while(SDL_PollEvent(&event))
//        {
//            switch(event.type)
//            {
//                case SDL_QUIT:
//                    continuer = -1;
//                    break;
//
//                case SDL_KEYDOWN:
//                    switch (event.key.keysym.sym)
//                    {
//                        case SDLK_ESCAPE:
//                            continuer = -1;
//                            break;
//
//                        default:
//                            break;
//                    }
//                    break;
//
//                default:
//
//                    break;
//            }
//        }
//
//        SDL_Delay(10);
//    }
//    /****/


    while (continuer == 1)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    continuer = -1;
                    mesInfos.continuer = continuer;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            continuer = -1;
                            mesInfos.continuer = continuer;
                            break;

                        case SDLK_LEFT:
                            monAnim.mouvement[GAUCHE] = 1;
                            if(monAnim.mouvement[DROITE]==1)
                            {
                                monAnim.lastMouvement[DROITE]=1;
                                monAnim.mouvement[DROITE]=0;
                            }
                            break;

                        case SDLK_RIGHT:
                            monAnim.mouvement[DROITE] = 1;
                            if(monAnim.mouvement[GAUCHE]==1)
                            {
                                monAnim.lastMouvement[GAUCHE]=1;
                                monAnim.mouvement[GAUCHE]=0;
                            }
                            break;

                        case SDLK_UP:
                            monAnim.mouvement[HAUT] = 1;
                            if(monAnim.mouvement[BAS]==1)
                            {
                                monAnim.lastMouvement[BAS]=1;
                                monAnim.mouvement[BAS]=0;
                            }
                            break;

                        case SDLK_DOWN:
                            monAnim.mouvement[BAS] = 1;
                            if(monAnim.mouvement[HAUT]==1)
                            {
                                monAnim.lastMouvement[HAUT]=1;
                                monAnim.mouvement[HAUT]=0;
                            }
                            break;

                        case SDLK_SPACE:
                            boostActif = 1;
                            //monAnim.mouvement[ACCELERE] = 5*ZOOM_X;
                            break;

                        case SDLK_RETURN:
                            /*
                            if(appuie == 0)
                            {
                                appuie = 1;
                                grossir(&monAnim,ecran,mange);
                            }*/
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_LEFT:
                            monAnim.mouvement[GAUCHE]=0;
                            monAnim.lastMouvement[GAUCHE]=0;
                            if(monAnim.lastMouvement[DROITE]==1)
                            {
                                monAnim.mouvement[DROITE]=1;
                                monAnim.lastMouvement[DROITE]=0;
                            }
                            break;

                        case SDLK_RIGHT:
                            monAnim.mouvement[DROITE]=0;
                            monAnim.lastMouvement[DROITE]=0;
                            if(monAnim.lastMouvement[GAUCHE]==1)
                            {
                                monAnim.mouvement[GAUCHE]=1;
                                monAnim.lastMouvement[GAUCHE]=0;
                            }
                            break;

                        case SDLK_UP:
                            monAnim.mouvement[HAUT]=0;
                            monAnim.lastMouvement[HAUT]=0;
                            if(monAnim.lastMouvement[BAS]==1)
                            {
                                monAnim.mouvement[BAS]=1;
                                monAnim.lastMouvement[BAS]=0;
                            }
                            break;

                        case SDLK_DOWN:
                            monAnim.mouvement[BAS]=0;
                            monAnim.lastMouvement[BAS]=0;
                            if(monAnim.lastMouvement[HAUT]==1)
                            {
                                monAnim.mouvement[HAUT]=1;
                                monAnim.lastMouvement[HAUT]=0;
                            }
                            break;

                        case SDLK_SPACE:
                            boostActif = 0;
                            break;

                        case SDLK_RETURN:
                            //appuie = 0;
                            break;

                        default:
                            break;

                    }
                    break;

                default:
                    break;
            }
        }
        for(i=0;i<5;i++)
        {
            mesInfos.monAnim.mouvement[i] = monAnim.mouvement[i];
            mesInfos.monAnim.lastMouvement[i] = monAnim.lastMouvement[i];
        }

            adversaires = mesInfos.adversaires;
            monAnim = mesInfos.monAnim;
            continuer = mesInfos.continuer;
            ecran = mesInfos.ecran;
            pointsBoost = mesInfos.pointsBoost;
            mesInfos.boostActif = boostActif;

            if(pointsBoost>0 && boostActif==1)
                monAnim.mouvement[ACCELERE] = monAnim.vitesse*1.6;//ici, 1.6 est le boost :p
            else
                monAnim.mouvement[ACCELERE] = 0;

            sprintf(score, "Fattitude : %d / %d",monAnim.poids,POIDS_GAIN);
            texte[0] = TTF_RenderText_Blended(police, score, couleur[BLANC]);

            positionFond.x = 0;
            positionFond.y = 0;

            SDL_BlitSurface(fond, NULL, ecran, &positionFond);
            if(monAnim.orientation == GAUCHE)
                SDL_BlitSurface(monAnim.animsEntite[monAnim.etat].animationGauche[monAnim.animActuelle],NULL,ecran,&monAnim.position);
            else if(monAnim.orientation == DROITE)
                SDL_BlitSurface(monAnim.animsEntite[monAnim.etat].animationDroite[monAnim.animActuelle],NULL,ecran,&monAnim.position);

            for(i=0;i<NB_ADVERSAIRES_MAX;i++)
            {
                if(adversaires[i].vivant==1)
                {
                    if(adversaires[i].orientation == GAUCHE)
                        SDL_BlitSurface(adversaires[i].animsEntite[adversaires[i].etat].animationGauche[adversaires[i].animActuelle],NULL,ecran,&adversaires[i].position);
                    else if(adversaires[i].orientation == DROITE)
                        SDL_BlitSurface(adversaires[i].animsEntite[adversaires[i].etat].animationDroite[adversaires[i].animActuelle],NULL,ecran,&adversaires[i].position);

                }
                else if (adversaires[i].vivantAvant==1 && adversaires[i].framesEffacement-10 > 0)
                {
                    adversaires[i].framesEffacement-=10;//on commence à 255, donc ça passe puisque ce sera = à 0 à un moment car 8 est diviseur commun de 128

                    imgTemp=(SDL_Surface*)malloc(sizeof(SDL_Surface*));
                    if(adversaires[i].orientation == GAUCHE)
                        imgTemp = SDL_ConvertSurface(adversaires[i].animsEntite[adversaires[i].etat].animationGauche[adversaires[i].animActuelle],adversaires[i].animsEntite[adversaires[i].etat].animationGauche[adversaires[i].animActuelle]->format,adversaires[i].animsEntite[adversaires[i].etat].animationGauche[adversaires[i].animActuelle]->flags);
                    else if(adversaires[i].orientation == DROITE)
                        imgTemp = SDL_ConvertSurface(adversaires[i].animsEntite[adversaires[i].etat].animationDroite[adversaires[i].animActuelle],adversaires[i].animsEntite[adversaires[i].etat].animationDroite[adversaires[i].animActuelle]->format,adversaires[i].animsEntite[adversaires[i].etat].animationDroite[adversaires[i].animActuelle]->flags);
                    My_SetAlpha32(imgTemp,adversaires[i].framesEffacement);
                    SDL_BlitSurface(imgTemp,NULL,ecran,&adversaires[i].position);
                    SDL_FreeSurface(imgTemp);
                }
                if(adversaires[i].vivantAvant==1 && adversaires[i].framesEffacement-10 <= 0)
                {
                    printf("\n\t\t\tMOOOOOOOOOOOOOOOORT\n");
                    adversaires[i].vivantAvant=0;
                    mesInfos.adversaires[i].vivantAvant = 0;
                }

            }

            partieBarre.x = 0;
            partieBarre.y = 0;
            partieBarre.w = pointsBoost*barre->w/boostTotal;
            partieBarre.h = barre->h;

            position.x = ECRAN_X/2-barre->w/2;
            position.y = ECRAN_Y-3*barre->h/2;
            SDL_BlitSurface(fondBarre,NULL,ecran,&position);
            SDL_BlitSurface(barre,&partieBarre,ecran,&position);


            sprintf(message,"BOOST : ");
            texte[3] = TTF_RenderText_Blended(policeBoost,message, couleur[BOOST]);
            position.x = ECRAN_X/2-texte[3]->w/2-texte[3]->w/8*3;
            position.y += barre->h/2-texte[3]->h/2;
            SDL_BlitSurface(texte[3],NULL,ecran,&position);
            position.x += texte[3]->w;
            SDL_FreeSurface(texte[3]);


            sprintf(message,"%d",pointsBoost);
            texte[3] = TTF_RenderText_Blended(policeBoost,message, couleur[BOOST]);
            SDL_BlitSurface(texte[3],NULL,ecran,&position);

            SDL_BlitSurface(texte[0], NULL, ecran, 0);

            /***AFFICHER FPS***/
            afficherFPS(couleur[BLANC],policePetite,ecran);

            SDL_Flip(ecran);

            SDL_FreeSurface(texte[3]);
            SDL_FreeSurface(texte[0]);


        do{
        SDL_Delay(1);
        tempsActuel = SDL_GetTicks();
        }while(tempsActuel-tempsDepart<1000/FPS);
        //printf("\tFPS : %d<%d\n",(int)(tempsActuel-tempsDepart),(1000/(FPS)));
        tempsDepart = tempsActuel;
        //continuer=PERDU;
/**A METTRE QUAND Y'AURA THREADS POUR LE RESTE**/

        //printf("\n\t\t\t\ttps = %d,(%d/%d) - (%d-%d)",(1000/FPS)-(SDL_GetTicks()-tempsDepart),1000,FPS,SDL_GetTicks(),tempsDepart);


/**FAIT DES FREEZZZZZS**/
/*
        if((int)(1000/FPS)>(int)(SDL_GetTicks()-tempsDepart))
        {
            printf("sleep de %d\n",(int)(1000/FPS)-(int)(SDL_GetTicks()-tempsDepart));
            SDL_Delay((int)(1000/FPS)-(int)(SDL_GetTicks()-tempsDepart)); //33 images par secondes !!!! (1000/30 = 33)          30      1
        }*/

        //SDL_Delay(DELAI);
/***
unsigned int checkTime = SDL_GetTicks();
const unsigned int fps = 20;

if(SDL_GetTicks() > (checkTime + 1000 / fps) )
{
    //....

    checkTime = SDL_GetTicks();
}

***/
    }

    pthread_cancel(thread_boucle);
    pthread_join(thread_boucle,NULL);

    /*
    detecte
    met jour
    SLeep ()

    */


    if(continuer==GAGNE || continuer==PERDU)
    {
        sprintf(score, "Fattitude : %d / %d",monAnim.poids,POIDS_GAIN);
        texte[0] = TTF_RenderText_Blended(police, score, couleur[BLANC]);


        if(continuer==GAGNE)
            sprintf(message, "~ Gagné ~");
        else
            sprintf(message, "~ Perdu ~");

        texte[1] = TTF_RenderText_Blended(policeMessageFin[0], message, couleur[ORANGE]);


        if(continuer==GAGNE)
            sprintf(message, "t'as grillé tous les sushis !!");
        else
            sprintf(message, "Ils t'ont tous noyé comme un poisson !");

        texte[2] = TTF_RenderText_Blended(policeMessageFin[1], message, couleur[ORANGE]);







        SDL_Flip(ecran);
        continuer = 1;

        alpha=0;
        while (continuer == 1)
        {
            SDL_Event event;
            while(SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_QUIT:
                        continuer = -1;
                        break;

                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym)
                        {
                            case SDLK_ESCAPE:
                                continuer = -1;
                                break;
                            case SDLK_RETURN:
                                continuer=2;
                                break;

                            default:
                                break;
                        }
                        break;

                    default:

                        break;
                }
            }

            monAnim.tempsActuel = SDL_GetTicks();

            if(monAnim.tempsActuel - monAnim.tempsPasse > monAnim.temps)  //changement d'anim
            {
                if(monAnim.animActuelle+1<monAnim.animsEntite[monAnim.etat].anims_totales)
                    {monAnim.animActuelle++;}
                else
                    {monAnim.animActuelle=0;}
                monAnim.tempsPasse = monAnim.tempsActuel;
            }


            for(i=0;i<NB_ADVERSAIRES_MAX;i++)
            {
                if(adversaires[i].vivant==1)
                {
                    adversaires[i].tempsActuel = SDL_GetTicks();

                    if(adversaires[i].tempsActuel - adversaires[i].tempsPasse > adversaires[i].temps)  //changement d'anim
                    {
                        if(adversaires[i].animActuelle+1<adversaires[i].animsEntite[adversaires[i].etat].anims_totales)
                            {adversaires[i].animActuelle++;}
                        else
                            {adversaires[i].animActuelle=0;}
                        adversaires[i].tempsPasse = adversaires[i].tempsActuel;
                    }
                }
            }
            positionFond.x = 0;
            positionFond.y = 0;

            SDL_BlitSurface(fond, NULL, ecran, &positionFond); // Effacer zozor
            //monAnim.position.x++;
            for(i=0;i<NB_ADVERSAIRES_MAX;i++)
            {
                if(adversaires[i].vivant==1)
                {
                    if(adversaires[i].orientation == GAUCHE)
                        SDL_BlitSurface(adversaires[i].animsEntite[adversaires[i].etat].animationGauche[adversaires[i].animActuelle],NULL,ecran,&adversaires[i].position);
                    else if(adversaires[i].orientation == DROITE)
                        SDL_BlitSurface(adversaires[i].animsEntite[adversaires[i].etat].animationDroite[adversaires[i].animActuelle],NULL,ecran,&adversaires[i].position);

                }
                else if (adversaires[i].vivantAvant==1 && adversaires[i].framesEffacement-10 > 0)
                {
                    adversaires[i].framesEffacement-=10;//on commence à 255, donc ça passe puisque ce sera = à 0 à un moment car 8 est diviseur commun de 128

                    imgTemp=(SDL_Surface*)malloc(sizeof(SDL_Surface*));
                    if(adversaires[i].orientation == GAUCHE)
                        imgTemp = SDL_ConvertSurface(adversaires[i].animsEntite[adversaires[i].etat].animationGauche[adversaires[i].animActuelle],adversaires[i].animsEntite[adversaires[i].etat].animationGauche[adversaires[i].animActuelle]->format,adversaires[i].animsEntite[adversaires[i].etat].animationGauche[adversaires[i].animActuelle]->flags);
                    else if(adversaires[i].orientation == DROITE)
                        imgTemp = SDL_ConvertSurface(adversaires[i].animsEntite[adversaires[i].etat].animationDroite[adversaires[i].animActuelle],adversaires[i].animsEntite[adversaires[i].etat].animationDroite[adversaires[i].animActuelle]->format,adversaires[i].animsEntite[adversaires[i].etat].animationDroite[adversaires[i].animActuelle]->flags);
                    My_SetAlpha32(imgTemp,adversaires[i].framesEffacement);
                    SDL_BlitSurface(imgTemp,NULL,ecran,&adversaires[i].position);
                    SDL_FreeSurface(imgTemp);
                }
                if(adversaires[i].vivantAvant==1 && adversaires[i].framesEffacement-10 <= 0)
                {
                    printf("\n\t\t\tMOOOOOOOOOOOOOOOORT\n");
                    adversaires[i].vivantAvant=0;
                    mesInfos.adversaires[i].vivantAvant = 0;
                }
            }

            if(monAnim.orientation == GAUCHE)
                SDL_BlitSurface(monAnim.animsEntite[monAnim.etat].animationGauche[monAnim.animActuelle],NULL,ecran,&monAnim.position);
            else if(monAnim.orientation == DROITE)
                SDL_BlitSurface(monAnim.animsEntite[monAnim.etat].animationDroite[monAnim.animActuelle],NULL,ecran,&monAnim.position);


            partieBarre.x = 0;
            partieBarre.y = 0;
            partieBarre.w = pointsBoost*barre->w/boostTotal;
            partieBarre.h = barre->h;

            position.x = ECRAN_X/2-barre->w/2;
            position.y = ECRAN_Y-3*barre->h/2;
            SDL_BlitSurface(fondBarre,NULL,ecran,&position);
            SDL_BlitSurface(barre,&partieBarre,ecran,&position);

            sprintf(message,"BOOST : ");
            texte[3] = TTF_RenderText_Blended(policeBoost,message, couleur[BOOST]);
            position.x = ECRAN_X/2-texte[3]->w/2-texte[3]->w/8*3;
            position.y += barre->h/2-texte[3]->h/2;
            SDL_BlitSurface(texte[3],NULL,ecran,&position);
            position.x += texte[3]->w;
            SDL_FreeSurface(texte[3]);
            sprintf(message,"%d",pointsBoost);
            texte[3] = TTF_RenderText_Blended(policeBoost,message, couleur[BOOST]);
            SDL_BlitSurface(texte[3],NULL,ecran,&position);

            if(alpha<100)
                alpha+=2;
            if(descente < ECRAN_Y/2-bande->h/2)
            {
                descente+=3*ZOOM_Y;/*
                if(descente>ECRAN_Y/8)
                    descente-=5*ZOOM_Y;
                if(descente>2*ECRAN_Y/8)
                    descente-=5*ZOOM_Y;
                if(descente>3*ECRAN_Y/8)
                    descente-=5*ZOOM_Y;*/

            }

            if(alpha<40)
            {
                SDL_SetAlpha(bande,SDL_SRCALPHA,(int)(alpha*255/100));//30% de transparence
            }
            position.x = 0;
            position.y = descente;
            SDL_BlitSurface(bande,NULL,ecran,&position);

            SDL_BlitSurface(texte[0], NULL, ecran,&positionFond);
/*
            position.y = ECRAN_Y/2 - bande->h/8 - texte[1]->h/2;
            position.y = ECRAN_Y/2 + bande->h/8 - texte[2]->h/2;
*/
            position.x = ECRAN_X/2 - texte[1]->w/2;
            position.y = descente + 3*bande->h/8 - texte[1]->h/2;
            if(alpha<100)
            {
                imgTemp=(SDL_Surface*)malloc(sizeof(SDL_Surface*));
                imgTemp = SDL_ConvertSurface(texte[1],texte[1]->format,texte[1]->flags);
                My_SetAlpha32(imgTemp,(int)(alpha*255/100));
                SDL_BlitSurface(imgTemp,NULL,ecran,&position);
                SDL_FreeSurface(imgTemp);
            }
            else
            SDL_BlitSurface(texte[1], NULL, ecran,&position);

            position.x = ECRAN_X/2 - texte[2]->w/2;
            position.y = descente + 5*bande->h/8 - texte[2]->h/2;
            if(alpha<100)
            {
                imgTemp=(SDL_Surface*)malloc(sizeof(SDL_Surface*));
                imgTemp = SDL_ConvertSurface(texte[2],texte[2]->format,texte[1]->flags);
                My_SetAlpha32(imgTemp,(int)(alpha*255/100));
                SDL_BlitSurface(imgTemp,NULL,ecran,&position);
                SDL_FreeSurface(imgTemp);
            }
            else
            SDL_BlitSurface(texte[2],NULL,ecran,&position);

            /***AFFICHER FPS***/
            afficherFPS(couleur[BLANC],policePetite,ecran);

            SDL_Flip(ecran);


        do{
            SDL_Delay(1);
            tempsActuel = SDL_GetTicks();
        }while(tempsActuel-tempsDepart<1000/FPS);

        tempsDepart = tempsActuel;

        }
    }
    if(continuer == 2)
    {
        printf("\n\n\n\n");
        printf("\t\t***************************\n");
        printf("\t\t******NOUVELLE PARTIE******\n");
        printf("\t\t***************************\n");

        FreeAnim(&monAnim);
        printf("je suis là\n");/*
        FreeAnim(&adversaires[0]);
        FreeAnim(&adversaires[1]);
        FreeAnim(&adversaires[2]);*/

        for(i=0;i<NB_ADVERSAIRES_MAX;i++)
        {
            printf("\n\tfesse");
            if(adversaires[i].rectangle==NULL)
                FreeAnim(&adversaires[i]);
        }

        printf("\nje suis là");

        //free((void*)mesPoissonsBase);

        TTF_CloseFont(police);
        TTF_CloseFont(policePetite);
        TTF_CloseFont(policeBoost);
        for(i=0;i<2;i++)
            TTF_CloseFont(policeMessageFin[i]);

        printf("\nliberation textes ...");
        for(i=0;i<4;i++)
            SDL_FreeSurface(texte[i]);
        printf("libéré !");


        printf("taille : %d",sizeof monAnim.animsEntite[0].animationGauche);

    }
    else
        for(i=0;i<4;i++)
            SDL_FreeSurface(texte[i]);

}while(continuer==2);


    printf("je suis làaaaaaaaa\n");

    FreeAnim(&monAnim);
    printf("je suis là\n");
    FreeAnim(&adversaires[0]);
    FreeAnim(&adversaires[1]);
    FreeAnim(&adversaires[2]);
/*
    for(i=0;i<NB_ADVERSAIRES_MAX;i++)
    {
        printf("\n\tfesse");
        FreeAnim(&adversaires[i]);
    }*/

    printf("je suis là\n");

    //free((void*)mesPoissonsBase);

    TTF_CloseFont(police);
    TTF_CloseFont(policePetite);
    TTF_CloseFont(policeBoost);
    for(i=0;i<2;i++)
        TTF_CloseFont(policeMessageFin[i]);

/*
    for(i=0;i<4;i++)
    {
        if(texte[i]->w>0)
        {
            printf("\nsurface %d pas vide ! liberation ...",i);
            SDL_FreeSurface(texte[i]);
        }
    }*/
    printf("libéré !");

    FMOD_Sound_Release(sons->musique);
    FMOD_Sound_Release(sons->croc);
    FMOD_Sound_Release(sons->meurt);
    FMOD_Sound_Release(sons->selection);
    printf("libéré !");
    freePoly(polyBase);
    printf("1");
    freePoly(&monAnim.polyPoisson);
    printf("libéré !");
    for(i=0;i<NB_ADVERSAIRES_MAX;i++)
        freePoly(&adversaires[i].polyPoisson);

    printf("libéré !");

    free((void*)adversaires);
    for(k=0;k<2;k++)
    {
        for(j=0;j<2;j++)
        {
            for(i=0;i<mesPoissonsBase[k].animPoissons[j].anims_totales;i++)
            {
                SDL_FreeSurface(mesPoissonsBase[k].animPoissons[j].animationGauche[i]);  //on libère
            }
        }
    }
    printf("libéré !");
    SDL_FreeSurface(fondBarre);
    SDL_FreeSurface(barre);
    SDL_FreeSurface(ecran);
    SDL_FreeSurface(fond);

    printf("\nliberation terminee ! Fin du programme...");
}


void* boucle(void *infos)
{
    Infos *mesInfos = infos;
    int i=0,newtime[4]={0},lasttime[4]={0},ajoutAdversaire = 1;

    printf("poids : %d\n",mesInfos->monAnim.poids);
    while(mesInfos->continuer == 1)
    {

        /***ANIM***/

        newtime[0] = SDL_GetTicks();

        if(newtime[0] - lasttime[0] > 200)  //changement de direction probable
        {
            for(i=0;i<NB_ADVERSAIRES_MAX;i++)
                if(mesInfos->adversaires[i].vivant==1)
                    changeDirection(&mesInfos->adversaires[i]);
            lasttime[0] = newtime[0];
        }


        newtime[1] = SDL_GetTicks();

        if(newtime[1] - lasttime[1] > 1200)  //spawn probable, réglé à 800
        {
            for(i=0;i<NB_ADVERSAIRES_MAX;i++)
            {
                if(mesInfos->adversaires[i].vivant==0 && mesInfos->adversaires[i].vivantAvant==0)   //s'il est bien mort ...
                {
                    nouveau(&ajoutAdversaire);
                    if(ajoutAdversaire==1)
                    {
                        printf("\t\tAJOUT de l'adversaire %d !\n",i);
                        spawnPoissons(&mesInfos->adversaires[i],mesInfos->mesPoissonsBase,mesInfos->ecran,mesInfos->monAnim.poids);
                        adaptPoly(mesInfos->polyBase,&mesInfos->adversaires[i].polyPoisson,POISSON_HT,mesInfos->adversaires[i].grosseur);
                        ajoutAdversaire = 0;
                        break;
                    }

                }
            }
            lasttime[1] = newtime[1];
        }


        newtime[2] = SDL_GetTicks();                //5*1/zoom_X comme ça on aura 5 pour une taille normale (1920/1080) et par exemple pour une fenetre deux fois plus petite,
                                                    //                  on aura 10 ; c'est parce que je ne peux pas ralentir la vitesse qui est à 1 px ttes les 5ms dejà
        if(newtime[2] - lasttime[2] > 5*1/ZOOM_X)   //mouvement  _   Ralentit le mouvement lorsque c'est supérieur au DELAI pour FPS
        {
            update(&mesInfos->monAnim);
            avance(&mesInfos->monAnim);
            //printf("\nMON ANIM\n");
            //afficherPts(mesInfos->monAnim.polyPoisson);
            for(i=0;i<NB_ADVERSAIRES_MAX;i++)
                if(mesInfos->adversaires[i].vivant==1)
                {
                    updateAdv(&mesInfos->adversaires[i]);
                    avance(&mesInfos->adversaires[i]);
                }
            lasttime[2] = newtime[2];
        }
        //if(continuer==1)
        //    continuer = verification(&mesInfos->monAnim,mesInfos->adversaires,mesPoissonsBase,polyBase,ecran,*sons);

        newtime[3] = SDL_GetTicks();

        if(newtime[3] - lasttime[3] > 15)
        {
            if(mesInfos->pointsBoost+mesInfos->regenBoost<=mesInfos->boostTotal && mesInfos->boostActif==0)
                mesInfos->pointsBoost+=mesInfos->regenBoost;
            else if(mesInfos->boostActif==0)
                mesInfos->pointsBoost=mesInfos->boostTotal;

            else if(mesInfos->boostActif==1)
            {
                if(mesInfos->pointsBoost-mesInfos->consoBoost>=0)
                    mesInfos->pointsBoost-=mesInfos->consoBoost;
                else
                    mesInfos->pointsBoost=0;
            }
            lasttime[3] = newtime[3];
        }

        mesInfos->monAnim.tempsActuel = SDL_GetTicks();

        if(mesInfos->monAnim.tempsActuel - mesInfos->monAnim.tempsPasse > mesInfos->monAnim.temps)  //changement d'anim
        {
            if(mesInfos->monAnim.animActuelle+1<mesInfos->monAnim.animsEntite[mesInfos->monAnim.etat].anims_totales)
                {mesInfos->monAnim.animActuelle++;}
            else
                {mesInfos->monAnim.animActuelle=0;}
            mesInfos->monAnim.tempsPasse = mesInfos->monAnim.tempsActuel;
        }


        for(i=0;i<NB_ADVERSAIRES_MAX;i++)
        {
            if(mesInfos->adversaires[i].vivant==1)
            {
                mesInfos->adversaires[i].tempsActuel = SDL_GetTicks();

                if(mesInfos->adversaires[i].tempsActuel - mesInfos->adversaires[i].tempsPasse > mesInfos->adversaires[i].temps)  //changement d'anim
                {
                    if(mesInfos->adversaires[i].animActuelle+1<mesInfos->adversaires[i].animsEntite[mesInfos->adversaires[i].etat].anims_totales)
                        {mesInfos->adversaires[i].animActuelle++;}
                    else
                        {mesInfos->adversaires[i].animActuelle=0;}
                    mesInfos->adversaires[i].tempsPasse = mesInfos->adversaires[i].tempsActuel;
                }
            }
        }
        mesInfos->continuer = verification(&mesInfos->monAnim,mesInfos->adversaires,mesInfos->mesPoissonsBase,mesInfos->polyBase,mesInfos->ecran,*mesInfos->sons);
        SDL_Delay(1);
        pthread_testcancel();
    }

    pthread_exit(NULL);
    return NULL;
}

int verification(Poissons *monAnim,Poissons adversaires[],AnimPoissons monPoisson[2],Polygone geomPoisson[2],SDL_Surface *ecran,Audio sons)
{
    int i=0;
    int resultat=1;
    for(i=0;i<NB_ADVERSAIRES_MAX;i++)
    {
        if(adversaires[i].vivant == 1)
        {
            /*if((((adversaires[i].position.x >= monAnim->position.x) &&
                (adversaires[i].position.x < monAnim->position.x+monAnim->animsEntite[0].animation[0]->w))||  //si ça touche de manière à ce que le point posision.x soit dans l'anim (voir schéma_1)          (() &&
                ((adversaires[i].position.x < monAnim->position.x) &&
                (adversaires[i].position.x + adversaires[i].animsEntite[0].animation[0]->w >= monAnim->position.x)))&&   //si ça touche mais que le point x soit avant l'image

                (((adversaires[i].position.y >= monAnim->position.y) &&
                (adversaires[i].position.y < monAnim->position.y+monAnim->animsEntite[0].animation[0]->h))||
                ((adversaires[i].position.y < monAnim->position.y) &&
                (adversaires[i].position.y + adversaires[i].animsEntite[0].animation[0]->h >= monAnim->position.y))))*/

            if(collisionPolyPolySimple(*monAnim,adversaires[i])==1)
            {

                /*SDL_Delay(1000);
                stop = 1;*/
                if(monAnim->poids>adversaires[i].poids)
                {
                    adversaires[i].vivant = 0;
                    adversaires[i].vivantAvant=1;

                    grossirPoisson(monAnim,monPoisson,adversaires[i].poids);
                    adaptPoly(geomPoisson,&monAnim->polyPoisson,POISSON_LG,monAnim->grosseur);

                    printf("\n\n\t\t\t%d < %d ==== mangé",monAnim->poids,adversaires[i].poids);
                    if(monAnim->poids>=POIDS_GAIN)
                    {
                        resultat=GAGNE;
                        break;
                    }
                    else
                    {/*
                        for(j=0;j<adversaires[i].animsEntite[0].anims_totales;j++)
                        {
                            SDL_FreeSurface(adversaires[i].animsEntite[0].anim[j]);
                        }
                        for(j=0;j<adversaires[i].animsEntite[1].anims_totales;j++)
                        {
                            SDL_FreeSurface(adversaires[i].animsEntite[1].anim[j]);
                        }
                        SDL_FreeSurface(adversaires[i].rectangle);*/
                        /**FAIRE FREE A LA FIN PUIS REINITANIM**/
                    }
                    FMOD_System_PlaySound(sons.system, CROC, sons.croc, 0, NULL);

                }
                else if (monAnim->poids<adversaires[i].poids)  //else if car il y a la possibilité que ça soit égal !
                {
                    printf("\n\n\t\t\t%d < %d ==== PERDUUUU",monAnim->poids,adversaires[i].poids);
                    //SDL_Delay(1000);
                    resultat = PERDU;
                    FMOD_System_PlaySound(sons.system, MEURT, sons.meurt, 0, NULL);
                    break;

                }

            }

        }
    }
    return resultat;
}

void update(Poissons *monAnim)
{
    if(monAnim->mouvement[DROITE] == 0 && monAnim->mouvement[GAUCHE] == 0 && monAnim->mouvement[BAS] == 0 && monAnim->mouvement[HAUT] == 0)
    {
        if(monAnim->etat == BOUGE)
            monAnim->animActuelle=0;
        monAnim->etat = BOUGEPAS;

    }
    else
    {
        if(monAnim->etat == BOUGEPAS)
            monAnim->animActuelle=0;
        monAnim->etat = BOUGE;
    }


    if(monAnim->mouvement[DROITE]==1)
    {
        if(monAnim->position.x+monAnim->animsEntite[0].animationGauche[0]->w+monAnim->vitesse+monAnim->mouvement[ACCELERE]<ECRAN_X)
            monAnim->position.x+=(monAnim->vitesse+monAnim->mouvement[ACCELERE]);
        else if(monAnim->position.x+monAnim->animsEntite[0].animationGauche[0]->w+monAnim->vitesse<ECRAN_X)
            monAnim->position.x+=(monAnim->vitesse);

        monAnim->orientation = DROITE;
    }
    else if (monAnim->mouvement[GAUCHE]==1)
    {
        if(monAnim->position.x-monAnim->vitesse-monAnim->mouvement[ACCELERE]>0)
            monAnim->position.x-=(monAnim->vitesse+monAnim->mouvement[ACCELERE]);
        else if(monAnim->position.x-monAnim->vitesse>0)
            monAnim->position.x-=(monAnim->vitesse);

        monAnim->orientation = GAUCHE;
    }
    if(monAnim->mouvement[BAS]==1)
    {
        if(monAnim->position.y+monAnim->animsEntite[0].animationGauche[0]->h+monAnim->vitesse+monAnim->mouvement[ACCELERE]<ECRAN_Y)
            monAnim->position.y+=(monAnim->vitesse+monAnim->mouvement[ACCELERE]);
        else if(monAnim->position.y+monAnim->animsEntite[0].animationGauche[0]->h+monAnim->vitesse<ECRAN_Y)
            monAnim->position.y+=(monAnim->vitesse);
    }
    else if (monAnim->mouvement[HAUT]==1)
    {
        if(monAnim->position.y-monAnim->vitesse-monAnim->mouvement[ACCELERE]>0)
            monAnim->position.y-=(monAnim->vitesse+monAnim->mouvement[ACCELERE]);
        else if(monAnim->position.y-monAnim->vitesse>0)
            monAnim->position.y-=(monAnim->vitesse);
    }
}


void updateAdv(Poissons *monAnim)
{
    if(monAnim->mouvement[DROITE] == 0 && monAnim->mouvement[GAUCHE] == 0 && monAnim->mouvement[BAS] == 0 && monAnim->mouvement[HAUT] == 0)
    {
        if(monAnim->etat == BOUGE)
            monAnim->animActuelle=0;
        monAnim->etat = BOUGEPAS;

    }
    else
    {
        if(monAnim->etat == BOUGEPAS)
            monAnim->animActuelle=0;
        monAnim->etat = BOUGE;
    }


    if(monAnim->mouvement[DROITE]==1)
    {
        if(monAnim->position.x+monAnim->animsEntite[0].animationGauche[0]->w+monAnim->vitesse+monAnim->mouvement[ACCELERE]<ECRAN_X)
            monAnim->position.x+=(monAnim->vitesse+monAnim->mouvement[ACCELERE]);
        else if(monAnim->position.x+monAnim->animsEntite[0].animationGauche[0]->w+monAnim->vitesse<ECRAN_X)
            monAnim->position.x+=(monAnim->vitesse);
        else
        {
            monAnim->mouvement[DROITE] = 0;
            monAnim->mouvement[GAUCHE] = 1;
        }

        monAnim->orientation = DROITE;
    }
    else if (monAnim->mouvement[GAUCHE]==1)
    {
        if(monAnim->position.x-monAnim->vitesse-monAnim->mouvement[ACCELERE]>0)
            monAnim->position.x-=(monAnim->vitesse+monAnim->mouvement[ACCELERE]);
        else if(monAnim->position.x-monAnim->vitesse>0)
            monAnim->position.x-=(monAnim->vitesse);
        else
        {
            monAnim->mouvement[DROITE] = 1;
            monAnim->mouvement[GAUCHE] = 0;
        }

        monAnim->orientation = GAUCHE;
    }
    if(monAnim->mouvement[BAS]==1)
    {
        if(monAnim->position.y+monAnim->animsEntite[0].animationGauche[0]->h+monAnim->vitesse+monAnim->mouvement[ACCELERE]<ECRAN_Y)
            monAnim->position.y+=(monAnim->vitesse+monAnim->mouvement[ACCELERE]);
        else if(monAnim->position.y+monAnim->animsEntite[0].animationGauche[0]->h+monAnim->vitesse<ECRAN_Y)
            monAnim->position.y+=(monAnim->vitesse);
        else
        {
            monAnim->mouvement[BAS] = 0;
            monAnim->mouvement[HAUT] = 1;
        }
    }
    else if (monAnim->mouvement[HAUT]==1)
    {
        if(monAnim->position.y-monAnim->vitesse-monAnim->mouvement[ACCELERE]>0)
            monAnim->position.y-=(monAnim->vitesse+monAnim->mouvement[ACCELERE]);
        else if(monAnim->position.y-monAnim->vitesse>0)
            monAnim->position.y-=(monAnim->vitesse);
        else
        {
            monAnim->mouvement[BAS] = 1;
            monAnim->mouvement[HAUT] = 0;
        }
    }

    /*
    On va lancer un dé qui détermine sile poisson change de direction ou reste comme ça
    on dira 1 chance sur 10*FPS de changer de direction (il ne faut pas oublier que l'update se fera chaque FPS)
    */
}


void changeDirection (Poissons *monAnim)
{
    int de=0;//dé

    de=rand()%(10);
    if(de == 0) //si on a eu cette chance !
    {
        de=rand()%3;
        switch(de)
        {
        case 0:
            monAnim->mouvement[GAUCHE] = 1;
            monAnim->mouvement[DROITE] = 0;
            break;

        case 1:
            monAnim->mouvement[DROITE] = 1;
            monAnim->mouvement[GAUCHE] = 0;//si ça fait 0, alors on recule _ si ça fait 1, il avance
            break;

        case 2:
            monAnim->mouvement[DROITE] = 0;
            monAnim->mouvement[GAUCHE] = 0;//si ça fait 2, alors on s'arrete => 1/3 de s'arreter ! on va voir ce que ça donne ...
            break;

        default:
            break;
        }
    }

    de=rand()%(10);
    if(de == 0) //si on a eu cette chance !
    {
        de=rand()%3;
        switch(de)
        {
        case 0:
            monAnim->mouvement[HAUT] = 1;
            monAnim->mouvement[BAS] = 0;
            break;

        case 1:
            monAnim->mouvement[HAUT] = 1;
            monAnim->mouvement[BAS] = 0;//si ça fait 0, alors on recule _ si ça fait 1, il avance
            break;

        case 2:
            monAnim->mouvement[HAUT] = 0;
            monAnim->mouvement[BAS] = 0;//si ça fait 2, alors on s'arrete => 1/3 de s'arreter ! on va voir ce que ça donne ...
            break;

        default:
            break;
        }
    }

}


/****************************    ***     *********************************/
/*****************************    *     **********************************/
/****************************** MODIFS ***********************************/
/*****************************    *     **********************************/
/****************************    ***     *********************************/
/*
void initAnim(AnimPoissons monPoisson[2],SDL_Surface *ecran);
void reinitAnimPoisson(Poissons *monAnim,AnimPoissons monPoisson[2],SDL_Surface *ecran,int typePoisson);
void spawnPoissons (Poissons *monAnim,AnimPoissons monPoisson[2],SDL_Surface *ecran,int taille);
void FreeAnim(Poissons *monAnim);



*/
    /*
    Au final cte fonction, elle remplit une structure nommée "monPoisson[]" qui est un tableau d'animation ; il contient dans chacune
    de ses cases les animations du poisson concerné qui bouge et qui bouge pas.
    La structure contient ainsi tous les poissons qui existent. Il faut donc lancer initAnim dès le départ pour charger tout, et on les manipules après ! :p

    Après, on aura juste à copier ces animations dans les structures de chaque poisson :) on a formé ici nos anims de base.
    */
void initAnim(AnimPoissons monPoisson[2/*ici, nb de sortes de poisson _ sera à 12 à terme !*/],SDL_Surface *ecran)
{
    int i=0,j=0;
    char nomImg[100];
    SDL_Surface *imgTemp=NULL;
    int sortePoisson = 0;



    /**relève le nombre de sprits**/
    FILE *fichier = NULL;
    fichier=fopen("Taille.txt","r");
    fscanf(fichier,"ImgStatique %d\nImgMouv %d",&monPoisson[0].animPoissons[BOUGEPAS].anims_totales,&monPoisson[0].animPoissons[BOUGE].anims_totales);
    fclose(fichier);//on enregistre la taille et on la copie dans les autres

    for(sortePoisson=0;sortePoisson<2;sortePoisson++)//on charge les anims de chaque couleur et forme de poisson
    {

        if(sortePoisson!=0) //car on l'a déjà chargé juste au dessus, lors de la lecture de fichier :p
        {
            monPoisson[sortePoisson].animPoissons[BOUGEPAS].anims_totales = monPoisson[0].animPoissons[BOUGEPAS].anims_totales;
            monPoisson[sortePoisson].animPoissons[BOUGE].anims_totales = monPoisson[0].animPoissons[BOUGE].anims_totales;//copie des tailles

        }
        /**Alloue la mémoire à l'anim de base**/
        monPoisson[sortePoisson].animPoissons[BOUGEPAS].animationGauche = (SDL_Surface**)malloc(monPoisson[sortePoisson].animPoissons[BOUGEPAS].anims_totales * sizeof(SDL_Surface*));
        monPoisson[sortePoisson].animPoissons[BOUGE].animationGauche = (SDL_Surface**)malloc(monPoisson[sortePoisson].animPoissons[BOUGE].anims_totales * sizeof(SDL_Surface*));

        monPoisson[sortePoisson].animPoissons[BOUGEPAS].animationDroite = (SDL_Surface**)malloc(monPoisson[sortePoisson].animPoissons[BOUGEPAS].anims_totales * sizeof(SDL_Surface*));
        monPoisson[sortePoisson].animPoissons[BOUGE].animationDroite = (SDL_Surface**)malloc(monPoisson[sortePoisson].animPoissons[BOUGE].anims_totales * sizeof(SDL_Surface*));

        printf("\n\n\t\t\tchargeee\n");

        /**monPoisson[sortePoisson].animPoissons->charge = 1;**/
        /**ENLEVER CTE VARIABLE**/



        /***AU LIEU DE LOADER CA 15 FOIS, LE FAIRE UNE FOIS ET COPIER DEDANS APRES***/
        for(j=0;j<2;j++)
        {
            for(i=0;i<monPoisson[sortePoisson].animPoissons[j].anims_totales;i++)
            {
                if(j==BOUGEPAS)
                    sprintf(nomImg, "Images/Poisson_%d/Poissons_bougePas_Gauche/Poissons_bougePas_%d.png",sortePoisson,i);
                else
                    sprintf(nomImg, "Images/Poisson_%d/Poissons_bouge_Gauche/Poissons_bouge_%d.png",sortePoisson,i);

                printf("%s\n",nomImg);
                imgTemp = IMG_Load(nomImg);
                imgTemp = SDL_DisplayFormatAlpha(imgTemp);//on a le droit ??
                monPoisson[sortePoisson].animPoissons[j].animationGauche[i] = zoomSurface(imgTemp,ZOOM_X,ZOOM_Y,0);   //Mise à l'échelle
                SDL_FreeSurface(imgTemp);

                if(monPoisson[sortePoisson].animPoissons[j].animationGauche[i] == NULL)
                    printf("fail");
            }

            for(i=0;i<monPoisson[sortePoisson].animPoissons[j].anims_totales;i++)
            {
                if(j==BOUGEPAS)
                    sprintf(nomImg, "Images/Poisson_%d/Poissons_bougePas_Droite/Poissons_bougePas_%d.png",sortePoisson,i);
                else
                    sprintf(nomImg, "Images/Poisson_%d/Poissons_bouge_Droite/Poissons_bouge_%d.png",sortePoisson,i);

                printf("%s\n",nomImg);
                imgTemp = IMG_Load(nomImg);
                imgTemp = SDL_DisplayFormatAlpha(imgTemp);//on a le droit ??
                monPoisson[sortePoisson].animPoissons[j].animationDroite[i] = zoomSurface(imgTemp,ZOOM_X,ZOOM_Y,0);   //Mise à l'échelle
                SDL_FreeSurface(imgTemp);

                if(monPoisson[sortePoisson].animPoissons[j].animationDroite[i] == NULL)
                    printf("fail");
            }
        }
    }

    printf("\n--- Chargement des anims de Base terminé ---\n");

}

/*****/

/*
La fonction reinitAnim doit :
    - créer un nouveau joueur en l'initialisant avec reinitAnim => crée les surfaces en les copiant + alloue mémoire
    - réinitialise toutes les caractéristiques à leur valeur de base (ex: vitesse, position ...)
    - met également leur variable de vie à 0, sauf pour le perso principal
    - elle devra pouvoir être utilisée par seulement un Poisson car, lorsqu'il meurt, il se réinitialise pour être prêt à respawner !
*/
void reinitAnimPoisson(Poissons *monAnim,AnimPoissons monPoisson[2],SDL_Surface *ecran,int typePoisson)
{
    int i=0,j=0;

    /**Récupération du nb d'anim totales**/
    monAnim->animsEntite[BOUGEPAS].anims_totales=monPoisson[0].animPoissons[BOUGEPAS].anims_totales;
    monAnim->animsEntite[BOUGE].anims_totales=monPoisson[0].animPoissons[BOUGE].anims_totales;


    /**alloue la mémoire nécessaire**/
    monAnim->animsEntite[BOUGEPAS].animationGauche = (SDL_Surface**)malloc(monAnim->animsEntite[BOUGEPAS].anims_totales * sizeof(SDL_Surface*));
    monAnim->animsEntite[BOUGE].animationGauche = (SDL_Surface**)malloc(monAnim->animsEntite[BOUGE].anims_totales * sizeof(SDL_Surface*));
    monAnim->animsEntite[BOUGEPAS].animationDroite = (SDL_Surface**)malloc(monAnim->animsEntite[BOUGEPAS].anims_totales * sizeof(SDL_Surface*));
    monAnim->animsEntite[BOUGE].animationDroite = (SDL_Surface**)malloc(monAnim->animsEntite[BOUGE].anims_totales * sizeof(SDL_Surface*));
    printf("%d o alloués\n",monAnim->animsEntite[BOUGEPAS].anims_totales * sizeof(SDL_Surface*));
    printf("%d o alloués\n",monAnim->animsEntite[BOUGE].anims_totales * sizeof(SDL_Surface*));

    //typePoisson
    /***********INITIALISER CA ************/


    /**Récupère les surfaces de base**/
    for(j=0;j<2;j++)
    {
        for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
        {
            monAnim->animsEntite[j].animationGauche[i] = SDL_ConvertSurface(monPoisson[typePoisson].animPoissons[j].animationGauche[i],monPoisson[typePoisson].animPoissons[j].animationGauche[i]->format,monPoisson[typePoisson].animPoissons[j].animationGauche[i]->flags);
            SDL_BlitSurface(monPoisson[typePoisson].animPoissons[j].animationGauche[i],NULL,monAnim->animsEntite[j].animationGauche[i],NULL);

            monAnim->animsEntite[j].animationDroite[i] = SDL_ConvertSurface(monPoisson[typePoisson].animPoissons[j].animationDroite[i],monPoisson[typePoisson].animPoissons[j].animationDroite[i]->format,monPoisson[typePoisson].animPoissons[j].animationDroite[i]->flags);
            SDL_BlitSurface(monPoisson[typePoisson].animPoissons[j].animationDroite[i],NULL,monAnim->animsEntite[j].animationDroite[i],NULL);
            //monAnim->animsEntite[j].animation[i] = (SDL_Surface*)zoomSurface(monAnim->animsEntite[j].animation[i],ZOOM_X,ZOOM_Y,0);  //on met à l'échelle
            //mise à l'échelle déjà effectuée !
        }
    }

    if(monAnim->animsEntite[0].animationGauche[0]==NULL || monAnim->animsEntite[0].animationDroite[0]==NULL)
        printf("merde");


    if(typePoisson==0)  //si c'est le poisson principal (le notre)
        monAnim->vivant = 1;
    else                //sinon, c'est un des bots, donc on le mettra en vie après, lors du lancement des dés pour le spawn
        monAnim->vivant = 0;

    monAnim->vivantAvant = 0;

    printf("\tcopie des anims terminée\n");

    //rectangle dans spawn en même temps que le zoom
    /**Initialisation ...**/
    monAnim->position.x=0;
    monAnim->position.y=0;
    monAnim->oldPos.x=0;
    monAnim->oldPos.y=0;
    monAnim->grosseur=1.0;
    monAnim->poids=100; //vu le ratio taille/poids (*100)
    monAnim->etat = BOUGEPAS;

    monAnim->typePoisson = typePoisson;
    monAnim->rectangle = NULL;
    if(typePoisson >= 1)
        monAnim->temps = 70;
    else
        monAnim->temps = 37;

    monAnim->tempsActuel = 0;
    monAnim->tempsPasse = 0;
    monAnim->animActuelle = 0;

    for(i=0;i<5;i++)
    {
        monAnim->mouvement[i] = 0;
        monAnim->lastMouvement[i] = 0;
    }
    monAnim->vitesse=0;
}

/*
La fonction spwan doit :
    - donner vie à un joueur aléatoire (prédéfini en entrée)
    - lui associer une vitesse, position, taille aléatoire
    - si la fonction spawn est utilisée avec le perso principal, on lui initialise les caractéristiques vitesse, position, taille pour un début de partie
    - elle se trouvera forcément après reinitAnim().
*/
void spawnPoissons (Poissons *monAnim,AnimPoissons monPoisson[2],SDL_Surface *ecran,int taille)
{
    int i=0,j=0;

    if(monAnim->typePoisson == 0)   //poisson principal
    {
        monAnim->poids = TAILLE_INITIALE;//pour 0.3 : 30g (ouais g parce que j'avais la flemme de faire un truc logique xD)
        monAnim->grosseur = (double)(monAnim->poids)/100;

        for(j=0;j<2;j++)
        {
            for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
            {
                SDL_FreeSurface(monAnim->animsEntite[j].animationGauche[i]);  //on libère
                SDL_FreeSurface(monAnim->animsEntite[j].animationDroite[i]);  //on libère

                monAnim->animsEntite[j].animationGauche[i] = (SDL_Surface*)malloc(sizeof(SDL_Surface*));  //on réalloue de la mémoire
                monAnim->animsEntite[j].animationDroite[i] = (SDL_Surface*)malloc(sizeof(SDL_Surface*));  //on réalloue de la mémoire

                monAnim->animsEntite[j].animationGauche[i] = zoomSurface(monPoisson[monAnim->typePoisson].animPoissons[j].animationGauche[i],monAnim->grosseur,monAnim->grosseur,0);
                monAnim->animsEntite[j].animationDroite[i] = zoomSurface(monPoisson[monAnim->typePoisson].animPoissons[j].animationDroite[i],monAnim->grosseur,monAnim->grosseur,0);
            }
        }   //fait grossir l'anim :)
        /**OU**/
        //grossir(monAnim,AnimPoissons,monAnim->grosseur);

        //monAnim->vitesse=3*ZOOM_X;//la vitesse est de 3 et s'adapte en fonction de ZOOM_X donc de la taille de la fenêtre

        definirVitesse(monAnim);
        printf("vitesse : %d\n",monAnim->vitesse);
        monAnim->rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, monAnim->animsEntite[0].animationGauche[0]->w, monAnim->animsEntite[0].animationGauche[0]->h, 32, 0, 0, 0, 0);
        SDL_FillRect(monAnim->rectangle, NULL, SDL_MapRGB(ecran->format, 255, 145, 7));

        monAnim->position.x = ECRAN_X/2-monAnim->animsEntite[0].animationGauche[0]->w/2;
        monAnim->position.y = ECRAN_Y/2-monAnim->animsEntite[0].animationGauche[0]->h/2;
        monAnim->oldPos.x = monAnim->position.x;
        monAnim->oldPos.y = monAnim->position.y;

        monAnim->framesEffacement=255;
    }
    else                            //ennemis
    {
        taille*=TAUX_GROSSEUR;  //en faisant ça, on définit si les poissons auront une chance de spawner d'un poids allant de 1 à la taille en entrée*taux ; ainsi
                                //, si taux = 1.5, on aura un poids aléatoire entre 0.01 et 1.5 fois plus grand que celui d'entrée. ça permet de faire
                                // spawner des poissons plus gros que le poisson principal dans ce cas là environ 1 fois sur 3.

        /**Taille aléatoire**/
        monAnim->grosseur = ((double)(rand() % taille)+1)/100;  //avec le taux de grosseur pris en compte
        monAnim->poids = monAnim->grosseur*100;

        printf("\n\tGrosseur = %f",monAnim->grosseur);
        for(j=0;j<2;j++)
        {
            for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
            {
                SDL_FreeSurface(monAnim->animsEntite[j].animationGauche[i]);  //on libère
                SDL_FreeSurface(monAnim->animsEntite[j].animationDroite[i]);  //on libère

                monAnim->animsEntite[j].animationGauche[i] = (SDL_Surface*)malloc(sizeof(SDL_Surface*));  //on réalloue de la mémoire
                monAnim->animsEntite[j].animationDroite[i] = (SDL_Surface*)malloc(sizeof(SDL_Surface*));  //on réalloue de la mémoire

                monAnim->animsEntite[j].animationGauche[i] = zoomSurface(monPoisson[monAnim->typePoisson].animPoissons[j].animationGauche[i],monAnim->grosseur,monAnim->grosseur,0);
                monAnim->animsEntite[j].animationDroite[i] = zoomSurface(monPoisson[monAnim->typePoisson].animPoissons[j].animationDroite[i],monAnim->grosseur,monAnim->grosseur,0);
            }
        }   //fait grossir l'anim :)

        /**Vitesse aléatoire**/
        //monAnim->vitesse=((rand() % (VITESSE_MAX - 2 + 1)) + 2)*ZOOM_X;//compris entre vitesse max et 1
        if(monAnim->poids<=30)
            monAnim->vitesse=VITESSE_PETIT*ZOOM_X;
        else if(monAnim->poids<=75)
            monAnim->vitesse=((rand() % (VITESSE_MOYEN - 1 + 1)) + 1)*ZOOM_X;//compris entre vitesse max et 1
        else if(monAnim->poids>100)
            monAnim->vitesse=((rand() % (VITESSE_GRAND - 1 + 1)) + 1)*ZOOM_X;//compris entre vitesse max et 1

        if (monAnim->vitesse==0)
            monAnim->vitesse=1;

        monAnim->rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, monAnim->animsEntite[0].animationGauche[0]->w, monAnim->animsEntite[0].animationGauche[0]->h, 32, 0, 0, 0, 0);
        SDL_FillRect(monAnim->rectangle, NULL, SDL_MapRGB(ecran->format, 255, 145, 7));

        /**Position aléatoire**/
        if(rand()%2 == 0)   //1 chance sur 2 d'être à gauche
        {
            monAnim->position.x = 0;
            monAnim->mouvement[DROITE] = 1;
        }
        else                //1 chance sur 2 d'être à droite
        {
            monAnim->position.x = ECRAN_X-monAnim->animsEntite[0].animationGauche[0]->w;
            monAnim->mouvement[GAUCHE] = 1;
        }

        monAnim->position.y = (rand()%(ECRAN_Y-monAnim->animsEntite[0].animationGauche[0]->h+1)+1);

        monAnim->oldPos.x = monAnim->position.x;
        monAnim->oldPos.y = monAnim->position.y;

        monAnim->etat = BOUGE;

        monAnim->framesEffacement=255;
        monAnim->vivant = 1;
    }
}

void nouveau(int *ajoutAdversaire)
{
    *ajoutAdversaire=rand()%4;//une chance sur deux
    //soit c'est 1, donc on ajoute un adversaire, soit c'est 0 et on en ajoute pas
}


void FreeAnim(Poissons *monAnim)
{
    int i=0,j=0;

    for(j=0;j<2;j++)
    {
        for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
        {
            SDL_FreeSurface(monAnim->animsEntite[j].animationGauche[i]);  //on libère
            SDL_FreeSurface(monAnim->animsEntite[j].animationDroite[i]);  //on libère
        }
    }   //fait grossir l'anim :)

    SDL_FreeSurface(monAnim->rectangle);
    free((void*)monAnim->animsEntite[0].animationGauche);
    free((void*)monAnim->animsEntite[1].animationGauche);
    free((void*)monAnim->animsEntite[0].animationDroite);
    free((void*)monAnim->animsEntite[1].animationDroite);
}


void My_SetAlpha32(SDL_Surface* src, const unsigned char val)
{
    int i,j,
        load;
    Uint32 pxlColor;
    Uint8* pxlPtr=(Uint8*)&pxlColor;

    SDL_LockSurface(src);

    for(j=0;j<src->h;j++)
    {
        for(i=load=0; i<src->w; i++, load=0)
        {
            pxlColor=getpixel(src,i,j);

            if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
            {
                pxlPtr[0]=(pxlPtr[0]>val)? val : pxlPtr[0];
                load=1;
            }
            else
            {
                pxlPtr[3]=(pxlPtr[3]>val)? val : pxlPtr[3];
                load=1;
            }

            if(load)
                putpixel(src,i,j,pxlColor);
        }
    }

    SDL_UnlockSurface(src);
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}






/*


void spawn(Poissons *monAnim,SDL_Surface *ecran,int taille)
{
    int j=0,i=0;

    taille*=TAUX_GROSSEUR;

    monAnim->grosseur = ((double)(rand() % taille)+1)/100;
    monAnim->poids = monAnim->grosseur*100;


    //printf("\ngrosseur : %f\tpoids : %d\n",monAnim->grosseur,monAnim->poids);
    monAnim->vitesse=((rand() % (VITESSE_MAX - 3 + 1)) + 3)*ZOOM_X;
    monAnim->etat = BOUGE;

    monAnim->vivant = 1;
    monAnim->vivantAvant = 0;


    for(j=0;j<2;j++)
    {
        for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
        {//rotozoomSurface(monAnim->animsEntite[j].animBase[i],0.0,monAnim->grosseur,NULL);
            //SDL_FreeSurface(monAnim->animsEntite[j].anim[i]);

            monAnim->animsEntite[j].anim[i] = zoomSurface(monAnim->animsEntite[j].animBase[i],monAnim->grosseur,monAnim->grosseur,0);
            monAnim->animsEntite[j].anim[i] = SDL_DisplayFormatAlpha(monAnim->animsEntite[j].anim[i]);
            //monAnim->animsEntite[j].anim[i]=monAnim->animsEntite[j].animBase[i];
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

    monAnim->rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, monAnim->animsEntite[0].taille_X, monAnim->animsEntite[0].taille_Y, 32, 0, 0, 0, 0);
    SDL_FillRect(monAnim->rectangle, NULL, SDL_MapRGB(ecran->format, 255, 145, 7));

    if(monAnim->position.x-ECRAN_X/2<0) //si plus proche de la gauche = va vers la droite et inversement
        monAnim->mouvement[AVANCE] = 1;
    else
        monAnim->mouvement[RECULE] = 1;

    if(rand()%2 == 0)
        monAnim->position.x = 0;
    else
        monAnim->position.x = ECRAN_X-monAnim->animsEntite[0].taille_X;


    monAnim->position.y = (rand()%(ECRAN_Y-monAnim->animsEntite[0].taille_Y));

    monAnim->oldPos.x = monAnim->position.x;
    monAnim->oldPos.y = monAnim->position.y;
}

void initAnim(Poissons *monAnim,SDL_Surface *ecran,int sortePoisson,int *ajoutAdversaire)
{
    int i=0,j=0;
    char nomImg[100];
    int adversaire = 0;
    int img[4];
    SDL_Surface *imgTemp=NULL;

    if(sortePoisson == 6)
        adversaire=1;


    if(adversaire==1)
    {
        if(*ajoutAdversaire==1)
        {
            monAnim->vivant = 1;
            monAnim->vivantAvant = 0;
            *ajoutAdversaire = 0;
        }
        else
        {
            monAnim->vivant = 0;
            monAnim->vivantAvant = 0;
        }
    }
    else
    {
        monAnim->vivant = 1;
        monAnim->vivantAvant = 0;
    }


    monAnim->tempsActuel = 0;
    monAnim->tempsPasse = 0;

    if(monAnim->charge == 0)
    {
        //relève le nombre de sprits
        FILE *fichier = NULL;
        fichier=fopen("Taille.txt","r");
        fscanf(fichier,"ImgX %d\nImgY %d\nImgX %d\nImgY %d",&img[0],&img[1],&img[2],&img[3]);
        fclose(fichier);

        monAnim->animsEntite[BOUGEPAS].anims_totales = img[0]*img[1];
        monAnim->animsEntite[BOUGE].anims_totales = img[2]*img[3];

        monAnim->animsEntite[BOUGEPAS].anim = (SDL_Surface**)malloc(monAnim->animsEntite[BOUGEPAS].anims_totales * sizeof(SDL_Surface*));
        monAnim->animsEntite[BOUGE].anim = (SDL_Surface**)malloc(monAnim->animsEntite[BOUGE].anims_totales * sizeof(SDL_Surface*));

        monAnim->animsEntite[BOUGEPAS].animBase = (SDL_Surface**)malloc(monAnim->animsEntite[BOUGEPAS].anims_totales * sizeof(SDL_Surface*));
        monAnim->animsEntite[BOUGE].animBase = (SDL_Surface**)malloc(monAnim->animsEntite[BOUGE].anims_totales * sizeof(SDL_Surface*));

        monAnim->animsEntite[0].anims_X = img[0];
        monAnim->animsEntite[0].anims_Y = img[1];
        monAnim->animsEntite[1].anims_X = img[2];
        monAnim->animsEntite[1].anims_Y = img[3];
        printf("\n\n\t\t\tchargeee\n");
        monAnim->charge = 1;
    }

    monAnim->animActuelle = 0;
    for(i=0;i<5;i++)
    {
        monAnim->mouvement[i] = 0;
        monAnim->lastMouvement[i] = 0;
    }

        //nombreMystere = (rand() % (MAX - MIN + 1)) + MIN;                                      => (non c'est juste que le noir c'est le perso principal xD)
    for(j=0;j<2;j++)
    {

        for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
        {
            if(j==BOUGEPAS)
                sprintf(nomImg, "Images/Poisson_%d/Poissons_bougePas/Poissons_bougePas_%d.png",sortePoisson,i);
            else
                sprintf(nomImg, "Images/Poisson_%d/Poissons_bouge/Poissons_bouge_%d.png",sortePoisson,i);

            printf("%s\n",nomImg);
            imgTemp = IMG_Load(nomImg);
            imgTemp = SDL_DisplayFormatAlpha(imgTemp);//on a le droit ??
            monAnim->animsEntite[j].animBase[i] = zoomSurface(imgTemp,ZOOM_X,ZOOM_Y,0);


            monAnim->animsEntite[j].animBase[i] = IMG_Load(nomImg);   //on ouvre l'image avec le nom final
            monAnim->animsEntite[j].animBase[i] = SDL_DisplayFormatAlpha(monAnim->animsEntite[j].animBase[i]);
            imgTemp = zoomSurface(monAnim->animsEntite[j].animBase[i],ZOOM_X,ZOOM_Y,0);
            monAnim->animsEntite[j].animBase[i] = imgTemp;
            //printf("imgBase[%d][%d] = %d",j,i,monAnim->animsEntite[j].animBase[i]);
            SDL_FreeSurface(imgTemp);
            if(monAnim->animsEntite[j].animBase[i] == NULL)
                printf("fail");

        }
    }


        //rand()/(double)RAND_MAX ) * (b-a) + a
        //(double)rand()/(RAND_MAX)*(max-min)+min;


    if(adversaire==1 && monAnim->vivant==1)
    {
        spawn(monAnim,ecran,1,TAILLE_INITIALE);
        printf("\n\tspawné !");
    }
    else
    {
        printf("\n\tchargement de notre poisson ...");
        monAnim->poids = TAILLE_INITIALE;//pour 0.3 : 30g (ouais g parce que j'avais la flemme de faire un truc logique xD)
        monAnim->grosseur = (double)(monAnim->poids)/100;

        monAnim->vitesse=6*ZOOM_X;
        //printf("\n\n\t\t\tgros poisson qui mesure %f !!!!",monAnim->grosseur);
        monAnim->etat = BOUGEPAS;
        //SDL_Delay(1000);

        for(j=0;j<2;j++)
        {
            for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
            {
                //SDL_FreeSurface(monAnim->animsEntite[j].anim[i]);

                //SDL_Delay(500);

                monAnim->animsEntite[j].anim[i] = zoomSurface(monAnim->animsEntite[j].animBase[i],monAnim->grosseur,monAnim->grosseur,0);
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
        printf("\n\tchargé !");


        monAnim->rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, monAnim->animsEntite[0].taille_X, monAnim->animsEntite[0].taille_Y, 32, 0, 0, 0, 0);
        SDL_FillRect(monAnim->rectangle, NULL, SDL_MapRGB(ecran->format, 255, 145, 7));

    }



    monAnim->temps = 37;
    if(sortePoisson >=6)
        monAnim->temps = 70;

}*/


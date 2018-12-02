#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include <math.h>

#include "collision.h"
#include "variables.h"


/*
Dans cette fonction, on va :
        - chopper les points dans le doc .txt du dossier "data"
        - calculer les eq de droite
        - rendre convexe les poly concave avec la technique des triangles

Tout ça engendre des variables pour :
        - tous les points pour les deux polygones (y compris les triangles)
        - toutes les droites d'eq
        - un tableau genre "geomPoissons[2]" qui contiendra tout ça (les pos de base à la limite)
        - dans les variables Poissons, faudra créer une variable geomPoissons aussi qui regroupera la geom du poisson concerné ; cette dernière sera updatée avec adaptPolyy
*/
void initPoly(Polygone geomPoisson[2])
{
    int i=0;
    //Polygone pol_ABCDE,pol_FGHIJ;

    FILE *fichier = NULL;
    fichier = fopen("Data/PointsPoissons.txt","r");
    if(fichier == NULL)
    {
        printf("Erreur de lecture du fichier !");
        exit(1);
    }

    fscanf(fichier,"%d sommets et %d sommets",&geomPoisson[POISSON_LG].nbPoints,&geomPoisson[POISSON_HT].nbPoints);
    fgetc(fichier);
    fgetc(fichier);
    printf("\n%d et %d points ! \n\n",geomPoisson[POISSON_LG].nbPoints,geomPoisson[POISSON_HT].nbPoints);

    geomPoisson[POISSON_LG].sommet = malloc(geomPoisson[POISSON_LG].nbPoints * sizeof(Point));
    geomPoisson[POISSON_LG].droites = malloc(geomPoisson[POISSON_LG].nbPoints * sizeof(EqDroite));

    geomPoisson[POISSON_HT].sommet = malloc(geomPoisson[POISSON_HT].nbPoints * sizeof(Point));
    geomPoisson[POISSON_HT].droites = malloc(geomPoisson[POISSON_HT].nbPoints * sizeof(EqDroite));

    for(i=0;i<geomPoisson[POISSON_LG].nbPoints;i++)
    {
        if(i+1<geomPoisson[POISSON_LG].nbPoints)
            fscanf(fichier,"%f,%f - ",&geomPoisson[POISSON_LG].sommet[i].x,&geomPoisson[POISSON_LG].sommet[i].y);
        else
            fscanf(fichier,"%f,%f",&geomPoisson[POISSON_LG].sommet[i].x,&geomPoisson[POISSON_LG].sommet[i].y);
        //printf("\tsommet %d : %f;%f\n",i,pol_ABCDE.sommet[i].x,pol_ABCDE.sommet[i].y);
        geomPoisson[POISSON_LG].sommet[i].x = (int)(geomPoisson[POISSON_LG].sommet[i].x*ZOOM_X);
        geomPoisson[POISSON_LG].sommet[i].y = (int)(geomPoisson[POISSON_LG].sommet[i].y*ZOOM_Y);
    }

    printf("\n\n");
    fgetc(fichier);
    fgetc(fichier);

    for(i=0;i<geomPoisson[POISSON_HT].nbPoints;i++)
    {
        if(i+1<geomPoisson[POISSON_HT].nbPoints)
            fscanf(fichier,"%f,%f - ",&geomPoisson[POISSON_HT].sommet[i].x,&geomPoisson[POISSON_HT].sommet[i].y);
        else
            fscanf(fichier,"%f,%f",&geomPoisson[POISSON_HT].sommet[i].x,&geomPoisson[POISSON_HT].sommet[i].y);

        geomPoisson[POISSON_HT].sommet[i].x = (int)(geomPoisson[POISSON_HT].sommet[i].x*ZOOM_X);
        geomPoisson[POISSON_HT].sommet[i].y = (int)(geomPoisson[POISSON_HT].sommet[i].y*ZOOM_Y);
        //printf("\tsommet %d : %f;%f\n",i,pol_FGHIJ.sommet[i].x,pol_FGHIJ.sommet[i].y);
        //printf("sommet %d : %f;%f\n",i,pol_FGHIJ.sommet[i].x,pol_FGHIJ.sommet[i].y);
    }

    printf("calcul des eq de droites ...\n");


    calculVecteurEquaPoly(&geomPoisson[POISSON_LG]);
    calculVecteurEquaPoly(&geomPoisson[POISSON_HT]);

    afficherPts(geomPoisson[POISSON_LG]);
    afficherPts(geomPoisson[POISSON_HT]);

}

/*
Pour quand un pichon grandit, on réadapte les points (position des pts proportionnelle) MAIS PAS LES DROITES D'EQUATION (elles ne changent pas !)
La fonction sera appelée pour les adv aussi bien que pour le poisson principal ! :p
*/
void adaptPoly(Polygone geomPoisson[2],Polygone *poly,int type,double grosseur)
{
    int i=0;
    for(i=0;i<poly->nbPoints;i++)
    {
        poly->sommet[i].x = (int)(geomPoisson[type].sommet[i].x * grosseur);
        poly->sommet[i].y = (int)(geomPoisson[type].sommet[i].y * grosseur);
        poly->sommetBase[i].x = poly->sommet[i].x;
        poly->sommetBase[i].y = poly->sommet[i].y;
    }
    calculVecteurEquaPoly(poly);


    for(i=0;i<poly->nbPoints;i++)
    {
        poly->droitesBase[i].a = poly->droites[i].a;
        poly->droitesBase[i].A.x = poly->droites[i].A.x;
        poly->droitesBase[i].A.y = poly->droites[i].A.y;
        poly->droitesBase[i].B.x = poly->droites[i].B.x;
        poly->droitesBase[i].B.y = poly->droites[i].B.y;
        poly->droitesBase[i].b = poly->droites[i].b;
        poly->droitesBase[i].x = poly->droites[i].x;
        poly->droitesBase[i].verticale = poly->droites[i].verticale;
    }
}

/*
Cette fonction initialise tout simplement une variable de type polygone avec les valeurs de geomPoisson[2] : (elle les copie)
*/
void attribuerPoly (Polygone geomPoisson[2],Polygone *poly,int type)
{
    if(type>2)
    {
        printf("\nfction atrribuerPoly : mauvais type de poly selectionné !!! ");
        exit(1);
    }

    poly->nbPoints = geomPoisson[type].nbPoints;
    printf("\nfction atrribuerPoly : %d pts",poly->nbPoints);

    poly->sommet = malloc(poly->nbPoints * sizeof(Point));
    poly->droites = malloc(poly->nbPoints * sizeof(EqDroite));
    poly->sommetBase = malloc(poly->nbPoints * sizeof(Point));
    poly->droitesBase = malloc(poly->nbPoints * sizeof(EqDroite));

    printf("\nfction atrribuerPoly : taille allouée !");
}

void freePoly(Polygone *poly)
{
    free(poly->droitesBase);
    free(poly->sommetBase);
    free(poly->droites);
    free(poly->sommet);
}

int collisionPolyPolySimple(Poissons poisson_1,Poissons poisson_2)
{
    if(colliAABB(poisson_1,poisson_2))
    {
        //printf("\tcolli AABB !\n");
        //return 1;
        if(poisson_2.grosseur<0.1)  //si le poisson est trop petit, pas besoin de chercher la colli polyPolySegments
            return 1;

        if(colliPolyPolySegments(poisson_1.polyPoisson,poisson_2.polyPoisson))
        {
            printf("\tcolli Segments !\n");
            return 1;
        }
    }
    /**
    1 _ AABB ?
    2 _ si oui, colli segments contours ?
    3 _ sinon, verif qu'un point est pas dedans avec technique collipolypoly()

    **/
    return 0;
}

int colliAABB (Poissons poisson_1,Poissons poisson_2)
{
    if( ( ((poisson_2.position.x>=poisson_1.position.x )
       &&  (poisson_2.position.x<poisson_1.position.x+poisson_1.animsEntite[0].animation[0]->w))
       || ((poisson_2.position.x<poisson_1.position.x)
       &&  (poisson_2.position.x+poisson_2.animsEntite[0].animation[0]->w>=poisson_1.position.x)))
       &&
       ( ((poisson_2.position.y>=poisson_1.position.y )
       &&  (poisson_2.position.y<poisson_1.position.y+poisson_1.animsEntite[0].animation[0]->h))
       || ((poisson_2.position.y<poisson_1.position.y)
       &&  (poisson_2.position.y+poisson_2.animsEntite[0].animation[0]->h>=poisson_1.position.y)))
      )
    {
        return 1;
    }
    return 0;
}

int colliPolyPolySegments(Polygone poly_1,Polygone poly_2)
{
    int i,j;

    for(j=0;j<poly_1.nbPoints;j++)
    {
        for(i=0;i<poly_2.nbPoints;i++)
        {
            if(CollisionSegmentSegment(poly_1.droites[j],poly_2.droites[i],0))
            {
                printf("\ncolli entre segment %d et %d\n",j,i);
                return 1;
            }
        }
    }
    return 0;
}

void avance (Poissons *monPoisson)
{
    int i=0;
    //printf("\n\t\tdeplacement en %d en X et %d en Y !!",deplacement.x,deplacement.y);
    for(i=0;i<monPoisson->polyPoisson.nbPoints;i++)
    {
        monPoisson->polyPoisson.sommet[i].x = (int)(monPoisson->position.x + monPoisson->polyPoisson.sommetBase[i].x);
        monPoisson->polyPoisson.sommet[i].y = (int)(monPoisson->position.y + monPoisson->polyPoisson.sommetBase[i].y);
        monPoisson->polyPoisson.droites[i].A.x = (int)(monPoisson->position.x + monPoisson->polyPoisson.droitesBase[i].A.x);
        monPoisson->polyPoisson.droites[i].A.y = (int)(monPoisson->position.y + monPoisson->polyPoisson.droitesBase[i].A.y);
        monPoisson->polyPoisson.droites[i].B.x = (int)(monPoisson->position.x + monPoisson->polyPoisson.droitesBase[i].B.x);
        monPoisson->polyPoisson.droites[i].B.y = (int)(monPoisson->position.y + monPoisson->polyPoisson.droitesBase[i].B.y);
        if(monPoisson->polyPoisson.droites[i].verticale == 1)
            monPoisson->polyPoisson.droites[i].x = (int)(monPoisson->position.x + monPoisson->polyPoisson.droitesBase[i].x);

        calculEquationDroite(&monPoisson->polyPoisson.droites[i]);
    }
}

void calculEquationDroite (EqDroite *droite)
{
    /*
    droite->A.x = 0;
    droite->A.y = 3;
    droite->B.x = 2;
    droite->B.y = 3;*/

    if(droite->A.x==droite->B.x)     //là elle est verticale
    {
        /*si(yA<=2/3*1+1<=yB)
        colliiiiiiiiii*/
        droite->verticale=1;
        droite->a = -500;//je mets ça comme ça au débug je verrai d'où viens le problème
        droite->b = -500;
        droite->x = droite->A.x;//on est sous la forme x=k
    }
    else
    {
        droite->verticale=0;/*
        droite->a = (int)(((droite->B.y-droite->A.y)/(droite->B.x-droite->A.x)*1000));
        droite->a/=1000;
        droite->b = (int)((droite->A.y-droite->a*droite->A.x)*1000);
        droite->b/=1000;*/
        droite->a = (droite->B.y-droite->A.y)/(droite->B.x-droite->A.x);/*
        if(droite->a+0.1 >= (int)droite->a+1)
            droite->a=(int)droite->a+1;
        if(droite->a-0.1 <= (int)droite->a)
            droite->a=(int)droite->a;*/
        droite->b = (droite->A.y-droite->a*droite->A.x);/*
        if(droite->b+0.1 >= (int)droite->b+1)
            droite->b=(int)droite->b+1;
        if(droite->b-0.1 <= (int)droite->b)
            droite->b=(int)droite->b;*//***Ces arrondis de glandu du cuc m'ont fait bugguer toouuuuut le programmer bordel a cuc***/
        /*
    if(x+0.1 >= (int)x+1)//12.97 => 13
        x=(int)x+1;
    if(x-0.1 <= (int)x)//12.03 => 12
        x=(int)x;*/
        //on arrondit pour pas avoir 3.3[...]4 #TESTD'ESPOIR
        //printf("a %f et b %f\n",droite->a,droite->b);
        //droite->a*=10000;
        /*
        converti = droite->a*1000;
        test = floor(converti);
        printf("test = %f\n",test);
        droite->a = test/1000;
        printf("a:%f\n",droite->a);
        //droite->a /= 10000;
        droite->b*=10000;
        droite->b = floor((double)droite->b);
        droite->b /= 10000;*/
        //printf("a %f et b %f,%f %f\n",droite->a,droite->b,droite->a*10000,droite->b*10000);
        /*****changement de méthode à 7h09 du mat le 09/05/18 => on va manipuler des int xD
*****/
        //printf("puis %f",droite->b);
    }
    /*else //pente infinie :o donc on va tricher, et replacer les points de manière qu'elle ne soit plus tout a fait verticale dès le début
    {

        //droite->a=0; //sinon c'est une droite verticale   ! donc coeff dir = 0
        //droite->b=droite->B.y-droite->A.y;

    }*/
}

int CollisionSegmentSegment (EqDroite d1,EqDroite d2,int print)
{
    float x,y;
    if(d1.a==d2.a && d1.b==d2.b && d1.verticale!=1 && d2.verticale!=1)//si on a la meme droite, on en chope une autre ! (techniquement, y'a collision, mais faudrait enlever tous les pts en colli ...
        return 1;
    if(d1.a-d2.a==0 && d1.verticale!=1 && d2.verticale!=1)
        return 0;//paralleles !
        //printf("\n\t\tpas Paralleles");
    if(d1.verticale == 1 && d2.verticale ==1 && d1.x==d2.x)//pareil, techniquement y'a collision, mais c'est la merde dans le cas ici, donc on regardera avec d'autres points (c'est pas grave, pas besoin de s'occuper de cas aussi particuliers)
        return 1;
    if(d1.verticale == 1 && d2.verticale ==1)   //si ttes les deux verticales ...
        return 0;


    if(d1.verticale == 1)
    {
        /*si(yA<=2/3*1+1<=yB)
        colliiiiiiiiii*/

        if(print)
            printf("\n\td1 verticale _ colli probable en %f;%f",d1.x,d2.a*d1.x+d2.b);

        if(d2.A.x<=d1.x && d2.B.x>=d1.x)
        {
            if(d1.A.y<d1.B.y)
            {
                if(d2.a*d1.x+d2.b>=d1.A.y && d2.a*d1.x+d2.b<=d1.B.y)
                    return 1;
            }
            else
            {
                if(d2.a*d1.x+d2.b>=d1.B.y && d2.a*d1.x+d2.b<=d1.A.y)
                    return 1;
            }
        }
        return 0;
    }
    if(d2.verticale == 1)
    {
    if(print)
        printf("\n\td2 verticale _ colli probable en %f;%f",d2.x,d1.a*d2.x+d1.b);

        if(d1.A.x<=d2.x && d1.B.x>=d2.x)
        {
            if(d2.A.y<d2.B.y)
            {
                if(d1.a*d2.x+d1.b>=d2.A.y && d1.a*d2.x+d1.b<=d2.B.y)
                    return 1;
            }
            else
            {
                if(d1.a*d2.x+d1.b>=d2.B.y && d1.a*d2.x+d1.b<=d2.A.y)
                    return 1;
            }
        }
        return 0;
    }/**Voir calculVert.jpg**/


    x = (d2.b-d1.b)/(d1.a-d2.a);/*
    if(x+0.1 >= (int)x+1)//12.97 => 13
        x=(int)x+1;
    if(x-0.1 <= (int)x)//12.03 => 12
        x=(int)x;*/
    y = d1.a*x+d1.b;            //calcul des coordonnées du point où elles sont sécantes
/*
    if(y+0.1 >= (int)y+1)//12.97 => 13
        y=(int)y+1;
    if(y-0.1 <= (int)y)//12.03 => 12
        y=(int)y;*//**ARRONDI**/

    if(print)
    printf("\n\tsecantes en %f;%f  ",x,y);
    if(print)
    printf("[%f;%f] - [%f;%f] _ %fx+%f",d1.A.x,d1.A.y,d1.B.x,d1.B.y,d1.a,d1.b);
    //printf("\t[%f;%f] %fx+%f et %fx+%f",x,y,d1.a,d1.b,d2.a,d2.b);
    if(d1.A.x<d1.B.x)
    {
        if(d1.A.y<d1.B.y)
        {
                    //printf("\tici");
            if(y>=d1.A.y && y<=d1.B.y)
            {
                if(x>=d1.A.x && x<=d1.B.x)//si le pt es sur d1
                {
                    if(d2.A.x<d2.B.x)
                    {
                        if(d2.A.y<d2.B.y)
                        {
                            if(y>=d2.A.y && y<=d2.B.y)
                                if(x>=d2.A.x && x<=d2.B.x)  /**problème à débugger : mettre des inférieur OU EGAL avec les var x et y***/
                                    return 1;
                        }
                        else if (d2.B.y<d2.A.y)
                        {
                            if(y>=d2.B.y && y<=d2.A.y)
                                if(x>=d2.A.x && x<=d2.B.x)
                                    return 1;
                        }
                    }
                    else if(d2.B.x<d2.A.x)
                    {
                        if(d2.A.y<d2.B.y)
                        {
                            if(y>=d2.A.y && y<=d2.B.y)
                                if(x>=d2.B.x && x<=d2.A.x)
                                    return 1;
                        }
                        else if (d2.B.y<d2.A.y)
                        {
                            if(y>=d2.B.y && y<=d2.A.y)
                                if(x>=d2.B.x && x<=d2.A.x)
                                    return 1;
                        }
                    }
                }
            }
        }
        else if(d1.B.y<d1.A.y)
        {
            if(y>=d1.B.y && y<=d1.A.y)
            {
                if(x>=d1.A.x && x<=d1.B.x)
                {
                    if(d2.A.x<d2.B.x)
                    {
                        if(d2.A.y<d2.B.y)
                        {
                            if(y>=d2.A.y && y<=d2.B.y)
                                if(x>=d2.A.x && x<=d2.B.x)
                                    return 1;
                        }
                        else if (d2.B.y<d2.A.y)
                        {
                            if(y>=d2.B.y && y<=d2.A.y)
                                if(x>=d2.A.x && x<=d2.B.x)
                                    return 1;
                        }
                    }
                    else if(d2.B.x<d2.A.x)
                    {
                        if(d2.A.y<d2.B.y)
                        {
                            if(y>=d2.A.y && y<=d2.B.y)
                                if(x>=d2.B.x && x<=d2.A.x)
                                    return 1;
                        }
                        else if (d2.B.y<d2.A.y)
                        {
                            if(y>=d2.B.y && y<=d2.A.y)
                                if(x>=d2.B.x && x<=d2.A.x)
                                    return 1;
                        }
                    }
                }
            }
        }
    }
    else if(d1.B.x<d1.A.x)
    {
        //printf("\tla");
        //printf("\tla secante en (%f;%f)\n",x,y);
        if(d1.A.y<d1.B.y)
        {
            if(y>=d1.A.y && y<=d1.B.y)
            {
                if(x>=d1.B.x && x<=d1.A.x)
                {
                    if(d2.A.x<d2.B.x)
                    {
                        if(d2.A.y<d2.B.y)
                        {
                    //printf("A(%f;%f) B(%f;%f)",d2.A.x,d2.A.y,d2.B.x,d2.B.y);
                            if(y>=d2.A.y && y<=d2.B.y)
                                if(x>=d2.A.x && x<=d2.B.x)
                                    return 1;
                        }
                        else if (d2.B.y<d2.A.y)
                        {
                            if(y>=d2.B.y && y<=d2.A.y)
                                if(x>=d2.A.x && x<=d2.B.x)
                                    return 1;
                        }
                    }
                    else if(d2.B.x<d2.A.x)
                    {
                        if(d2.A.y<d2.B.y)
                        {
                            if(y>=d2.A.y && y<=d2.B.y)
                                if(x>=d2.B.x && x<=d2.A.x)
                                    return 1;
                        }
                        else if (d2.B.y<d2.A.y)
                        {
                            if(y>=d2.B.y && y<=d2.A.y)
                                if(x>=d2.B.x && x<=d2.A.x)
                                    return 1;
                        }
                    }
                }
            }
        }
        else if(d1.B.y<d1.A.y)
        {
            if(y>=d1.B.y && y<=d1.A.y)
            {
                if(x>=d1.B.x && x<=d1.A.x)
                {
                    if(d2.A.x<d2.B.x)
                    {
                        if(d2.A.y<d2.B.y)
                        {
                            if(y>=d2.A.y && y<=d2.B.y)
                                if(x>=d2.A.x && x<=d2.B.x)
                                    return 1;
                        }
                        else if (d2.B.y<d2.A.y)
                        {
                            if(y>=d2.B.y && y<=d2.A.y)
                                if(x>=d2.A.x && x<=d2.B.x)
                                    return 1;
                        }
                    }
                    else if(d2.B.x<d2.A.x)
                    {
                        if(d2.A.y<d2.B.y)
                        {
                            if(y>=d2.A.y && y<=d2.B.y)
                                if(x>=d2.B.x && x<=d2.A.x)
                                    return 1;
                        }
                        else if (d2.B.y<d2.A.y)
                        {
                            if(y>=d2.B.y && y<=d2.A.y)
                                if(x>=d2.B.x && x<=d2.A.x)
                                    return 1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}
        /**FCT° CollisionSegmentSegment**/

void afficherPts (Polygone poly)
{
    printf("\n");
    int i=0;
    for(i=0;i<poly.nbPoints;i++)
    {
        printf("\tsommet %d en [%f;%f]\t\tdroite : %f x + %f de [%f;%f] à [%f;%f]\n",i,poly.sommet[i].x,poly.sommet[i].y,poly.droites[i].a,poly.droites[i].b,poly.droites[i].A.x,poly.droites[i].A.y,poly.droites[i].B.x,poly.droites[i].B.y);
    }
}

void calculVecteurEquaPoly (Polygone *poly)
{
    int i=0;

    for(i=0;i<poly->nbPoints;i++)
    {
        poly->droites[i].A.x = poly->sommet[i].x;//le 1er pt de la droite du poly (du segment)
        poly->droites[i].A.y = poly->sommet[i].y;
        if(i+1<poly->nbPoints)
        {
            poly->droites[i].B.x = poly->sommet[i+1].x;//le 2e pt de la droite du poly (du segment)
            poly->droites[i].B.y = poly->sommet[i+1].y;
        }
        else
        {
            poly->droites[i].B.x = poly->sommet[0].x;
            poly->droites[i].B.y = poly->sommet[0].y;
        }//
        calculEquationDroite(&poly->droites[i]);
        //printf("\n\td2 : %fx+%f",poly->droites[i].a,poly->droites[i].b);

        //printf("\n\tvect %d : %f;%f",i,poly->vecteur[i].x,poly->vecteur[i].y);
    }
}




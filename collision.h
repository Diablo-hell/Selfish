#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED

#include "variables.h"

int colliAABB (Poissons poisson_1,Poissons poisson_2);
int collisionPolyPoly(Polygone poly_1,Polygone poly_2);
void avance (Poissons *monPoisson);
int CollisionSegmentSegment (EqDroite d1,EqDroite d2,int print);
void calculEquationDroite (EqDroite *droite);
void afficherPts (Polygone poly);
void calculVecteurEquaPoly (Polygone *poly);
int collisionPolyPolySimple(Poissons poisson_1,Poissons poisson_2);
int colliPolyPolySegments(Polygone poly_1,Polygone poly_2);
void freePoly(Polygone *poly);
void initPoly(Polygone geomPoisson[2],int direction);
void attribuerPoly (Polygone geomPoisson[2],Polygone *poly,int type);
void adaptPoly(Polygone geomPoisson[2][2],Polygone poly[2],int type,double grosseur);

#endif // COLLISION_H_INCLUDED

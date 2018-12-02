#ifndef GRAPHIQUE_H_INCLUDED
#define GRAPHIQUE_H_INCLUDED

void afficherFPS (SDL_Color couleur,TTF_Font *policePetite,SDL_Surface *ecran);
void grossirPoisson(Poissons *monAnim,AnimPoissons monPoisson[2],int mange);
void definirVitesse (Poissons *monAnim);
void SetSurfaceAlpha (SDL_Surface *surface, Uint8 alpha);


#endif // GRAPHIQUE_H_INCLUDED

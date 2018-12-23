#ifndef JEU_H_INCLUDED
#define JEU_H_INCLUDED

void* boucle(void *infos);
void initAnim(AnimPoissons monPoisson[2],SDL_Surface *ecran);
void reinitAnimPoisson(Poissons *monAnim,AnimPoissons monPoisson[2],SDL_Surface *ecran,int typePoisson);
void spawnPoissons (Poissons *monAnim,AnimPoissons monPoisson[2],SDL_Surface *ecran,int taille);
void FreeAnim(Poissons *monAnim);
int verification(Poissons *monAnim,Poissons adversaires[],AnimPoissons monPoisson[2],Polygone geomPoisson[2][2],SDL_Surface *ecran,Audio sons);

void update(Poissons *monAnim);
void jeu(SDL_Surface *ecran,Audio *sons);
void updateAdv(Poissons *monAnim);
void changeDirection (Poissons *monAnim);
void nouveau(int *ajoutAdversaire);
void mooveMap(Poissons monAnim, int *xscroll, int*yscroll);


void My_SetAlpha32(SDL_Surface* src, const unsigned char val);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 getpixel(SDL_Surface *surface, int x, int y);

#endif // JEU_H_INCLUDED

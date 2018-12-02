    temp = IMG_Load("Images/fond.jpg");
    fond = SDL_DisplayFormat(temp);
    fond = zoomSurface(fond,ZOOM_X,ZOOM_Y,0);
    //ça c'est pour le fond, il est mis à l'échelle avec un zoom
    //il est chargé dans la fonction principale "jeu"

    //FONCTION INIT
    for(j=0;j<2;j++)
    {
        for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
        {
            if(j==BOUGEPAS)
                sprintf(nomImg, "Images/Poisson_%d/Poissons_bougePas/Poissons_bougePas_%d.png",sortePoisson,i);
            else
                sprintf(nomImg, "Images/Poisson_%d/Poissons_bouge/Poissons_bouge_%d.png",sortePoisson,i);

            printf("%s\n",nomImg);  //là je marque juste on s'en fout

            monAnim->animsEntite[j].animBase[i] = IMG_Load(nomImg);   //on ouvre l'image avec le nom
            //Ce que je fait, c'est que je load tout en taille normale dans "anim.base" et après je la traite en dessous par exemple pour le spawn des bots
            monAnim->animsEntite[j].animBase[i] = zoomSurface(monAnim->animsEntite[j].animBase[i],ZOOM_X,ZOOM_Y,0); //on met a l'echelle

            if(monAnim->animsEntite[j].animBase[i] == NULL)
                printf("fail");
        }
    }


    //FONCTION SPAWN
    for(j=0;j<2;j++)
    {
        for(i=0;i<monAnim->animsEntite[j].anims_totales;i++)
        {

            monAnim->animsEntite[j].anim[i] = rotozoomSurface(monAnim->animsEntite[j].animBase[i],0.0,monAnim->grosseur,0)/*zoomSurface(monAnim->animsEntite[j].animBase[i],monAnim->grosseur,monAnim->grosseur,0)*/;

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

        }
    }

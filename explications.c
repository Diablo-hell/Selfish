
    //INITIALISATION


    SDL_Event event;
    while (continuer == 1)
    {
        SDL_PollEvent(&event);
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

                    //...déplacement
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                    //...déplacement
                }
                break;
        }

        //avant que l'on rentre dans le code problématique, quelques infos :
        //la structure "monAnim" et "adversaires" comportent tout ce qui est sprites, tailles, positions, ... des poissons (du notre pour monAnim et des adversaires pour l'autre)
        //tempsMaintenant et tempsPrecedent sont juste des int pour pouvoir calculer les fps (pareil pour lastTime et newTime, je sais pas trop pourquoi j'ai fait 2 variables mais c'est pas le problème xD)
        //ecran et fond sont des SDL_Surface *
        //et après je crois que c'est plutôt clair ; si y'a un truc que vous comprenez pas, demandez moi

        tempsMaintenant = SDL_GetTicks();

        if(tempsMaintenant - tempsPrecedent > 1000/FPS && continuer==1)   //réglage des FPS
        {
            update(&monAnim); //là je mets à jour les poissons ( pour les déplacements)
            for(i=0;i<NB_ADVERSAIRES_MAX;i++)//pour chaque poisson qui existe
                if(adversaires[i].vivant==1)//et qui soit vivant
                    updateAdv(&adversaires[i]);//j'update differement, je les fais se déplacer aléatoirement


            clipper.x = monAnim.oldPos.x;   //là j'utilise la technique du clipper pour optimiser mes animations _ ici c'est celui du poisson du joueur
            clipper.y = monAnim.oldPos.y;
            clipper.h = monAnim.animsEntite[monAnim.etat].taille_Y;
            clipper.w = monAnim.animsEntite[monAnim.etat].taille_X;

            SDL_SetClipRect(ecran,&clipper);

            positionFond.x = 0;
            positionFond.y = 0;

            SDL_BlitSurface(fond, NULL, ecran, &positionFond);

            SDL_SetClipRect(ecran,NULL);

            monAnim.oldPos.x = monAnim.position.x;
            monAnim.oldPos.y = monAnim.position.y;


            //...j'affiche aussi le score avec un autre clipper

            for(i=0;i<NB_ADVERSAIRES_MAX;i++)
            {
                if(adversaires[i].vivant==1)    //je fais le clipper pour tous les autres poissons ennemis sur la map
                {
                    clipper.x = adversaires[i].oldPos.x;
                    clipper.y = adversaires[i].oldPos.y;
                    clipper.h = adversaires[i].animsEntite[adversaires[i].etat].taille_Y;//ça faut pas en avoir peur, c'est juste la taille en pixel du poisson en question (parce que j'ai plusieurs sortes de poissons)
                    clipper.w = adversaires[i].animsEntite[adversaires[i].etat].taille_X;

                    SDL_SetClipRect(ecran,&clipper);

                    positionFond.x = 0;
                    positionFond.y = 0;

                    SDL_BlitSurface(fond, NULL, ecran, &positionFond);

                    SDL_SetClipRect(ecran,NULL);
                    adversaires[i].oldPos.x=adversaires[i].position.x;
                    adversaires[i].oldPos.y=adversaires[i].position.y;
                }
            }

            //(les clippers servent à "effacer" les poissons de l'écran et éviter d'effacer tout l'écran et réafficher un a un les poissons (+le fond + les scores ... bref ))


            SDL_BlitSurface(monAnim.animsEntite[monAnim.etat].anim[monAnim.animActuelle],NULL,ecran,&monAnim.position); //je blitte notre poisson
            for(i=0;i<NB_ADVERSAIRES_MAX;i++)
                if(adversaires[i].vivant==1)
                    SDL_BlitSurface(adversaires[i].animsEntite[adversaires[i].etat].anim[adversaires[i].animActuelle],NULL,ecran,&adversaires[i].position);//et là les autres


            SDL_BlitSurface(texte, NULL, ecran, 0); //là c'est juste le score


            SDL_Flip(ecran);


            //là ça serait la fin de la fonction affichage





            monAnim.tempsActuel = SDL_GetTicks();

            if(monAnim.tempsActuel - monAnim.tempsPasse > monAnim.temps)  //changement d'anim pour notre poisson
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

                    if(adversaires[i].tempsActuel - adversaires[i].tempsPasse > adversaires[i].temps)  //changement d'anim pour tous les autres
                    {
                        if(adversaires[i].animActuelle+1<adversaires[i].animsEntite[adversaires[i].etat].anims_totales)
                            {adversaires[i].animActuelle++;}
                        else
                            {adversaires[i].animActuelle=0;}
                        adversaires[i].tempsPasse = adversaires[i].tempsActuel;
                    }
                }
            }


            continuer = verification(&monAnim,adversaires,ecran);//je vérifie ce qui se passe. le "continuer =" c'est parce que cette fonction renvoit 1 pour "il ne s'est rien passé" et des valeurs différentes sinon. Et ce continuer est
            tempsPrecedent = tempsMaintenant;

        }
        else if (continuer==1)
        {
                SDL_Delay((1000/FPS)-(tempsMaintenant-tempsPrecedent)); //FPS
        }
    }

    //FIN DU JEU

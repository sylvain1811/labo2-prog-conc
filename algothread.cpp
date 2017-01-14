#include "algothread.h"
#include <QTime>
#include <cstdlib>

AlgoThread::AlgoThread(MainWindow* mainWindow,int _nbSite,int _nbHabitants,int _nbBorne,int _nbVelo)
{
    nbSite = _nbSite;
    nbHabitants = _nbHabitants;
    nbBorne = _nbBorne;
    nbVelo = _nbVelo;

    listSite = new QList<site*>();

    connect(this,SIGNAL(initSite(int,int)),mainWindow,SLOT(initSite(int,int)));
    connect(this,SIGNAL(initHabitant(int,int)),mainWindow,SLOT(initHabitant(int,int)));
    connect(this,SIGNAL(initCamion()),mainWindow,SLOT(initCamion()));

    connect(this,SIGNAL(setHabitantState(int,int)),mainWindow,SLOT(setHabitantState(int,int)));
    connect(this,SIGNAL(setSiteVelo(int,int)),mainWindow,SLOT(setSiteVelo(int,int)));
    connect(this,SIGNAL(startDeplacement(int,int,int,int)),mainWindow,SLOT(startDeplacement(int,int,int,int)));

    connect(this,SIGNAL(setDepotVelo(int)),mainWindow,SLOT(setDepotVelo(int)));
    connect(this,SIGNAL(setCamVelo(int)),mainWindow,SLOT(setCamVelo(int)));
    connect(this,SIGNAL(startCamionDeplacement(int,int,int)),mainWindow,SLOT(startCamionDeplacement(int,int,int)));
}

void* runHabitants(void* arguments)
{
    qsrand(QTime::currentTime().msec());
    //MAKE SOME CHANGE HERE
    struct Habitant* hab = (struct Habitant*)arguments;
    AlgoThread* algoThread = ((AlgoThread*)hab->algoThread);

    //L'habitant est placé a son site de départ
    emit algoThread->initHabitant(hab->idHab,hab->siteDepart);

    //Tous les habitants ne partent pas en même temps
    Sleep(hab->idHab * 100 + qrand()%2000);

    while(1)
    {
        //L'habitant souhaite se deplacer jusqu'à son site d'arrivée
        pthread_mutex_lock(&algoThread->listSite->at(hab->siteDepart)->mutex);

        //Mais il ne peut pas si il n'y a pas de vélo sur son site de départ
        if(algoThread->listSite->at(hab->siteDepart)->nbVeloSurSite == 0) //if(!(algoThread->listSite->at(hab->siteDepart)->nbVeloSurSite > 0))
        {
            //alors il attend que quelqu'un en dépose un
            emit algoThread->setHabitantState(hab->idHab,ParamList::WAIT);
            pthread_cond_wait(&algoThread->listSite->at(hab->siteDepart)->condVeloDispo,&algoThread->listSite->at(hab->siteDepart)->mutex);
        }

        //Ici l'habitant a obtenu un velo alors il peut partir -> le velo est retiré de la borne du site de départ
        algoThread->listSite->at(hab->siteDepart)->nbVeloSurSite--;
        emit algoThread->setSiteVelo(hab->siteDepart,algoThread->listSite->at(hab->siteDepart)->nbVeloSurSite);
        //Une fois que l'habitant est parti, on signale qu'une borne s'est libérée
        pthread_cond_signal(&algoThread->listSite->at(hab->siteDepart)->condBorneLibre);
        pthread_mutex_unlock(&algoThread->listSite->at(hab->siteDepart)->mutex);
        emit algoThread->setHabitantState(hab->idHab,ParamList::BIKE);
        emit algoThread->startDeplacement(hab->idHab,hab->siteDepart,hab->siteArrivee,hab->tempVoyage);

        //L'habitant voyage
        Sleep(hab->tempVoyage);

        //L'habitant arrive à son site d'arrivée
        pthread_mutex_lock(&algoThread->listSite->at(hab->siteArrivee)->mutex);
        //Il veut déposer son vélo mais il doit vérifier qu'une borne est disponible
        if(!(algoThread->listSite->at(hab->siteArrivee)->nbVeloSurSite < algoThread->nbBorne))
        {
            //Si il n'y a pas de borne disponible (nombre max de vélo sur ce site) il attend qu'une se libère
            pthread_cond_wait(&algoThread->listSite->at(hab->siteArrivee)->condBorneLibre,&algoThread->listSite->at(hab->siteArrivee)->mutex);
        }

        //Une borne s'est libérée, il peut y placer son vélo et commencer son action (=activité)
        algoThread->listSite->at(hab->siteArrivee)->nbVeloSurSite++;
        emit algoThread->setSiteVelo(hab->siteArrivee,algoThread->listSite->at(hab->siteArrivee)->nbVeloSurSite);
        //L'habitant signale qu'il a apporté un vélo sur le site d'arrivée (donc qu'un vélo est disponible pour quelqu'un qui veut partir)
        pthread_cond_signal(&algoThread->listSite->at(hab->siteArrivee)->condVeloDispo);
        pthread_mutex_unlock(&algoThread->listSite->at(hab->siteArrivee)->mutex);
        emit algoThread->setHabitantState(hab->idHab,ParamList::ACTION);

        //Mise à jour du site de départ (ancien site d'arrivée)
        hab->siteDepart = hab->siteArrivee;
        //Pour le site de d'arrivée, il ne doit pas être le même que le site de départ
        while(hab->siteArrivee == hab->siteDepart)
            hab->siteArrivee = (rand()+hab->idHab) % hab->algoThread->nbSite;

        //Changement des temps d'activité et de voyage
        hab->tempVoyage = qrand()%7000 + 3000;
        hab->tempsActivite = qrand()%7000 + 3000;

        //Fait son activité
        Sleep(hab->tempsActivite);

    }
}

void* runMaintenance(void* arguments)
{
    //MAKE SOME CHANGE HERE
    struct Maintenance* maint = (struct Maintenance*)arguments;
    AlgoThread* algoThread = (AlgoThread*)maint->algoThread;
    emit algoThread->setDepotVelo(maint->nbVeloDepot);
    emit algoThread->initCamion();

    while(1)
    {
        //Fais une pause
        Sleep(maint->tempsPause);
        //Rempli le camion si necessaire
        maint->nbVeloCamion = min(2,maint->nbVeloDepot);
        maint->nbVeloDepot -= maint->nbVeloCamion;
        emit algoThread->setDepotVelo(maint->nbVeloDepot);
        emit algoThread->setCamVelo(maint->nbVeloCamion);
        int B = algoThread->nbBorne;
        //L'equipe commence le tour des sites
        for(int i = 0; i<algoThread->nbSite;i++)
        {
            //L'equipe se déplace
            emit algoThread->startCamionDeplacement(i-1,i,maint->tempVoyage);

            //L'équipe se déplace
            Sleep(maint->tempVoyage);

            pthread_mutex_lock(&algoThread->listSite->at(i)->mutex);
            int Vi = algoThread->listSite->at(i)->nbVeloSurSite;
            //Si trop de velo sur le site, l'equipe en prends dans le camion
            if(Vi > B - 2)
            {
                int a = maint->nbVeloCamion;
                int c = min(Vi-(B-2),4-a);
                a = a + c;
                maint->nbVeloCamion = a;
                algoThread->listSite->at(i)->nbVeloSurSite = Vi - c;
                pthread_cond_signal(&algoThread->listSite->at(i)->condBorneLibre);

            }
            // si trop peu de vélo, l'equipe dépose des vélos sur le site
            else if(Vi<B-2)
            {
                int a = maint->nbVeloCamion;
                int c = min((B-2)-Vi,a);
                a = a - c;
                maint->nbVeloCamion = a;
                algoThread->listSite->at(i)->nbVeloSurSite = Vi  + c;
                pthread_cond_signal(&algoThread->listSite->at(i)->condVeloDispo);
            }
            emit algoThread->setCamVelo(maint->nbVeloCamion);
            emit algoThread->setSiteVelo(i,algoThread->listSite->at(i)->nbVeloSurSite);
            pthread_mutex_unlock(&algoThread->listSite->at(i)->mutex);
            //Le temps de voyage varie entre 2 sites
            maint->tempVoyage = qrand()%2000 + 3000;
        }
        //l'equipe retourne au depot, depose les vélos puis fait une pause
        emit algoThread->startCamionDeplacement(algoThread->nbSite-1,algoThread->nbSite,maint->tempVoyage);
        Sleep(maint->tempVoyage);
        maint->nbVeloDepot += maint->nbVeloCamion;
        maint->nbVeloCamion=0;
        emit algoThread->setCamVelo(maint->nbVeloCamion);
        emit algoThread->setDepotVelo(maint->nbVeloDepot);

        //Puis recommence la boucle
    }
}

int min(int a, int b) //Fonction minimum
{
    return !(b<a)?a:b;
}

void AlgoThread::run()
{
    //MAKE SOME CHANGE HERE

    qsrand(QTime::currentTime().msec());
    int veloPourDepot = this->nbVelo; // reste à mettre dans le déport

    //initialisation des sites
    for(int i = 0; i < this->nbSite; i++)
    {
        struct site * s = new site();
        s->idSite = i;
        s->nbVeloSurSite = nbBorne-2;
        s->mutex = PTHREAD_MUTEX_INITIALIZER;
        s->condVeloDispo = PTHREAD_COND_INITIALIZER;
        s->condBorneLibre = PTHREAD_COND_INITIALIZER;
        listSite->append(s);
        emit this->initSite(s->idSite,s->nbVeloSurSite);
        //Le nombre de vélo pour le dépot est réduit du nombre de vélo de chaque site
        veloPourDepot -= s->nbVeloSurSite;
    }

    //Initialisation des habitants
    pthread_t thread_hab[this->nbHabitants];
    for(int i = 0; i<this->nbHabitants;i++)
    {
        struct Habitant * hab = new Habitant();

        hab->algoThread = this;
        hab->idHab = i;
        hab->siteDepart = qrand() % this->nbSite;
        hab->siteArrivee = (rand()+hab->idHab) % hab->algoThread->nbSite;

        //Pour le site de d'arrivée, il ne doit pas être le même que le site de départ
        while(hab->siteArrivee == hab->siteDepart)
            hab->siteArrivee = (rand()+hab->idHab) % hab->algoThread->nbSite;


        hab->tempVoyage = qrand()%7000 + 3000;//Entre 3 et 10 secondes
        hab->tempsActivite = qrand()%7000 + 3000;//Entre 3 et 10 secondes
        emit hab->algoThread->setHabitantState(i,ParamList::ACTION);

        pthread_create (&thread_hab[i], NULL, runHabitants, (void*)hab);
    }

    //Initialisation du depot et de l'equipe de maintenance
    struct Maintenance* maintenance = new Maintenance();
    maintenance->algoThread = this;
    maintenance->nbVeloDepot = veloPourDepot;
    maintenance->tempVoyage = qrand()%2000 + 2000;

    pthread_t thread_maint;
    pthread_create(&thread_maint, NULL, runMaintenance, (void*)maintenance);

    //Le main attend la fin des autres threads pour terminer
    for(int i = 0; i<this->nbHabitants;i++)
    {
        pthread_join (thread_hab[i], NULL);
    }
    pthread_join (thread_maint, NULL);
}

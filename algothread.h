#ifndef ALGOTHREAD_H
#define ALGOTHREAD_H

#include <QThread>
#include <QDebug>
#include <windows.h>
#include <pthread.h>
#include "mainwindow.h"
#include "paramlist.h"

using namespace std;

//Struct contenant les infos sur un site
struct site{
    int idSite;
    int nbVeloSurSite;
    pthread_mutex_t mutex;
    pthread_cond_t condVeloDispo;
    pthread_cond_t condBorneLibre;
};

class AlgoThread : public QThread
{
    Q_OBJECT
public:
    AlgoThread(MainWindow* mainWindow,int nbSite,int nbHabitants,int nbBorne,int nbVelo);

    friend void* runHabitants(void*);
    friend void* runMaintenance(void*);

protected:
    void run();

signals:
    void initSite(int siteId,int nbVelo);
    void initHabitant(int habId,int initSiteId);
    void initCamion();

    void setHabitantState(int habId,int param);
    void setSiteVelo(int idSite,int nbVelo);
    void startDeplacement(int habId,int initSite,int destSite,int parcourTime);

    void setDepotVelo(int nbVeloDep);
    void setCamVelo(int nbVeloCam);
    void startCamionDeplacement(int initSite,int destSite,int parcourTime);

private:
    int nbSite;
    int nbHabitants;
    int nbBorne;
    int nbVelo;

    QList<site*>* listSite;

};

//Anciennement : initHab
struct Habitant {
    AlgoThread* algoThread;
    int idHab;
    int siteDepart;
    int siteArrivee;
    int tempVoyage;
    int tempsActivite;

};

//Anciennement : initDep
struct Maintenance {
    AlgoThread* algoThread;
    int nbVeloDepot;
    int nbVeloCamion;
    int siteDepart;
    int siteArrivee;
    int tempVoyage;
    const int tempsPause = 5000;
};



#endif // ALGOTHREAD_H



//   _____                          _     _____  _  _
//  /  ___|                        | |   /  __ \(_)| |
//  \ `--.  _ __ ___    __ _  _ __ | |_  | /  \/ _ | |_  _   _
//   `--. \| '_ ` _ \  / _` || '__|| __| | |    | || __|| | | |
//  /\__/ /| | | | | || (_| || |   | |_  | \__/\| || |_ | |_| |
//  \____/ |_| |_| |_| \__,_||_|    \__|  \____/|_| \__| \__, |
//                                                        __/ |
//                                                       |___/

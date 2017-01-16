/*
 * Projet :         Labo 2 du cours Programmation concurrente : Smart City
 * Enseignante :    A.Rizzotti
 * Auteurs :        Sylvain Renaud & Johnny Da Costa
 * Date :           22.01.2017
 *
 *   _____                          _     _____  _  _
 *  /  ___|                        | |   /  __ \(_)| |
 *  \ `--.  _ __ ___    __ _  _ __ | |_  | /  \/ _ | |_  _   _
 *   `--. \| '_ ` _ \  / _` || '__|| __| | |    | || __|| | | |
 *  /\__/ /| | | | | || (_| || |   | |_  | \__/\| || |_ | |_| |
 *  \____/ |_| |_| |_| \__,_||_|    \__|  \____/|_| \__| \__, |
 *                                                        __/ |
 *                                                       |___/
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>

#include <QMainWindow>
#include <QPropertyAnimation>

#include "GUI/people.h"
#include "GUI/place.h"
#include "GUI/camion.h"

#include "paramlist.h"

class QAction;
class QActionGroup;
class QLabel;
class QMenu;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int nbSite,int nbHabitants,int nbBorne,int nbVelo,QWidget *parent = 0);
    ~MainWindow();

private :
    void createMenus();
    void createActions();
    QMenu* aboutMenu;
    QAction *aboutAct;

private slots:
    void about();

public slots:
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
    Ui::MainWindow *ui;

    int nbSite;
    int nbHabitants;
    int nbBorne;
    int nbVelo;

    QList<Place*>* placeList;
    Place* depot;
    Camion* camion;
    QList<People*>* peopleList;

};

#endif // MAINWINDOW_H

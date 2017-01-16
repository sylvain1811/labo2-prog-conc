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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QActionGroup>
#include <QMessageBox>

MainWindow::MainWindow(int _nbSite,int _nbHabitants,int _nbBorne,int _nbVelo,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Labo 2 : Smart City");
    createActions();
    createMenus();

    nbSite = _nbSite;
    nbHabitants = _nbHabitants;
    nbBorne = _nbBorne;
    nbVelo = _nbVelo;
   // initialisation des places
    placeList = new QList<Place*>();
    Place* place;
    int radius = 200;
    for(int i=1;i<=nbSite;i++)
    {
        QPointF position = QPointF(120+radius+radius*cos(2.0*3.14/((float)nbSite) *((float)i+1)),
                10+radius+radius*sin(2.0*3.14/((float)nbSite) *((float)i+1)));
        place = new Place(ParamList::siteList[i-1],position,this);
        placeList->append(place);
        place->show();
    }

    //Initialisation des personnes
    peopleList = new QList<People*>();
    People* people;
    for(int i=1;i<=nbHabitants;i++)
    {
        people = new People(ParamList::nameList[i-1],this);
        peopleList->append(people);
        people->show();
    }

    //Initialisation du dépot, ne fait partie de la liste des endrois
    depot = new Place("DEPOT",QPointF(650,20),this);
    depot->show();

   //Initialisation de la camionette
    camion = new Camion(this);
    initCamion();
    camion->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMenus()
{
    aboutMenu = menuBar()->addMenu(tr("&About"));
    aboutMenu->addAction(aboutAct);
}

void MainWindow::createActions()
{
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setShortcut(tr("Ctrl+A"));
    connect(aboutAct, SIGNAL(triggered(bool)), this, SLOT(about()));
}

void MainWindow::about()
{
    /*QMessageBox::about(this, tr("A Propos"),
    tr("Développeur : \n Sylvain Renaud & Johnny Da Costa\n\n"
       "A propos du logiciel :\n"
       "Ce programme est issu du laboratoire 2 de programmation concurrente."
       ""));*/


    QMessageBox::about(this, tr("A Propos"),
    tr("Projet : Labo 2 du cours Programmation concurrente : Smart City\n"
       "Enseignante : A.Rizzotti\n"
       "Auteurs : Sylvain Renaud & Johnny Da Costa\n"
       "Date : 22.01.2017\n"
       ""));
}


// methodes d'initialisation Graphique
void MainWindow::initSite(int siteId,int nbVelo)
{
   // Mettre les velos sur les sites
    placeList->at(siteId)->setNbVelo(nbVelo);
}

void MainWindow::initHabitant(int habId,int initSiteId)
{
    //Mettre les habitants sur les sites
    peopleList->at(habId)->setPosition(placeList->at(initSiteId)->getDest());
}

void MainWindow::initCamion()
{
    // Mettre le camio au dépot
    camion->setPosition(depot->getDest());
}

void MainWindow::setHabitantState(int habId,int state)
{
    //Changer l'état du thread habitant
    peopleList->at(habId)->changeState(state);
}

void MainWindow::setSiteVelo(int idSite,int nbVelo)
{

    placeList->at(idSite)->setNbVelo(nbVelo);
}

//  indice habId du site initSite vers destSite en temps parcoutTime
void MainWindow::startDeplacement(int habId,int initSite,int destSite,int parcourTime)
{
    QPropertyAnimation *animation = new QPropertyAnimation(peopleList->at(habId),"geometry");
    animation->setDuration(parcourTime);
    QPointF A = placeList->at(initSite)->getDest();
    animation->setStartValue(QRect(A.x(),A.y(),100,100));
    QPointF B = placeList->at(destSite)->getDest();
    animation->setEndValue(QRect(B.x(),B.y(),100,100));
    animation->start();
}

void MainWindow::setDepotVelo(int nbVeloDep)
{
    //Mettre à jour le nombre de vélos du dépot
    depot->setNbVelo(nbVeloDep);
}

void MainWindow::setCamVelo(int nbVeloCam)
{
    //Mettre à jour le nombre de vélos dans le camion
    camion->setNbVelo(nbVeloCam);
}

void MainWindow::startCamionDeplacement(int initSite,int destSite,int parcourTime)
{
    QPropertyAnimation *animation = new QPropertyAnimation(camion,"geometry");
    animation->setDuration(parcourTime);

    QPointF A;
    if(initSite==-1)
        A = depot->getDest();
    else
        A = placeList->at(initSite)->getDest();

    QPointF B;
    if(destSite==placeList->size())
        B = depot->getDest();
    else
        B = placeList->at(destSite)->getDest();

    animation->setStartValue(QRect(A.x(),A.y(),100,100));
    animation->setEndValue(QRect(B.x(),B.y(),100,100));
    animation->start();
}

#include "mainwindow.h"
#include <QApplication>

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

#include "algothread.h"
#include <QInputDialog>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/img/logoHeArc.png"));

    //parametre d'entrée pour les sites
    QInputDialog* inputParam = new QInputDialog;
    inputParam->setInputMode(QInputDialog::IntInput);
    inputParam->setIntMinimum(2);
    inputParam->setIntMaximum(15);
    inputParam->setLabelText("Nombre de sites");
    inputParam->exec();
    int nbSite = inputParam->intValue();

    //parametre d'entrée pour les habitants
    inputParam->setIntMinimum(4);
    inputParam->setIntMaximum(15);
    inputParam->setLabelText("Nombre d'habitants");
    inputParam->exec();
    int nbHabitants=inputParam->intValue();

    //parametre d'entrée pour les bornes
    inputParam->setIntMinimum(4);
    inputParam->setLabelText("Nombre de borne par site");
    inputParam->exec();
    int nbBorne=inputParam->intValue();

    //parametre d'entrée pour les vélos
    inputParam->setIntMinimum(nbSite*(nbBorne-2)+3);
    inputParam->setIntMaximum(100);
    inputParam->setLabelText("Nombre de vélos");
    inputParam->exec();
    int nbVelo=inputParam->intValue();

    /*
    int nbSite=2;
    int nbHabitants=10;
    int nbBorne=4;
    int nbVelo=30;
    */

    MainWindow* mainWindow = new MainWindow(nbSite,nbHabitants,nbBorne,nbVelo);
    mainWindow->show();

    AlgoThread* algoThread = new AlgoThread(mainWindow,nbSite,nbHabitants,nbBorne,nbVelo);
    algoThread->start();

    return a.exec();
}

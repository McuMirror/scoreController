/*
 *
Copyright (C) 2016  Gabriele Salvato

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "gamedirector.h"
#include "choosediscipline.h"
#include "volleycontroller.h"
#include "basketcontroller.h"
#include "handballcontroller.h"


/*!
 * \brief GameDirector::GameDirector Shows the dialog for choosing the wanted Panel and starts it
 * \param argc Unused
 * \param argv Unused
 */
GameDirector::GameDirector(int &argc, char **argv)
    : QApplication(argc, argv)
    , pChooser(Q_NULLPTR)
    , pController(Q_NULLPTR)
{
#ifdef LOG_VERBOSE
    logMessage(Q_NULLPTR,
               Q_FUNC_INFO,
               QString(" Starting"));
#endif
    pChooser = new ChooseDiscipline();
    pChooser->setWindowFlags(Qt::Window);
}


/*!
 * \brief GameDirector::~GameDirector
 */
GameDirector::~GameDirector() {
    delete pChooser;
}


/*!
 * \brief GameDirector::exec
 * \return
 */
int
GameDirector::exec() {
    int iResult = 0;
    while(pChooser->exec() != QDialog::Rejected) {
        int iDiscipline = pChooser->getDiscipline();
        if(iDiscipline == VOLLEY_PANEL)
            pController = new VolleyController();
        else if(iDiscipline == BASKET_PANEL)
            pController = new BasketController();
        else if(iDiscipline == HANDBALL_PANEL)
            pController = new HandballController();
        else
            pController = new VolleyController();
        // To automagically delete the pController Object
        // after it has been closed
        pController->setAttribute(Qt::WA_DeleteOnClose);
#ifdef LOG_VERBOSE
    logMessage(Q_NULLPTR,
               Q_FUNC_INFO,
               QString(" Before showing the window"));
#endif
    #ifdef Q_OS_ANDROID
        pController->showFullScreen();
    #else
        pController->show();
    #endif
#ifdef LOG_VERBOSE
    logMessage(Q_NULLPTR,
               Q_FUNC_INFO,
               QString(" Window shown"));
#endif
        // Enters the main event loop and waits until exit() is called
        iResult = QApplication::exec();
        pController = Q_NULLPTR;
    }
    return iResult;
}

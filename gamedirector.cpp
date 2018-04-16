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


GameDirector::GameDirector(int &argc, char **argv)
    : QApplication(argc, argv)
    , pChooser(Q_NULLPTR)
{
}



GameDirector::~GameDirector() {
}


int
GameDirector::exec() {
    // pChooser is responsible to start the Control Panel or close the App
    pChooser = new ChooseDiscipline();
    pChooser->show();
    int iResult = QApplication::exec();
    return iResult;
}


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

#include "choosediscilpline.h"
#include <QApplication>

QApplication* pApp;

int
main(int argc, char *argv[]) {
    int iresult = 0;
    pApp = new QApplication(argc, argv);
    // Create a Dialog to choose the right panel
    ChooseDiscilpline *pChooser = new ChooseDiscilpline();
    // Show the dialog. It is responsible to start the control Panel
    // or close the App
    pChooser->show();
    // Start the event loop and waits until exit()
    iresult = pApp->exec();
    delete pChooser;
    delete pApp;
    return iresult;
}

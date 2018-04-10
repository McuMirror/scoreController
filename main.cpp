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

#include "choosediscipline.h"
#include <QApplication>

class MyApplication : public QApplication
{
public:
    MyApplication(int& argc, char ** argv);
    virtual ~MyApplication() { }
    int exec();

private:
    ChooseDiscipline *pChooser;
};


MyApplication::MyApplication(int& argc, char ** argv)
    : QApplication(argc, argv)
{
}


int
MyApplication::exec() {
    // pChooser is responsible to start the Control Panel or close the App
    pChooser = new ChooseDiscipline();
    pChooser->show();
    int iResult = QApplication::exec();
    return iResult;
}


int
main(int argc, char *argv[]) {
    int iresult = 0;
    MyApplication* pApp = new MyApplication(argc, argv);

    // Start the event loop and waits until exit() is called
    iresult = pApp->exec();
    return iresult;
}

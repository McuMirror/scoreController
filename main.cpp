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
#include "build_number.h"



/*!
 * \mainpage The Score Panels
 * \section intro Introduction
 *
 * This is the Server-Side of the "Score Panels System" aimed to replace
 * the very basics score displays adopted in many sport halls.
 *
 * You can have many clients running and they will be all handled by
 * this Server.
 *
 * It realize a ScorePanel Server that can be run, at present,
 * from Linux (tested on UBUNTU 18.04), Raspbian (tested on Raspberry
 * Pi3B with Raspbian) or Android.
 *
 * It does not require any configuration apart to guarantee that the
 * computer is connected to the same network of the "Score Panels".
 *
 *
 * \section install Installation
 * For Ubuntu or Raspberry you have to simply copy the executable(s)
 * from the following url:
 * <a href="https://github.com/salvato/ScorePanel_Executables"> link to the Apps</a>
 *
 * The program depends on some qt5 libraries that can be already installed and on some other that can be missing.
 * To check which libraries are missing you may issue the following command:
 *
 * <pre>ldd ./scoreController</pre>
 *
 * If you want to show Slides and/or Movies in your controlled Score Panels, you
 * have to create two directories in which place the files. At present only
 * jpeg, png and mp4 formats are allowed.
 *
 * \section Another Section
 * \subsection sub1 A subsection: How to
 *
 */
int
main(int argc, char *argv[]) {
    int iresult = 0;
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // Create the main application
    auto* pApp = new GameDirector(argc, argv);
    // Start the event loop and waits unil exit() is called
    iresult = pApp->exec();
    delete pApp;
    return iresult;
}

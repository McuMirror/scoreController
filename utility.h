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

#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

//#define LOG_MESG
//#define LOG_VERBOSE

#ifdef CUS_TS
#define WINDOW_ICON ":/android/CusTriesteH.png"
#endif
#ifdef CUS_UNIME
#define WINDOW_ICON ":/android/CusMessinaH.png"
#endif

#define VOLLEY_PANEL      0
#define FIRST_PANEL_TYPE  VOLLEY_PANEL
#define BASKET_PANEL      1
#define HANDBALL_PANEL    2
#define LAST_PANEL_TYPE   HANDBALL_PANEL

QString XML_Parse(const QString& input_string, const QString& token);
void logMessage(QFile *logFile, QString sFunctionName, QString sMessage);

#endif // UTILITY_H

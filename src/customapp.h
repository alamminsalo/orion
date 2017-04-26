/*
 * Copyright © 2015-2016 Antti Lamminsalo
 *
 * This file is part of Orion.
 *
 * Orion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with Orion.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CUSTOMAPP_H
#define CUSTOMAPP_H

#include <QApplication>

class CustomApp: public QApplication
{
    Q_OBJECT
public:
    CustomApp(int &argc, char **argv);
    virtual ~CustomApp() {}

public slots:
    bool event(QEvent *);
};

#endif // CUSTOMAPP_H

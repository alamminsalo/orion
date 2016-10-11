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

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDataStream>

namespace util{
	std::string notabs(std::string);
    QString readFile(const QString&);
    bool writeFile(const QString&, const QByteArray&);
    bool writeBinaryFile(const QString&, const QByteArray&);
	void writeImage(const char*,FILE*);
	bool fileExists(const char*);
//	bool folderExists(const char*);
}

#endif

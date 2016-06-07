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

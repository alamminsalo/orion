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

#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QString>
#include <QDir>
#include <QHash>
#include <QImage>
#include <QSet>
#include <QObject>
#include <QQuickImageProvider>
#include <QNetworkReply>

// Handles state for an individual download
class DownloadHandler : public QObject
{
    Q_OBJECT
public:
    DownloadHandler(const QString filename, const QString key);
private:
    const QString filename;
    const QString key;
    QFile _file;
    bool hadError;

signals:
    void downloadComplete(QString filename, bool hadError);

public slots:
    void dataAvailable();
    void replyFinished();
    void error(QNetworkReply::NetworkError code);
    const QString getKey() { return key; }
};

// provides QImages to QML frontend
class CachedImageProvider : public QQuickImageProvider {
public:
    CachedImageProvider(QHash<QString, QImage*> & imageTable);
    QImage requestImage(const QString &id, QSize * size, const QSize & requestedSize);
private:
    QHash<QString, QImage*> & imageTable;
};

// interface to QML
class ImageProvider : public QObject {
    Q_OBJECT
public:
    ImageProvider(const QString imageProviderName, const QString urlFormat, const QDir cacheDir, const QString extension);
    ~ImageProvider();

    QString getImageProviderName() { return _imageProviderName;  }
    QString getBaseUrl() { return "image://" + _imageProviderName; }
    bool makeAvailable(QString key);
    bool download(QString key);
    bool downloadsInProgress();

    QHash<QString, QImage*> imageTable();
    void loadImageFile(QString key, QString filename);

    QQmlImageProviderBase * getQMLImageProvider();
signals:
    void downloadComplete();

public slots:
    bool bulkDownload(QList<QString> keys);
    void individualDownloadComplete(QString filename, bool hadError);

private:
    QNetworkAccessManager _manager;

    QHash<QString, QImage*> _imageTable;
    QString _imageProviderName;
    QString _urlFormat;
    QDir _cacheDir;
    int activeDownloadCount;
    QString _extension;
    QSet<QString> currentlyDownloading;
};

#endif

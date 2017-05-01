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


#include <QString>
#include <QUrl>
#include <QtGlobal>
#include <QDebug>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QApplication>
#include <QDateTime>
#include "imageprovider.h"

ImageProvider::ImageProvider(const QString imageProviderName, const QString extension, const QString cacheDirName) : 
    _imageProviderName(imageProviderName), _extension(extension), _bulkDownloadStarting(false), _downloadCompletePending(false) {

    activeDownloadCount = 0;

    QString useCacheDirName = cacheDirName != "" ? cacheDirName : imageProviderName;
    _cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QString("/" + useCacheDirName);
}

ImageProvider::~ImageProvider() {
    qDeleteAll(_imageTable);
}

bool ImageProvider::makeAvailable(QString key) {
    key = getCanonicalKey(key);

    if (currentlyDownloading.contains(key)) {
        // download of this emote in progress
        return true;
    }
    else if (download(key)) {
        // if this emote isn't already downloading, it's safe to load the cache file or download if not in the cache
        currentlyDownloading.insert(key);
        activeDownloadCount += 1;
        return true;
    }
    else {
        // we already had the emote locally and don't need to wait for it to download
        return false;
    }
}

bool ImageProvider::download(QString key) {
    if (_imageTable.contains(key)) {
        //qDebug() << "already in the table";
        return false;
    }

    const QUrl url = getUrlForKey(key);
    _cacheDir.mkpath(".");

    QString filename = _cacheDir.absoluteFilePath(key + _extension);

    if (_cacheDir.exists(key + _extension)) {
        //qDebug() << "local file already exists";
        loadImageFile(key, filename);
        return false;
    }
    qDebug() << "downloading";

    QNetworkRequest request(url);
    QNetworkReply* _reply = nullptr;
    _reply = _manager.get(request);

    DownloadHandler * dh = new DownloadHandler(filename, key);

    connect(_reply, &QNetworkReply::readyRead,
        dh, &DownloadHandler::dataAvailable);
    connect(_reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
        dh, &DownloadHandler::error);
    connect(_reply, &QNetworkReply::finished,
        dh, &DownloadHandler::replyFinished);
    connect(dh, &DownloadHandler::downloadComplete,
        this, &ImageProvider::individualDownloadComplete);

    return true;
}

bool ImageProvider::bulkDownload(const QList<QString> & keys) {
    Q_ASSERT(!_bulkDownloadStarting);
    _bulkDownloadStarting = true;
    const int MSEC_PER_DOWNLOAD = 16; // ~ 256kbit/sec for 2k images
    bool waitForDownloadComplete = false;
    for (const auto & key : keys) {
        qint64 curTime = QDateTime::currentMSecsSinceEpoch();
        qint64 nextDownloadTime = curTime + MSEC_PER_DOWNLOAD;
        if (makeAvailable(key)) {
            waitForDownloadComplete = true;
            do {
                qApp->processEvents();
                curTime = QDateTime::currentMSecsSinceEpoch();
                if (curTime >= nextDownloadTime) break;
                if (curTime < nextDownloadTime - 100) {
                    // assume clock jump
                    break;
                }
            } while (true);
        }
        else {
            qApp->processEvents();
        }
    }
    _bulkDownloadStarting = false;
    if (_downloadCompletePending) {
        _downloadCompletePending = false;
        emit downloadComplete();
    }

    return waitForDownloadComplete && activeDownloadCount > 0;
}


void ImageProvider::individualDownloadComplete(QString filename, bool hadError) {
    DownloadHandler * dh = qobject_cast<DownloadHandler*>(sender());
    const QString emoteKey = dh->getKey();
    delete dh;

    if (hadError) {
        // delete partial download if any
        QFile(filename).remove();
    }
    else {
        loadImageFile(emoteKey, filename);
    }

    if (activeDownloadCount > 0) {
        activeDownloadCount--;
        qDebug() << activeDownloadCount << "active downloads remaining";
    }

    currentlyDownloading.remove(emoteKey);

    if (activeDownloadCount == 0) {
        if (_bulkDownloadStarting) {
            _downloadCompletePending = true;
        }
        else {
            emit downloadComplete();
        }
    }
}

QHash<QString, QImage*> ImageProvider::imageTable() {
    return _imageTable;
}

void ImageProvider::loadImageFile(QString emoteKey, QString filename) {
    QImage* emoteImg = new QImage();
    //qDebug() << "loading" << filename;
    emoteImg->load(filename);
    _imageTable.insert(emoteKey, emoteImg);
}

QQmlImageProviderBase * ImageProvider::getQMLImageProvider() {
    return new CachedImageProvider(_imageTable);
}

bool ImageProvider::downloadsInProgress() {
    return activeDownloadCount > 0;
}


URLFormatImageProvider::URLFormatImageProvider(const QString imageProviderName, const QString urlFormat, const QString extension, const QString cacheDir) :
    ImageProvider(imageProviderName, extension, cacheDir), _urlFormat(urlFormat)
{

}

const QUrl URLFormatImageProvider::getUrlForKey(QString & key) {
    return _urlFormat.arg(key);
}

QString ImageProvider::getCanonicalKey(const QString key) {
    return key;
}

// DownloadHandler

DownloadHandler::DownloadHandler(QString filename, QString key) : filename(filename), key(key), hadError(false) {
    _file.setFileName(filename);
    _file.open(QFile::WriteOnly);
    qDebug() << "starting download of" << filename;
}

void DownloadHandler::dataAvailable() {
    QNetworkReply* _reply = qobject_cast<QNetworkReply*>(sender());
    auto buffer = _reply->readAll();
    _file.write(buffer.data(), buffer.size());
}

void DownloadHandler::error(QNetworkReply::NetworkError /*code*/) {
    hadError = true;
    QNetworkReply* _reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "Network error downloading" << filename << ":" << _reply->errorString();
}

void DownloadHandler::replyFinished() {
    QNetworkReply* _reply = qobject_cast<QNetworkReply*>(sender());
    if (_reply) {
        _reply->deleteLater();
        _file.commit();
        //qDebug() << _file.fileName();
        //might need something for windows for the forwardslash..
        qDebug() << "download of" << _file.fileName() << "complete";

        emit downloadComplete(_file.fileName(), hadError);
    }
}


// CachedImageProvider
CachedImageProvider::CachedImageProvider(QHash<QString, QImage*> & imageTable) : QQuickImageProvider(QQuickImageProvider::Image), imageTable(imageTable) {

}

QImage CachedImageProvider::requestImage(const QString &id, QSize * size, const QSize & /*requestedSize*/) {
    // TODO figure out something sensible to do re requestedSize
    //qDebug() << "Requested id" << id << "from image provider";
    QImage * entry = NULL;
    auto result = imageTable.find(id);
    if (result != imageTable.end()) {
        entry = *result;
    }
    if (entry) {
        if (size) {
            *size = entry->size();
        }
        return *entry;
    }
    return QImage();
}

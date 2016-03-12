#ifndef VOD_H
#define VOD_H

#include <QString>
#include <QObject>

class Vod
{
    QString title;
    QString id;
    QString game;
    quint32 duration;
    quint64 views;
    QString preview;

public:
    Vod();
    Vod(Vod &other);
    ~Vod(){};

    QString getPreview() const;
    void setPreview(const QString &value);
    quint64 getViews() const;
    void setViews(const quint64 &value);
    quint32 getDuration() const;
    void setDuration(const quint32 &value);
    QString getGame() const;
    void setGame(const QString &value);
    QString getId() const;
    void setId(const QString &value);
    QString getTitle() const;
    void setTitle(const QString &value);
};

#endif // VOD_H

#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QString>

class Game : public QObject
{
    Q_OBJECT

    uint id;
    QString name;
    QString logo;
    QString preview;
    quint32 viewers;

public:
    Game();
    Game(const Game&);
    ~Game(){}

    QString getName() const;
    void setName(const QString &value);

    QString getLogo() const;
    void setLogo(const QString &value);

    quint32 getViewers() const;
    void setViewers(const quint32 &value);

    QString getPreview() const;
    void setPreview(const QString &value);

    uint getId() const;
    void setId(const uint &value);

signals:
    void updated();
};
Q_DECLARE_METATYPE(Game)

#endif // GAME_H

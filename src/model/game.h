#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QString>

class Game : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY updated)
    Q_PROPERTY(QString logo READ getLogo WRITE setLogo NOTIFY updated)
    Q_PROPERTY(QString preview READ getPreview WRITE setPreview NOTIFY updated)
    Q_PROPERTY(quint32 viewers READ getViewers WRITE setViewers NOTIFY updated)

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

signals:
    void updated();
};
Q_DECLARE_METATYPE(Game)

#endif // GAME_H

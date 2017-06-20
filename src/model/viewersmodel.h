#ifndef VIEWERSMODEL_H
#define VIEWERSMODEL_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include "../network/networkmanager.h"

class ViewersModel : public QObject
{
    Q_OBJECT

    NetworkManager *netman;
    static ViewersModel *instance;

    explicit ViewersModel(QObject *parent = nullptr);

public:
    static ViewersModel *getInstance() {
        if (!instance)
            instance = new ViewersModel();
        return instance;
    }

    static QObject *provider(QQmlEngine */*eng*/, QJSEngine */*jseng*/) {
        QQmlEngine::setObjectOwnership(getInstance(), QQmlEngine::CppOwnership);
        return getInstance();
    }

signals:
    void chatterListLoaded(QVariantMap chatters);

public slots:
    void loadChatterList(const QString channel);
    void processChatterList(QMap<QString, QList<QString>> chatters);

};

#endif // VIEWERSMODEL_H

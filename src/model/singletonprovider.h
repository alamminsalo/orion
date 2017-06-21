#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

///\brief
/// QML Singleton provider macro
/// Needs getInstance()-method defined when used.
/// Use on begin of class declaration
///

#define QML_SINGLETON \
    public: \
    static QObject *provider(QQmlEngine */*eng*/, QJSEngine */*jseng*/) {           \
        QQmlEngine::setObjectOwnership(getInstance(), QQmlEngine::CppOwnership);    \
        return getInstance();                                                       \
    }   \
    protected:

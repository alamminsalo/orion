#ifndef CUSTOMAPP_H
#define CUSTOMAPP_H

#include <QObject>
#include <QApplication>

class CustomApp: public QApplication
{
    Q_OBJECT
public:
    CustomApp(int &argc, char **argv);
    virtual ~CustomApp();

public slots:
    bool event(QEvent *);

signals:
    void windowClosed();
};

#endif // CUSTOMAPP_H

#ifndef CUSTOMAPP_H
#define CUSTOMAPP_H

#include <QApplication>

class CustomApp: public QApplication
{
public:
    CustomApp(int &argc, char **argv);

public slots:
    bool event(QEvent *);
};

#endif // CUSTOMAPP_H

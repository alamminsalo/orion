#include "customapp.h"
#include <QDebug>

CustomApp::CustomApp(int &argc, char **argv): QApplication(argc, argv)
{

}

CustomApp::~CustomApp()
{

}

bool CustomApp::event(QEvent *e)
{
    qDebug() << e->type();
    if (e->type() == QEvent::Type::Quit){
       emit windowClosed();
    } else {
        QApplication::event(e);
    }
}

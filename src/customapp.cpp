#include "customapp.h"
#include <QDebug>

CustomApp::CustomApp(int &argc, char **argv): QApplication(argc, argv)
{
}

bool CustomApp::event(QEvent *e)
{
    // qDebug() << "Window event: " << e->type();
    if (e->type() == QEvent::Type::Quit){
        //Do nothing
    } else {
       QApplication::event(e);
    }

    return true;
}

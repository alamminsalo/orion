#include "customapp.h"
#include <QDebug>

CustomApp::CustomApp(int &argc, char **argv): QApplication(argc, argv)
{

}

bool CustomApp::event(QEvent *e)
{
    qDebug() << e->type();
    if (e->type() == QEvent::Type::Quit){
        qDebug() << "Closing";
        //Do something
    } else {
        QApplication::event(e);
    }
}

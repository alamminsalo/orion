#include "customapp.h"
#include <QDebug>

CustomApp::CustomApp(int &argc, char **argv): QApplication(argc, argv)
{

}

bool CustomApp::event(QEvent *e)
{
    if (e->type() == QEvent::Type::Quit){
        //Do nothing
    } else {
        QApplication::event(e);
    }
}

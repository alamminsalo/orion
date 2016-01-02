import QtQuick 2.0
import QtQuick.Window 2.0
import "../styles.js" as Style

Window {
    width: 100
    height: 62
    flags: Qt.SplashScreen
    id: root

    function display(mX, mY){
//        root.x = mX

//        if (root.x + root.width > Screen.width)
//            root.x -= root.width

//        root.y = mY

        root.show()
    }
}

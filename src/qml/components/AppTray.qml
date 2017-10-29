import QtQuick 2.5
import QtQuick.Controls 2.1
import Qt.labs.platform 1.0
import app.orion 1.0

// Tray icon for orion app

SystemTrayIcon {
	visible: true
	iconSource: "qrc:/icon/orion.ico"

	property bool appVisible: rootWindow.visible

	menu: Menu {
		visible: false

		MenuItem {
			text: appVisible ? "Hide" : "Show"
			onTriggered: {
				if (appVisible)
				rootWindow.hide()
				else
				rootWindow.show()
			}
		}
		MenuItem {
			text: "Close"
			onTriggered: Qt.quit()
		}
	}
	Component.onCompleted: {
		if (!available)
		console.warn("SystemTrayIcon: Not available on current platform")

		else if (!supportsMessages)
		console.warn("SystemTrayIcon: No message support")

		else {
			console.info("SystemTrayIcon: OK")

			// Enable win notifications here
			if (Qt.platform.os == "windows") {
				ChannelManager.pushNotification.connect(function(title, message, imgUrl) {
					showMessage(title, message);
				});
			}
		}
	}
}

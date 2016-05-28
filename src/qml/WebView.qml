import QtQuick 2.0
import QtWebEngine 1.1
import "components"

Item {
    height: parent.height
    width: parent.width

    property bool requestInProgress: false

    function requestAccessToken() {
        if (!requestInProgress) {
            var clientId = netman.getClientId()
            var scope = "user_read%20user_subscriptions%20user_follows_edit"
            web.url = "https://api.twitch.tv/kraken/oauth2/authorize?response_type=token&client_id=" + clientId
                    + "&redirect_uri=http://localhost&scope=" + scope
            requestInProgress = true
        }
    }

    function logout() {
        g_cman.setAccessToken("")
        g_cman.checkFavourites()
        netman.clearCookies()
    }

    WebEngineView {
        id: web
        height: parent.height
        width: parent.width

        onLoadingChanged: {
            if (!loading && requestInProgress) {
                requestSelectionChange(7)
                requestInProgress = false
            }
        }

        onUrlChanged: {
            console.log(url)

            var urlStr = url.toLocaleString();

            var matchStr = "http://localhost/#access_token=";

            //Check if access token is received
            if (urlStr.indexOf(matchStr) === 0) {
                requestInProgress = false

                var access_token = urlStr.substring(matchStr.length, urlStr.indexOf("&scope="))

                console.log("Success! Access token is " + access_token)
                g_cman.setAccessToken(access_token)

                requestSelectionChange(6)
            }
        }
    }
}


/*
 * Copyright © 2015-2016 Antti Lamminsalo
 *
 * This file is part of Orion.
 *
 * Orion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with Orion.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtWebEngine 1.2
import QtQml 2.0
import "components"

Item {
    anchors.fill: parent

    property bool requestInProgress: false

    function requestAccessToken() {
        if (!requestInProgress) {
            var clientId = netman.getClientId()
            var scope = "user_read%20user_subscriptions%20user_follows_edit%20chat_login"
            web.url = "https://api.twitch.tv/kraken/oauth2/authorize?response_type=token&client_id=" + clientId
                    + "&redirect_uri=http://localhost&scope=" + scope
                    + "&force_verify=true"
            requestInProgress = true
        }
    }

    function getHttpAcceptLanguage() {
        var localeStr = Qt.locale().name;
        console.log("using locale", localeStr);
        var languageCountry = localeStr.replace("_", "-");
        var languageOnly = localeStr.split("_")[0];
        var out = languageCountry + "," + languageOnly;
        console.log("using httpAcceptLanguage setting", out);
        return out;
    }

    function logout() {
        g_cman.setAccessToken("")
        g_cman.checkFavourites()
        netman.clearCookies()
    }

    WebEngineView {
        id: web
        anchors.fill: parent

        profile: WebEngineProfile {
            httpAcceptLanguage: getHttpAcceptLanguage()
        }

        onLoadingChanged: {
            if (!loading) {
                if (requestInProgress) {
                    requestSelectionChange(7)
                    requestInProgress = false
                }
            }
        }

        onFullScreenRequested: {
            console.log("OK")
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


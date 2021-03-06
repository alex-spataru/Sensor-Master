/*
 * Copyright (c) 2018 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QtQml>
#include <QScreen>
#include <QQuickStyle>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "shareutils.h"
#include "AppInfo.h"
#include "QtAdMobBanner.h"

#ifdef Q_OS_ANDROID
#include <QColor>
#include <QtAndroid>
#endif

int main (int argc, char** argv) {
    // Set application options
    QGuiApplication::setApplicationName (APP_NAME);
    QGuiApplication::setOrganizationName (APP_DEVELOPER);
    QGuiApplication::setApplicationVersion (APP_VERSION);
    QGuiApplication::setApplicationDisplayName (APP_NAME);
    QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);

    // Init. application
    QGuiApplication app (argc, argv);
    qreal dpr = app.primaryScreen()->devicePixelRatio();

    // Set statusbar color on android
#ifdef Q_OS_ANDROID
    QtAndroid::runOnAndroidThread([=]() {
        QAndroidJniObject window = QtAndroid::androidActivity().callObjectMethod (
                    "getWindow", "()Landroid/view/Window;");
        window.callMethod<void>("addFlags", "(I)V", 0x80000000);
        window.callMethod<void>("clearFlags", "(I)V", 0x04000000);
        window.callMethod<void>("setStatusBarColor", "(I)V", QColor("#202126").rgba());
    });
#endif

    // Declare QML modules
    ShareUtils::DeclareQML();
    QmlAdMobBanner::DeclareQML();
    QQuickStyle::setStyle ("Material");

    // Init. QML interface
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty ("AppName", APP_NAME);
    engine.rootContext()->setContextProperty ("AppVersion", APP_VERSION);
    engine.rootContext()->setContextProperty ("AdsEnabled", ADS_ENABLED);
    engine.rootContext()->setContextProperty ("AdBannerId", ADS_BANNER_ID);
    engine.rootContext()->setContextProperty ("AppDeveloper", APP_DEVELOPER);
    engine.rootContext()->setContextProperty ("DevicePixelRatio", dpr);
    engine.load (QUrl (QStringLiteral ("qrc:/qml/main.qml")));

    // QML error, quit application
    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    // Enter QApplication event loop
    return app.exec();
}


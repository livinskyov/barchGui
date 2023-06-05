#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "imagelistmodel.h"
#include "watcheddirectory.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCommandLineOption custom_directory_option({"p", "path-to-directory"},
                                               QCoreApplication::translate("main", "Open the directory at the specified path."),
                                               QCoreApplication::translate("main", "path to directory"));
    QCommandLineParser parser;
    parser.addOption(custom_directory_option);
    parser.process(app);

    QString custom_directory = parser.value(custom_directory_option);
    if (!custom_directory.isEmpty()) {
        WatchedDirectory::get().watch(custom_directory);
    }

    qmlRegisterType<ImageListModel>("ImageListModel", 1, 0, "ImageListModel");

    QQmlApplicationEngine engine;
    QObject::connect(&engine,
                     &QQmlApplicationEngine::objectCreationFailed,
                     &app,
                     []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    engine.load(u"qrc:/BarchGui/ImageList.qml"_qs);
    engine.load(u"qrc:/BarchGui/Main.qml"_qs);

    return app.exec();
}

#ifndef WATCHEDDIRECTORY_H
#define WATCHEDDIRECTORY_H

#include <QFileSystemWatcher>

class WatchedDirectory
{
    WatchedDirectory();
public:

    static WatchedDirectory& get();

    QString path() const;
    void watch(QString path);

    template <class Subscriber>
    QMetaObject::Connection subscribe(Subscriber* subscriber, void (Subscriber::* on_directory_changed)(QString /*path*/))
    {
        return QObject::connect(&watcher, &QFileSystemWatcher::directoryChanged, subscriber, on_directory_changed);
    }

private:
    QFileSystemWatcher watcher;
    QString directory_path = "./";
};

#endif // WATCHEDDIRECTORY_H

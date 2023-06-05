#include "watcheddirectory.h"

WatchedDirectory::WatchedDirectory()
{
    watcher.addPath(directory_path);
}

WatchedDirectory& WatchedDirectory::get()
{
    static WatchedDirectory instance;
    return instance;
}

QString WatchedDirectory::path() const
{
    return directory_path;
}

void WatchedDirectory::watch(QString path)
{
    if (!directory_path.isEmpty()) {
        watcher.removePath(directory_path);
    }
    directory_path = path;
    watcher.addPath(path);
}

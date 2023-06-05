#include "imagelistmodel.h"
#include "threading.h"
#include "watcheddirectory.h"
#include <QDir>
#include <QDirIterator>
#include <algorithm>

using namespace Qt::StringLiterals;

static constexpr auto bmp   = "bmp"_L1;
static constexpr auto png   = "png"_L1;
static constexpr auto barch = "barch"_L1;

ImageListModel::ImageListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    WatchedDirectory::get().subscribe(this, &ImageListModel::browseDirectory);
    browseDirectory(WatchedDirectory::get().path());
}

void ImageListModel::browseDirectory(QString path)
{
    QDir dir(path);
    QStringList filters;

    if (showBMP) {
        filters.append("*.bmp");
    }
    if (showPNG) {
        filters.append("*.png");
    }
    if (showBarch) {
        filters.append("*.barch");
    }

    emit layoutAboutToBeChanged();
    if (!filters.empty()) {
        dir.setNameFilters(filters);
        items = dir.entryInfoList();
    } else {
        items.clear();
    }
    emit layoutChanged();

    filters.clear();
    if (!showBMP) {
        filters.append("*.bmp");
    }
    if (!showPNG) {
        filters.append("*.png");
    }
    if (!showBarch) {
        filters.append("*.barch");
    }

    if (!filters.empty()) {
        dir.setNameFilters(filters);
        hiddenItems = dir.entryInfoList();
    } else {
        hiddenItems.clear();
    }
}

QVariant ImageListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (section) {
    case 0:
        return u"Зображення"_s;
    case 1:
        return u"Розмір (Кб)"_s;
    }

    return u""_s;
}

bool ImageListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return false;
}

int ImageListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return items.size();
}

bool ImageListModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}

bool ImageListModel::canFetchMore(const QModelIndex &parent) const
{
    return false;
}

void ImageListModel::fetchMore(const QModelIndex &parent)
{
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case FileName:
        {
            const QString file_name = items.at(index.row()).fileName();
            if (compressing.contains(file_name)) {
                return file_name + u" (Кодується)"_s;
            }
            if (uncompressing.contains(file_name)) {
                return file_name + u" (Розкодовується)"_s;
            }
            return file_name;
        }
    case FileSize:
        {
            const auto sizeKb = items.at(index.row()).size() / 1024.0;
            return size_t(sizeKb * 100) / 100.0; // 2 digits precision
        }
    }

    return QVariant();
}

void ImageListModel::hide(QString suffix)
{
    for (std::size_t i = items.size(); i--;) {
        if (items[i].suffix().compare(suffix, Qt::CaseInsensitive) == 0) {
            hiddenItems.append(items.takeAt(i));
            continue;
        }
    }
}

void ImageListModel::show(QString suffix)
{
    for (std::size_t i = hiddenItems.size(); i--;) {
        if (hiddenItems[i].suffix().compare(suffix, Qt::CaseInsensitive) == 0) {
            items.append(hiddenItems.takeAt(i));
            continue;
        }
    }
    std::sort(items.begin(), items.end(), [](const QFileInfo& first, const QFileInfo& second) {
        return first.fileName() < second.fileName();
    });
}

template<const QLatin1StringView& suffix>
void ImageListModel::toggle()
{
    if ((showBMP = !showBMP)) {
        const auto oldSize = items.size();
        emit layoutAboutToBeChanged();
        show(suffix);
        emit layoutChanged();
    } else {
        const auto oldSize = items.size();
        emit layoutAboutToBeChanged();
        hide(suffix);
        emit layoutChanged();
    }
}

void ImageListModel::toggleBMP()
{
    toggle<bmp>();
}

void ImageListModel::togglePNG()
{
    toggle<png>();
}

void ImageListModel::toggleBarch()
{
    toggle<barch>();
}

bool ImageListModel::processImage(int row)
{
    const auto file_info = items.at(row);
    if (compressing.contains(file_info.fileName()) || uncompressing.contains(file_info.fileName())) {
        return false;
    }
    const auto suffix = file_info.suffix();
    if (suffix.compare(bmp, Qt::CaseInsensitive) == 0 || suffix.compare(png, Qt::CaseInsensitive) == 0) {
        auto *& thread = compressing[file_info.fileName()];
        if (!thread) {
            thread = new CompressingThread(file_info);
            connect(static_cast<CompressingThread*>(thread), &CompressingThread::compressed, this, &ImageListModel::onCompressed);
            connect(thread, &QThread::finished, thread, &QObject::deleteLater);
            thread->start();

            emit dataChanged(this->index(row, 0), this->index(row, 0));
            return true;
        }
    } else if (suffix.compare(barch, Qt::CaseInsensitive) == 0) {
        auto *& thread = uncompressing[file_info.fileName()];
        if (!thread) {
            thread = new UncompressingThread(file_info);
            connect(static_cast<UncompressingThread*>(thread), &UncompressingThread::uncompressed, this, &ImageListModel::onUncompressed);
            connect(thread, &QThread::finished, thread, &QObject::deleteLater);
            thread->start();

            emit dataChanged(this->index(row, 0), this->index(row, 0));
            return true;
        }
    }
    return false;
}

void ImageListModel::onCompressed(QFileInfo image, bool result)
{
    compressing.remove(image.fileName());
    onProcessed(image, result);
}

void ImageListModel::onUncompressed(QFileInfo image, bool result)
{
    uncompressing.remove(image.fileName());
    onProcessed(image, result);
}

void ImageListModel::onProcessed(QFileInfo image, bool result) {
    const auto i = items.indexOf(image);
    emit dataChanged(index(i, 0), index(i, 0));

    if (!result) {
        emit processingError(image.fileName(), i);
    }
}

bool ImageListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags ImageListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool ImageListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // beginInsertRows(parent, row, row + count - 1);
    // endInsertRows();
    return false;
}

QHash<int, QByteArray> ImageListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[FileName] = "name";
    roles[FileSize] = "size";
    return roles;
}

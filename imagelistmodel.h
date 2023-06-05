#ifndef IMAGELISTMODEL_H
#define IMAGELISTMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>
#include <QMap>
#include <QThread>

class ImageListModel : public QAbstractListModel
{
    Q_OBJECT

    enum Role {
        FileName = Qt::UserRole,
        FileSize
    };

    void browseDirectory(QString path);
    void hide(QString suffix);
    void show(QString suffix);

    template<const QLatin1StringView& suffix>
    void toggle();

    void onCompressed(QFileInfo image, bool result);
    void onUncompressed(QFileInfo image, bool result);
    void onProcessed(QFileInfo image, bool result);

signals:
    void processingError(QString file_name, int row);

public:
    explicit ImageListModel(QObject *parent = nullptr);

    // Header:
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::UserRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = FileName) const override;

    // Editable:
    Q_INVOKABLE void toggleBMP();
    Q_INVOKABLE void togglePNG();
    Q_INVOKABLE void toggleBarch();

    Q_INVOKABLE bool processImage(int row);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QHash<int, QByteArray> roleNames() const override;

private:
    QFileInfoList items;
    QFileInfoList hiddenItems;

    QMap<QString, QThread*> compressing;
    QMap<QString, QThread*> uncompressing;

    bool showBMP = true;
    bool showPNG = true;
    bool showBarch = true;
};

#endif // IMAGELISTMODEL_H

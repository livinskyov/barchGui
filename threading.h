#ifndef THREADING_H
#define THREADING_H

#include <QFileInfo>
#include <QThread>

class CompressingThread : public QThread
{
    Q_OBJECT

public:
    CompressingThread(QFileInfo image);

    void run() override;

signals:
    void compressed(QFileInfo image, bool success);

private:
    QFileInfo image;
};

class UncompressingThread : public QThread
{
    Q_OBJECT

public:
    UncompressingThread(QFileInfo image);

    void run() override;

signals:
    void uncompressed(QFileInfo image, bool success);

private:
    QFileInfo image;
};

#endif // THREADING_H

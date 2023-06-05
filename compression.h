#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <QImage>

struct Compression
{
    bool compress(QString image_name, QString format) const;
    bool uncompress(QString image_name) const;

    QString path;
};

#endif // COMPRESSION_H

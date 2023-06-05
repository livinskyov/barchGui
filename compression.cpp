#include "compression.h"
#include <barch.h>

bool Compression::compress(QString image_name, QString format) const
{
    try
    {
        const auto full_file_name = path + '/' + image_name + '.' + format;
        QImage image(full_file_name);
        if (image.format() == QImage::Format_Invalid) {
            return false;
        }

        auto pixels = std::make_unique<unsigned char[]>(image.width() * image.height());
        for (int j = 0; j < image.height(); ++j)
        {
            for (int i = 0; i < image.width(); ++i)
            {
                const QColor color = image.pixelColor(i, j);
                pixels[image.width() * j + i] = color.lightness();
            }
        }

        Barch::RawImageData raw_data{image.width(), image.height(), pixels.get()};
        const auto compressed = Barch::compress(raw_data);

        const auto packed_image_name = path + '/' + image_name + " packed";
        return Barch::saveToDisk(compressed.data(), packed_image_name.toStdString());
    }
    catch(...)
    {
        return false;
    }
}

bool Compression::uncompress(QString image_name) const
{
    try
    {
        const auto full_image_name = path + '/' + image_name;

        const auto compressed = Barch::loadFromDisk(full_image_name.toStdString());
        const auto uncompressed = Barch::uncompress(compressed.data());

        if (!uncompressed.raw_data)
        {
            return false;
        }

        QImage unpacked(uncompressed.data().width, uncompressed.data().height, QImage::Format_Grayscale8);
        for (int j = 0; j < uncompressed.height; ++j)
        {
            for (int i = 0; i < uncompressed.width; ++i)
            {
                const auto grayscale = uncompressed.data().data[uncompressed.width * j + i];
                unpacked.setPixelColor(i, j, QColor(grayscale, grayscale, grayscale));
            }
        }

        const auto unpacked_file_name = path + '/' + image_name + " unpacked.bmp";
        return unpacked.save(unpacked_file_name, "BMP");
    }
    catch(...)
    {
        return false;
    }
}

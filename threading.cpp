#include "threading.h"
#include "compression.h"

CompressingThread::CompressingThread(QFileInfo image)
    : image(image)
{
}

void CompressingThread::run()
{
    Compression processing{image.path()};
    const bool success = processing.compress(image.completeBaseName(), image.suffix());
    emit compressed(image, success);
}

UncompressingThread::UncompressingThread(QFileInfo image)
    : image(image)
{
}

void UncompressingThread::run()
{
    Compression processing{image.path()};
    const bool success = processing.uncompress(image.completeBaseName());
    emit uncompressed(image, success);
}

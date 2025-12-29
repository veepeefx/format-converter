#ifndef FORMAT_CONVERTER_CONVERTER_H
#define FORMAT_CONVERTER_CONVERTER_H
#include <QString>

class Converter {
public:
    static bool runConverter(const QString& inputFilePath, const QString& outputFilePath);
    static bool runMetaDataRemover(const QString& inputFilePath, const QString& outputFilePath);

private:

    static bool runFFmpeg(QStringList args);
    Converter() = delete;
    ~Converter() = delete;
};


#endif //FORMAT_CONVERTER_CONVERTER_H
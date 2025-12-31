#ifndef FORMAT_CONVERTER_CONVERTER_H
#define FORMAT_CONVERTER_CONVERTER_H
#include <QString>

#include "CommonEnums.h"

class Converter {
public:
    static bool runConverter(const QString& inputFilePath,
                             const QString& outputFilePath,
                             const FormatInfo& inputFormat);

    static bool runMetaDataRemover(const QString& inputFilePath,
                                   const QString& outputFilePath,
                                   const FormatInfo& inputFormat);

private:

    static bool runFFmpeg(QStringList args);
    static void updateVideoArgs(QStringList& args, const int& enumValue);
    static void updateAudioArgs(QStringList& args, const int& enumValue);
    static void updateImageArgs(QStringList& args, const int& enumValue);

    Converter() = delete;
    ~Converter() = delete;
};


#endif //FORMAT_CONVERTER_CONVERTER_H
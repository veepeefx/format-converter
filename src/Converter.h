#ifndef FORMAT_CONVERTER_CONVERTER_H
#define FORMAT_CONVERTER_CONVERTER_H
#include <QString>

#include "CommonEnums.h"

class Converter {
public:

    static bool runConverter(const QString& inputFilePath,
                             const QString& outputFilePath,
                             const FormatInfo& inputFormat,
                             std::function<void(int)> progressCallBack);

    static bool runMetaDataRemover(const QString& inputFilePath,
                                   const QString& outputFilePath,
                                   const FormatInfo& inputFormat,
                                   std::function<void(int)> progressCallBack);

private:

    static bool runFFmpeg(QStringList args, std::function<void(int)> progressCallBack);
    static void updateVideoArgs(QStringList& args, const int& enumValue);
    static void updateAudioArgs(QStringList& args, const int& enumValue);
    static void updateImageArgs(QStringList& args, const int& enumValue);

    Converter() = delete;
    ~Converter() = delete;
};


#endif //FORMAT_CONVERTER_CONVERTER_H
# WORK IN PROGRESS!
# Format Converter 
## (C++ / Qt / FFmpeg)

## Description
Format Converter is a cross-platform media conversion tool written in C++ (Qt 6) and powered by
FFmpeg. It allows you to convert audio, video, and image files between supported formats and
optionally remove all metadata from your media files. **This tool attempts to convert files and
remove metadata where possible. It does NOT guarantee perfect results. Use at your own risk.
Make backups! READ FULL DISCLAIMER!**

## Features
- Change file format of your music, videos or images
- Remove unwanted metadata from your media
- Simple and clean Qt-based user interface

## Supported Formats
- Audio: mp3, wav, aac, flac, ogg, wma, m4a, aiff
- Video: mp4, avi, mkv, mov, wmv, flv, webm, mpeg, m4v
- Image: jpeg, png, gif, bmp, tiff, heif, webp, ico 

Format conversions seems to be fully working, excluding heif image format.  
Metadata copying and removal still has issues with some formats.

## Disclaimer

This tool attempts to convert files and remove metadata where possible.  
**It does NOT guarantee** that all metadata will be removed or that the output files will function
perfectly in all situations.

- Some formats may retain certain metadata or technical data that cannot safely be removed.
- Using this tool may result in corrupted files, loss of data, or unintended changes to the
original content.
- Opting out of “Preserve metadata” does not remove metadata from the generated file and metadata
  embedded in the source file may still remain in the output file. Disabling it simply tells the
  application not to explicitly copy metadata during processing.

**Use at your own risk.** The user is responsible for backups and any consequences of file modifications.

## Installation / Build
1. Clone this repo
   ```
   git clone https://github.com/veepeefx/format-converter
   cd format-converter
   ```

2. Create a build directory and run CMake
    ```
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```

3. Run the executable
    ```
    ./format-converter
    ```

## Dependencies

### Required
- [FFmpeg](https://ffmpeg.org/download.html) - media conversion (must be installed and available in PATH)
- C++ 20 compatible compiler
- CMake 4.0 ->
- Qt6 with the following required modules:
    - Core
    - Gui
    - Widgets

### Optional
- [ExifTool](https://exiftool.org/install.html) - metadata removal and preservation (must be
installed and available in PATH)

## License
Licensed under MIT License.

MESSAGE(STATUS "Using bundled FindTurboJpeg.cmake...")

FIND_PATH(LIB_JPEG_INCLUDE_DIR
        jpeglib.h turbojpeg.h
        "/usr/include/"
        "/usr/local/include/")

FIND_LIBRARY(LIB_JPEG_LIBRARIES
        NAMES libjpeg
        PATHS "/usr/lib/" "/usr/local/lib/")
cmake_minimum_required(VERSION 2.8)

project(ffmpeg-java)

#add_definitions(-std=c11)
set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

file(GLOB SOURCE_FILES src/main/c/*.c src/main/c/*.h)

if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND UNIX)
    message(STATUS "current platform: Linux")
    set(LINUX 1)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows" OR WIN32 OR CMAKE_HOST_WIN32)
    message(STATUS "current platform: Windows")
    set(WINDOWS 1)
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD" AND UNIX)
    message(STATUS "current platform: FreeBSD")
    set(FREEBSD 1)
elseif(CMAKE_SYSTEM_NAME MATCHES "MINGW" AND UNIX)
    message(STATUS "current platform: MINGW")
    set(MINGW 1)
elseif(APPLE AND NOT UNIX AND CMAKE_HOST_APPLE)
    message(STATUS "current platform: MacOS or iOS or watchOS")
elseif(UNIX)
    message(STATUS "current platform: Unix")
else()
    message(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
endif()

message(STATUS "CMAKE_CL_64: " "${CMAKE_CL_64}")
message(STATUS "CMAKE_CL_86: " "${CMAKE_CL_86}")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
if(WINDOWS)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".dll")
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".so")
endif()

# jni
if(WINDOWS)
	if(CMAKE_CL_64)
		# win64
		include_directories("${PROJECT_SOURCE_DIR}/pkg/x64-windows/jni/include")
		include_directories("${PROJECT_SOURCE_DIR}/pkg/x64-windows/jni/include/win32")
		target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/jni/lib/jawt.lib")
        target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/jni/lib/jvm.lib")

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${PROJECT_SOURCE_DIR}/pkg/x64-windows/jni/bin"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>)

		else()
		# win32
	endif(CMAKE_CL_64)
else()
	
endif()

# libjpeg
if(WINDOWS)
	if(CMAKE_CL_64)
		# win64
		include_directories("${PROJECT_SOURCE_DIR}/pkg/x64-windows/turbojpeg/include")
		target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/turbojpeg/lib/jpeg.lib")
        target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/turbojpeg/lib/turbojpeg.lib")

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${PROJECT_SOURCE_DIR}/pkg/x64-windows/turbojpeg/bin"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>)

		else()
		# win32
	endif(CMAKE_CL_64)
else()
	
endif()

# ffmpeg
if(WINDOWS)
	if(CMAKE_CL_64)
		# win64
		include_directories("${PROJECT_SOURCE_DIR}/pkg/x64-windows/ffmpeg/include")

        target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/ffmpeg/lib/avcodec.lib")
        target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/ffmpeg/lib/avdevice.lib")
        target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/ffmpeg/lib/avfilter.lib")
        target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/ffmpeg/lib/avformat.lib")
        target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/ffmpeg/lib/avutil.lib")
        target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/ffmpeg/lib/swresample.lib")
        target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/pkg/x64-windows/ffmpeg/lib/swscale.lib")

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${PROJECT_SOURCE_DIR}/pkg/x64-windows/ffmpeg/bin"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>)

		else()
		# win32
	endif(CMAKE_CL_64)
else()
	find_path(FFMPEG_AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
	find_library(FFMPEG_LIBAVCODEC_LIBRARY avcodec)
	target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_AVCODEC_INCLUDE_DIR})
	target_link_libraries( ${PROJECT_NAME} PRIVATE ${FFMPEG_LIBAVCODEC_LIBRARY})

	find_path(FFMPEG_AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
	find_library(FFMPEG_AVFORMAT_LIBRARY avformat)
	target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_AVFORMAT_INCLUDE_DIR})
	target_link_libraries( ${PROJECT_NAME} PRIVATE ${FFMPEG_AVFORMAT_LIBRARY})

	find_path(FFMPEG_AVUTIL_INCLUDE_DIR libavutil/imgutils.h)
	find_library(FFMPEG_AVUTIL_LIBRARY avformat)
	target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_AVUTIL_INCLUDE_DIR})
	target_link_libraries( ${PROJECT_NAME} PRIVATE ${FFMPEG_AVUTIL_LIBRARY})

	find_path(FFMPEG_AVDEVICE_INCLUDE_DIR libavcodec/avcodec.h)
	find_library(FFMPEG_LIBAVDEVICE_LIBRARY avformat)
	target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_AVDEVICE_INCLUDE_DIR})
	target_link_libraries( ${PROJECT_NAME} PRIVATE ${FFMPEG_LIBAVDEVICE_LIBRARY})

	find_path(FFMPEG_AVFILTER_INCLUDE_DIR libavfilter/avfilter.h)
	find_library(FFMPEG_LIBAVFILTER_LIBRARY avformat)
	target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_AVFILTER_INCLUDE_DIR})
	target_link_libraries( ${PROJECT_NAME} PRIVATE ${FFMPEG_LIBAVFILTER_LIBRARY})

	find_path(FFMPEG_LIBSWSCALE_INCLUDE_DIR libswscale/swscale.h)
	find_library(FFMPEG_LIBSWSCALE_LIBRARY avformat)
	target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBSWSCALE_INCLUDE_DIR})
	target_link_libraries( ${PROJECT_NAME} PRIVATE ${FFMPEG_LIBSWSCALE_LIBRARY})
endif()

cmake_minimum_required(VERSION 3.4.1)
add_library( # Sets the name of the library.
        native-lib
        # Sets the library as a shared library.
        SHARED
        # graphics
        GLUtils.h
        GLUtils.cpp
        Matrix.h
        Matrix.cpp
        native_lib.h
        native-lib.cpp)
find_library( # Sets the name of the path variable.
        log-lib
        log)
target_link_libraries(native-lib
                      EGL
                      GLESv2
                      android
                      ${log-lib})
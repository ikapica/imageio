### How to compile?

Local installation:

    > mkdir build
    > cd build
    > cmake -DCMAKE_INSTALL_PREFIX=../installed ..
    > make install

Unintall library:

    > make uninstall

### How to use it?

Once the library is installed, CMake will be able to find it using
`find_package()` command. For example:

    cmake_minimum_required(VERSION 2.6)
    project(Bar)

    find_package(Imageio REQUIRED)
    include_directories(${IMAGEIO_INCLUDE_DIRS})

    add_executable(bar bar.cpp)
    target_link_libraries(bar ${IMAGEIO_LIBRARIES})
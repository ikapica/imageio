#
# Copyright Pablo Speciale https://github.com/pablospe/cmake-example-library
# Modified by Ireneusz Kapica.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

# (uncomment these lines for an external project)
#
# cmake_minimum_required(VERSION 3.7)
# project(Bar)
# find_package(Imageio REQUIRED)
# include_directories(${IMAGEIO_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)

add_executable(example main.cpp)

target_link_libraries(example ${LIBRARY_NAME})

#install(TARGETS example
#        # In order to export target, uncomment next line
#        #   EXPORT ${PROJECT_EXPORT}
#        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)

cmake_minimum_required(VERSION 3.8)
project(simple_downloader)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)

link_directories(/usr/local/opt/curl/lib)

add_executable(simple_downloader ${SOURCE_FILES})
target_link_libraries(simple_downloader curl)

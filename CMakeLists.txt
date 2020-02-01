cmake_minimum_required(VERSION 3.15.2)
add_definitions("-Wall")
set(CMAKE_CXX_STANDARD 11)

project(sepd VERSION 0.0.1 DESCRIPTION "sequential event pattern detector")
add_library(sepd INTERFACE)

include_directories("${CMAKE_SOURCE_DIR}/include")

target_include_directories(sepd INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

include(GNUInstallDirs)
install(TARGETS sepd
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Add sub-directories
add_subdirectory(example)
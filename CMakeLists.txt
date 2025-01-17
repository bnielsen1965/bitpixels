cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
project(bitpixels VERSION 0.0.1 DESCRIPTION "Pixel and image library for monochrome displays, i.e. tiny OLEDs.")

#include(GNUInstallDirs)

add_library(bitpixels STATIC src/bitpixels.c)
set_target_properties(bitpixels PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/bitpixels.h)


#configure_file(bitpixels.pc.in bitpixels.pc @ONLY)

target_include_directories(bitpixels PUBLIC include)

#install(TARGETS bitpixels
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#install(FILES ${CMAKE_BINARY_DIR}/bitpixels.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

add_executable(test src/test.c)
target_include_directories(test PUBLIC include)
target_link_libraries(test bitpixels)

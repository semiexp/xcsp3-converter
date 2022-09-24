cmake_minimum_required(VERSION 3.1)
project(xcsp3_converter CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(ExternalProject)

set(XCSP3_CPP_Parser_SOURCE_DIR ${CMAKE_BINARY_DIR}/XCSP3-CPP-Parser)
set(XCSP3_CPP_Parser_BINARY_DIR ${CMAKE_BINARY_DIR}/XCSP3-CPP-Parser_bin)
ExternalProject_Add(XCSP3_CPP_Parser_project
    GIT_REPOSITORY https://github.com/xcsp3team/XCSP3-CPP-Parser
    GIT_TAG 9ce13b5fa8d09bd3b5e9b961a31815efa4f79a49
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    INSTALL_COMMAND ""
    SOURCE_DIR ${XCSP3_CPP_Parser_SOURCE_DIR}
    BINARY_DIR ${XCSP3_CPP_Parser_BINARY_DIR}
)
add_library(XCSP3_CPP_Parser_lib STATIC IMPORTED)
add_dependencies(XCSP3_CPP_Parser_lib XCSP3_CPP_Parser_project)
set_property(TARGET XCSP3_CPP_Parser_lib PROPERTY IMPORTED_LOCATION ${XCSP3_CPP_Parser_BINARY_DIR}/libxcsp3parser.a)

find_package(LibXml2 REQUIRED)

add_executable(xcsp3_converter src/main.cc src/TreeConverter.cc src/Converter.cc)
target_include_directories(xcsp3_converter PUBLIC ${PROJECT_SOURCE_DIR}/include ${XCSP3_CPP_Parser_SOURCE_DIR}/include ${LIBXML2_INCLUDE_DIR})
target_link_libraries(xcsp3_converter XCSP3_CPP_Parser_lib ${LIBXML2_LIBRARIES})
add_dependencies(xcsp3_converter XCSP3_CPP_Parser_project)

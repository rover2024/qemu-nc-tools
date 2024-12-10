project(clangutils)

file(GLOB _src *.h *.cpp)

add_library(${PROJECT_NAME} STATIC ${_src})

nc_link_clang(${PROJECT_NAME})
project(nclift)

file(GLOB _src *.h *.cpp)

add_executable(${PROJECT_NAME} ${_src})

nc_link_clang(${PROJECT_NAME})
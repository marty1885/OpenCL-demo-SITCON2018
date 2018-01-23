project(heatflow)

add_executable(heatflow src/main.cpp)

target_link_libraries(heatflow DisplayWindow)
target_link_libraries(heatflow OpenCL)

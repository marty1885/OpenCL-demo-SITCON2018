project(DisplayWindow)

file(GLOB DISPLAYWINDOW_SRC "src/*.cpp")
add_library(DisplayWindow SHARED ${DISPLAYWINDOW_SRC})

if(WIN32)
	target_link_libraries(DisplayWindow SDL2 opengl32)
#elseif(APPLE)#OS X and Linux/BSD uses the same find_package, no need to do a if()
else()
	find_package(OpenGL REQUIRED)
 	target_link_libraries(DisplayWindow SDL2 ${OPENGL_LIBRARIES})
endif()

if(NOT APPLE)
	find_package(GLEW REQUIRED)
	target_link_libraries(DisplayWindow ${GLEW_LIBRARIES})
endif()

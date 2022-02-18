# Get the name of this directory and use that as the name of the build_target/application
get_filename_component(TARGET_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
MESSAGE("Target name: ${TARGET_NAME}")

# Find all source files
FILE(GLOB_RECURSE SOURCE *.cpp *.h)
#FILE(GLOB SOURCE_EXTRA ../*.cpp ../*.h)

#INCLUDE_DIRECTORIES(../)


# Create an executable from the sources
ADD_EXECUTABLE(${TARGET_NAME} ${SOURCE} ${SOURCE_EXTRA})

# This executable requires a few libraries to link
TARGET_LINK_LIBRARIES(${TARGET_NAME} vrlib ${GLFW3_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${SOIL_LIBRARIES} ${ASSIMP_LIBRARY} ${ZLIB_LIBRARY} ${FREETYPE_LIBRARIES} )

IF(NOT WIN32) 
	TARGET_LINK_LIBRARIES(${TARGET_NAME} ${X_LIBS}  )
ENDIF()

# Where to put files during install
INSTALL(TARGETS ${TARGET_NAME}  RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION bin )

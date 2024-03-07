file(
 GLOB_RECURSE ASSIGNMENT3-again_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENT3-again_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignment3-again's asset folder to bin when it is built
add_custom_target(copyAssetsA3-again ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENT3-again_INC} DESTINATION include/assignment3-again)
add_executable(assignment3-again ${ASSIGNMENT3-again_SRC} ${ASSIGNMENT3-again_INC})
target_link_libraries(assignment3-again PUBLIC core IMGUI assimp)
target_include_directories(assignment3-again PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignment3-again is built
add_dependencies(assignment3-again copyAssetsA3-again)
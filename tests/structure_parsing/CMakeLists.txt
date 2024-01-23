project(structure_parsing)

file(GLOB _src *.h *.cpp)

add_executable(mcstructure_test_${PROJECT_NAME} ${_src})

target_link_libraries(mcstructure_test_${PROJECT_NAME} PRIVATE mcstructure)
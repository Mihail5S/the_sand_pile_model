add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Parser)
target_link_libraries(${PROJECT_NAME} PRIVATE create_art)


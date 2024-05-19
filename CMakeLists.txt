cmake_minimum_required(VERSION 3.23)
project(Computerlub)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
enable_testing()
add_subdirectory(tests)

add_executable(Computerlub main.cpp )

target_link_libraries(${PROJECT_NAME} PRIVATE ComputerClubLib)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

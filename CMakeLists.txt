cmake_minimum_required(VERSION 3.7)

project(stodo)

add_executable(stodo "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
target_compile_definitions(stodo PRIVATE _WIN32_WINNT=0x0501) # For Windows
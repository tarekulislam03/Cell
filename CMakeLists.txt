cmake_minimum_required(VERSION 3.16)
project(CELL_Terminal VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -O2)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(READLINE REQUIRED readline)

add_executable(cell cell.c ui.c)
target_include_directories(cell PRIVATE ${READLINE_INCLUDE_DIRS})
target_link_libraries(cell PRIVATE ${READLINE_LIBRARIES})

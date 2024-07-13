cmake_minimum_required(VERSION 3.27)

project(variant)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

set(CMAKE_COMPILE_WARNING_AS_ERROR Off)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS On)


set(HEADER_FILES
    precompiled.hxx
)

set(SOURCE_FILES
    variant.cxx
    shapes.cxx
    virtual.cxx
    main.cxx
    ${HEADER_FILES}
)

add_compile_definitions(CELERO_HAS_SEH=0)

add_subdirectory(Celero)

add_executable(variant ${SOURCE_FILES})
target_precompile_headers(variant PRIVATE ${HEADER_FILES})
target_link_libraries(variant celero)


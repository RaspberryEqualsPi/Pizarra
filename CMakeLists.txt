cmake_minimum_required(VERSION 3.10.0)
project(Pizarra VERSION 0.1.0 LANGUAGES C CXX)

set(SOURCE_FILES
    main.cpp
    whiteboard.cpp
    Engine/Engine.cpp)
 
add_executable(Pizarra ${SOURCE_FILES})
set_property(TARGET Pizarra PROPERTY CXX_STANDARD 17)
target_include_directories(Pizarra PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_compile_definitions(SFML_STATIC)
target_link_directories(Pizarra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(Pizarra sfml-graphics-s)
target_link_libraries(Pizarra sfml-window-s)
target_link_libraries(Pizarra sfml-system-s)
target_link_libraries(Pizarra opengl32)
target_link_libraries(Pizarra freetype)
target_link_libraries(Pizarra winmm)
target_link_libraries(Pizarra gdi32)
target_link_libraries(Pizarra sfml-audio-s)
target_link_libraries(Pizarra flac)
target_link_libraries(Pizarra vorbisenc)
target_link_libraries(Pizarra vorbisfile)
target_link_libraries(Pizarra vorbis)
target_link_libraries(Pizarra ogg)
target_link_libraries(Pizarra sfml-network-s)
target_link_libraries(Pizarra ws2_32)
target_link_libraries(Pizarra tgui-s)
target_link_libraries(Pizarra SelbaWard)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

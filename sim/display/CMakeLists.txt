add_definitions(-DFONT_FILE="${CMAKE_CURRENT_BINARY_DIR}/cp437-8x8")
add_library(simdisplay SHARED
            Display.cpp)
target_link_libraries(simdisplay ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})
install(TARGETS simdisplay
        LIBRARY DESTINATION lib
        COMPONENT simulator)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/fonts/cp437-8x8 cp437-8x8
               COPYONLY)

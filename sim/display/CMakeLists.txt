add_library(simdisplay SHARED
            Display.cpp)
target_link_libraries(simdisplay ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})
install(TARGETS simdisplay
        LIBRARY DESTINATION lib)

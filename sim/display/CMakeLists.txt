# Copyright Jamie Iles, 2017
#
# This file is part of s80x86.
#
# s80x86 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# s80x86 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

add_definitions(-DFONT_FILE="${CMAKE_CURRENT_BINARY_DIR}/cp437-8x8")
add_library(simdisplay SHARED
            Display.cpp)
include_directories(../common)
target_link_libraries(simdisplay ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})
install(TARGETS simdisplay
        LIBRARY DESTINATION lib
        COMPONENT simulator)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/fonts/cp437-8x8 cp437-8x8
               COPYONLY)

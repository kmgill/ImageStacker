cmake_minimum_required(VERSION 3.15)
project(ImageStacker)

set(CMAKE_CXX_STANDARD 14)



find_package(Boost REQUIRED COMPONENTS program_options)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

include_directories("."  ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(ImageStacker main.cpp jpeg.cpp ImageBuffer.cpp Image.cpp Offset.cpp Stacker.cpp pngrw.cpp)

target_link_libraries(ImageStacker ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${Boost_LIBRARIES})

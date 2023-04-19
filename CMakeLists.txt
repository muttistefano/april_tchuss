cmake_minimum_required(VERSION 3.0.2)
project(april_tchuss)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++17)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

if (CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Release)
  add_definitions(-O3)
endif()


find_package(OpenCV REQUIRED)
find_package(PkgConfig)
pkg_search_module(apriltag REQUIRED apriltag)
set(apriltag_INCLUDE_DIRS "${apriltag_INCLUDE_DIRS}/apriltag")
link_directories(${apriltag_LIBDIR})


include_directories(include
  include
  ${apriltag_INCLUDE_DIRS}
)

if($ENV{ROS_DISTRO} STREQUAL "noetic") 
  message( "Building for ROS noetic" )
  find_package(catkin REQUIRED COMPONENTS
    roscpp
  )

  catkin_package(
  #  INCLUDE_DIRS include
  #  LIBRARIES april_tchuss
  #  CATKIN_DEPENDS roscpp
  #  DEPENDS system_lib
  )

  include_directories(
  # include
    ${OpenCV_INCLUDE_DIRS}
    ${catkin_INCLUDE_DIRS}
    ${apriltag_INCLUDE_DIRS}
  )

  add_library(${PROJECT_NAME}
    SHARED
    src/april_tchuss.cpp
  )

  target_link_libraries(${PROJECT_NAME}
    ${catkin_LIBRARIES}
    ${apriltag_LIBRARIES} 
    ${OpenCV_LIBS}
    tbb
  )

  add_executable(${PROJECT_NAME}_node src/node.cpp)

  target_link_libraries(${PROJECT_NAME}_node
  ${PROJECT_NAME}
  ${apriltag_LIBRARIES} 
  ${OpenCV_LIBS}
)


else()
  message( "Standard build" )
  include_directories(
  # include
    ${OpenCV_INCLUDE_DIRS}
    ${apriltag_INCLUDE_DIRS}
  )

  add_library(${PROJECT_NAME}
    SHARED
    src/april_tchuss.cpp
  )

  target_link_libraries(${PROJECT_NAME}
    ${apriltag_LIBRARIES} 
    ${OpenCV_LIBS}
  )

endif()




# add_executable(${PROJECT_NAME}_node src/april_tchuss.cpp)


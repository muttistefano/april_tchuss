cmake_minimum_required(VERSION 3.5)
project(apriltag_fast)

add_compile_options(-std=c++17)

# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
# SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

if (CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Release)
  add_definitions(-O3)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(PkgConfig)
find_package(realtime_tools REQUIRED)
find_package(generate_parameter_library REQUIRED)

pkg_search_module(apriltag REQUIRED apriltag)
set(apriltag_INCLUDE_DIRS "${apriltag_INCLUDE_DIRS}/apriltag")
link_directories(${apriltag_LIBDIR})


include_directories(include
  include
  ${apriltag_INCLUDE_DIRS}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

set(dependencies
  rclcpp
  realtime_tools
  tf2_ros
)

# generate_parameter_library(
#   nav_follow_parameters 
#   src/nav_follow_parameters.yaml 
# )



add_library(${PROJECT_NAME}
  SHARED
  src/apriltag_fast.cpp
)

ament_target_dependencies(${PROJECT_NAME} ${dependencies})

target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
  ${apriltag_LIBRARIES} 
  ${OpenCV_LIBS}
  tbb
)

add_executable(${PROJECT_NAME}_node src/node.cpp)

ament_target_dependencies(${PROJECT_NAME}_node ${dependencies})

target_link_libraries(${PROJECT_NAME}_node
${PROJECT_NAME}
${apriltag_LIBRARIES} 
${OpenCV_LIBS}
)

install(TARGETS ${PROJECT_NAME}_node ${PROJECT_NAME}
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

ament_package()
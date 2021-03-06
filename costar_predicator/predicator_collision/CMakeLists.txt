cmake_minimum_required(VERSION 2.8.3)
project(predicator_collision)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  moveit_msgs moveit_core moveit_ros_planning_interface moveit_ros_perception
  predicator_msgs std_srvs
  tf tf_conversions
  urdf
)

find_package(PkgConfig)
pkg_check_modules(LIBFCL REQUIRED fcl)
find_library(LIBFCL_LIBRARIES_FULL ${LIBFCL_LIBRARIES} ${LIBFCL_LIBRARY_DIRS})
set(LIBFCL_LIBRARIES "${LIBFCL_LIBRARIES_FULL}")

## System dependencies are found with CMake's conventions
find_package(Boost REQUIRED COMPONENTS system)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES predicator_geometry
  CATKIN_DEPENDS moveit_msgs moveit_core moveit_ros_perception moveit_ros_planning_interface predicator_msgs tf urdf urdfdom
  DEPENDS PkgConfig LIBFCL
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(include)
include_directories(
  SYSTEM
  ${catkin_INCLUDE_DIRS}
  ${LIBFCL_INCLUDE_DIRS}
)

link_directories(
  ${catkin_LIBRARY_DIRS}
)

## Declare a cpp library
# add_library(predicator_geometry
#   src/${PROJECT_NAME}/predicator_geometry.cpp
# )

## Declare a cpp executable
add_executable(predicator_geometry_node
  src/predicator_geometry.cpp
  src/predicate.h
  src/predicate.cpp
)

# collision executable
add_executable(predicator_collision_node
  src/predicator_collision.cpp
)

add_executable(predicator_robot_interaction_node
  src/predicator_robot_interaction.cpp
)

## Add cmake target dependencies of the executable/library
## as an example, message headers may need to be generated before nodes
# add_dependencies(predicator_geometry_node predicator_geometry_generate_messages_cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(predicator_geometry_node
  ${catkin_LIBRARIES}
  ${LIBFCL_LIBRARIES}
)

target_link_libraries(predicator_collision_node
  ${catkin_LIBRARIES}
)

target_link_libraries(predicator_robot_interaction_node
  ${catkin_LIBRARIES}
)

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
# install(TARGETS predicator_geometry predicator_geometry_node
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_predicator_geometry.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)

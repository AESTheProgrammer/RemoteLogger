﻿# CMakeList.txt : CMake project for RemoteLogger, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (RemoteLogger "RemoteLogger.cpp" "RemoteLogger.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RemoteLogger PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
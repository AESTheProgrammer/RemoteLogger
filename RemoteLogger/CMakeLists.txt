# CMakeList.txt : CMake project for RemoteLogger, include source and define
# project specific logic here.
#

# Add source to this project's executable.
# add_executable (RemoteLogger 
#   "RemoteLogger.cpp" 
#   "Server.cpp" 
#   "Acceptor.cpp"
#   "Service.cpp"

#   "RemoteLogger.hpp"
#   "Server.hpp" 
#   "Acceptor.hpp"
#   "Service.hpp")

set (SOURCES 
  RemoteLogger.cpp
  SSLConnection/Server.cpp
  SSLConnection/Acceptor.cpp
  SSLConnection/Service.cpp
  FileUtil.cpp
  DBUtil.cpp
)

set (INCLUDES
  RemoteLogger.hpp 
  SSLConnection/Server.hpp
  SSLConnection/Acceptor.hpp
  SSLConnection/Service.hpp
  FileUtil.hpp
  DBUtil.hpp
)

set (ARTIFACTS
dhparams.pem
cert.pem
key.pem)


add_executable(RemoteLogger ${SOURCES} ${INCLUDES})

configure_file(${CMAKE_SOURCE_DIR}/${ARTIFACTS} COPYONLY)
set_target_properties(RemoteLogger PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RemoteLogger PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  cmake_policy(SET CMP0079 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("RemoteLogger")

# Include sub-projects.
add_subdirectory ("RemoteLogger")


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON) 
set(Boost_USE_STATIC_RUNTIME OFF) 
set(Boost_DEBUG ON)
set(DETAILED_FAILURE_MSG ON)
#set(Boost_INCLUDE_DIR "C:/Boost/boost_1_82_0/include/boost-1_82")
find_package(Boost 1.82.0 REQUIRED)
find_package(OpenSSL REQUIRED)
#set(ENV{DCMAKE_TOOLCHAIN_FILE} "C:/src/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake")
find_package(bsoncxx CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)
if(Boost_FOUND)
    message("include directory: ${Boost_INCLUDE_DIRS}")
    message("library directory: ${Boost_LIBRARY_DIRS}")
endif()
target_link_libraries(RemoteLogger PRIVATE 
${Boost_LIBRARIES} 
OpenSSL::SSL OpenSSL::Crypto
$<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
$<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)
target_include_directories(RemoteLogger PUBLIC ${Boost_INCLUDE_DIRS})
# find_package(unofficial-concurrentqueue CONFIG REQUIRED)
find_path(CONCURRENTQUEUE_INCLUDE_DIR concurrentqueue.h "C:\\src\\vcpkg\\vcpkg\\installed\\x86-windows\\include\\concurrentqueue")
target_include_directories(RemoteLogger  PUBLIC ${CONCURRENTQUEUE_INCLUDE_DIR} PUBLIC ${Boost_INCLUDE_DIRS})
message("prefix path: ${CMAKE_PREFIX_PATH}")
message("toolchain f: ${CMAKE_TOOLCHAIN_FILE}")


#include(CheckIncludeFileCXX)
#macro(CHECK_INCLUDE_FILE_ERROR INCLUDE_FILE HAVE_FILE)
#    unset(${HAVE_FILE})
#    CHECK_INCLUDE_FILE_CXX(${INCLUDE_FILE} ${HAVE_FILE})
#    IF(NOT ${HAVE_FILE})
#        UNSET(HAVE_FILE CACHE)
#        message( FATAL_ERROR "${INCLUDE_FILE} is not found      ${HAVE_FILE}" )
#    ENDIF()
#endmacro()

#CHECK_INCLUDE_FILE_ERROR("/boost/asio.hpp" HAVE_ASIO_HPP)
#find_path(ASIO_DIR_PATH asio.hpp "${Boost_INCLUDE_DIRS}/boost")
#if(NOT ASIO_DIR_PATH)
#    message(FATAL_ERROR "Can't find folder containing asio.hpp")
#else()
#    message("Include path: ${ASIO_DIR_PATH}")
#endif()
#
#find_file(ASIO_FILE_PATH asio.hpp "${Boost_INCLUDE_DIRS}/boost")
#if(NOT ASIO_FILE_PATH)
#  message(FATAL_ERROR "Can't find asio.hpp in ${Boost_INCLUDE_DIRS}/boost")
#endif()

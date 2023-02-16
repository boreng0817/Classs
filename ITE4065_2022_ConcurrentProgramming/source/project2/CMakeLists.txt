cmake_minimum_required (VERSION 2.6)
project (Sig18)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address -Wall")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(include)

add_library(snapshot NaiveSnapshot.cpp SimpleSnapshot.cpp WFSnapshot.cpp)
target_include_directories(snapshot PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

add_executable(run run.cpp)

target_link_libraries(run snapshot Threads::Threads)

# Options for libraries
option(USE_GOOGLE_TEST "Use GoogleTest for testing" ON)

# GoogleTest
if(USE_GOOGLE_TEST)
  add_subdirectory(test)
endif()

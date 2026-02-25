cmake_minimum_required(VERSION 3.20)
project(mysqlpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mysqlpp INTERFACE)
add_library(mysqlpp::mysqlpp ALIAS mysqlpp)

target_include_directories(mysqlpp INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if (MSVC)
  target_compile_options(mysqlpp INTERFACE /W4 /permissive-)
else()
  target_compile_options(mysqlpp INTERFACE -Wall -Wextra -Wpedantic)
endif()

# --- Dependency: MySQL Connector/C++ ---
# Prefer a CMake package if present, otherwise use robust fallback (find_path/find_library).
set(MYSQLPP_HAS_CONNECTOR OFF)

find_package(MySQLCppConn QUIET)
if (MySQLCppConn_FOUND)
  target_link_libraries(mysqlpp INTERFACE MySQLCppConn::MySQLCppConn)
  set(MYSQLPP_HAS_CONNECTOR ON)
else()
  find_package(mysqlcppconn QUIET)
  if (mysqlcppconn_FOUND)
    target_link_libraries(mysqlpp INTERFACE mysqlcppconn)
    set(MYSQLPP_HAS_CONNECTOR ON)
  else()
    # Fallback: locate headers + libs directly (works across many distros)
    find_path(MYSQLCPPCONN_INCLUDE_DIR
      NAMES cppconn/driver.h
    )

    find_library(MYSQLCPPCONN_LIBRARY
      NAMES mysqlcppconn mysqlcppconn8
    )

    if (MYSQLCPPCONN_INCLUDE_DIR AND MYSQLCPPCONN_LIBRARY)
      target_include_directories(mysqlpp INTERFACE ${MYSQLCPPCONN_INCLUDE_DIR})
      target_link_libraries(mysqlpp INTERFACE ${MYSQLCPPCONN_LIBRARY})
      set(MYSQLPP_HAS_CONNECTOR ON)

      # Optional: some environments also require libmysqlclient
      find_library(MYSQLCLIENT_LIBRARY NAMES mysqlclient)
      if (MYSQLCLIENT_LIBRARY)
        target_link_libraries(mysqlpp INTERFACE ${MYSQLCLIENT_LIBRARY})
      endif()
    endif()
  endif()
endif()

if (NOT MYSQLPP_HAS_CONNECTOR)
  message(FATAL_ERROR
    "mysql-connector-cpp not found.\n"
    "Install the development package (headers + library) and reconfigure.\n"
    "Expected: cppconn/driver.h and libmysqlcppconn (or libmysqlcppconn8)."
  )
endif()

include(CTest)
enable_testing()

option(MYSQLPP_BUILD_EXAMPLES "Build examples" ON)

if (MYSQLPP_BUILD_EXAMPLES)
  add_executable(mysqlpp_example_basic examples/basic_connect.cpp)
  target_link_libraries(mysqlpp_example_basic PRIVATE mysqlpp::mysqlpp)

  add_executable(mysqlpp_example_insert examples/insert_and_select.cpp)
  target_link_libraries(mysqlpp_example_insert PRIVATE mysqlpp::mysqlpp)

  add_executable(mysqlpp_example_tx examples/transaction.cpp)
  target_link_libraries(mysqlpp_example_tx PRIVATE mysqlpp::mysqlpp)
endif()

add_executable(mysqlpp_basic_test tests/test_basic.cpp)
target_link_libraries(mysqlpp_basic_test PRIVATE mysqlpp::mysqlpp)
add_test(NAME mysqlpp.basic COMMAND mysqlpp_basic_test)

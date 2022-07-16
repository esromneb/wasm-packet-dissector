# Install script for directory: /src/wireshark/epan/wmem

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/emsdk/upstream/emscripten/cache/sysroot")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/epan/wmem/wmem.h;/epan/wmem/wmem_array.h;/epan/wmem/wmem_core.h;/epan/wmem/wmem_list.h;/epan/wmem/wmem_map.h;/epan/wmem/wmem_miscutl.h;/epan/wmem/wmem_queue.h;/epan/wmem/wmem_scopes.h;/epan/wmem/wmem_stack.h;/epan/wmem/wmem_strbuf.h;/epan/wmem/wmem_strutl.h;/epan/wmem/wmem_tree.h;/epan/wmem/wmem_interval_tree.h;/epan/wmem/wmem_user_cb.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/epan/wmem" TYPE FILE FILES
    "/src/wireshark/epan/wmem/wmem.h"
    "/src/wireshark/epan/wmem/wmem_array.h"
    "/src/wireshark/epan/wmem/wmem_core.h"
    "/src/wireshark/epan/wmem/wmem_list.h"
    "/src/wireshark/epan/wmem/wmem_map.h"
    "/src/wireshark/epan/wmem/wmem_miscutl.h"
    "/src/wireshark/epan/wmem/wmem_queue.h"
    "/src/wireshark/epan/wmem/wmem_scopes.h"
    "/src/wireshark/epan/wmem/wmem_stack.h"
    "/src/wireshark/epan/wmem/wmem_strbuf.h"
    "/src/wireshark/epan/wmem/wmem_strutl.h"
    "/src/wireshark/epan/wmem/wmem_tree.h"
    "/src/wireshark/epan/wmem/wmem_interval_tree.h"
    "/src/wireshark/epan/wmem/wmem_user_cb.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/src/wireshark/epan/wmem/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

# Install script for directory: /home/xie/github-wsl/wd_http/wfrest-main

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/wfrest" TYPE FILE RENAME "wfrest-config.cmake" FILES "/home/xie/github-wsl/wd_http/build/config.toinstall.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/wfrest" TYPE FILE FILES
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/Copyable.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/ErrorCode.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/json_fwd.hpp"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/json.hpp"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/Macro.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/Noncopyable.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/StringPiece.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/Timestamp.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/base64.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/Compress.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/SysInfo.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/base/Json.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/HttpContent.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/HttpCookie.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/HttpDef.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/HttpFile.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/HttpMsg.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/HttpServer.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/HttpServerTask.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/MultiPartParser.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/BluePrint.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/BluePrint.inl"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/Router.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/RouteTable.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/VerbHandler.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/AopUtil.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/core/Aspect.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/util/FileUtil.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/util/MysqlUtil.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/util/PathUtil.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/util/StrUtil.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/util/UriUtil.h"
    "/home/xie/github-wsl/wd_http/wfrest-main/src/util/CodeUtil.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/wfrest-0.9.2" TYPE FILE FILES "/home/xie/github-wsl/wd_http/wfrest-main/README.md")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/xie/github-wsl/wd_http/build/src/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/xie/github-wsl/wd_http/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

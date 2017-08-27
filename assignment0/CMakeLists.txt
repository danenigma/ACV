# Author: David Vernon
 
#Reduce warning level with cmake 2.6
IF(WIN32)
cmake_minimum_required(VERSION 2.6)
#disable annoying warnings about use of deprecated c functions
ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libc;libcmt.lib;libcmtd.lib") 
ELSE(WIN32)
cmake_minimum_required(VERSION 2.4)
ENDIF(WIN32)

##############################################
PROJECT(assignment0)
##############################################

SET(NESTED_BUILD TRUE)

SET(ACV_DIR ${PROJECT_SOURCE_DIR})
IF (NOT ACV_FOUND)
#Changing the default CMAKE_INSTALL_PREFIX
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "${ACV_DIR}" CACHE PATH "ACV install prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
ENDIF (NOT ACV_FOUND)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

FIND_PACKAGE(OPENCV)

SUBDIRS(src)
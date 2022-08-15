SET(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo CACHE TYPE INTERNAL FORCE) 
cmake_minimum_required(VERSION 3.0)
project(CrapsSimulator)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Modules ${CMAKE_MODULE_PATH})
find_package(BPSQt55 REQUIRED)
find_package(CreateUserVCProj REQUIRED)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)

IF(CMAKE_SIZEOF_VOID_P EQUAL 4) #32 bit
    SET( BITSIZE 32 )
ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 8) #64 bit
    SET( BITSIZE 64 )
ELSE (CMAKE_SIZEOF_VOID_P EQUAL 8) 
    MESSAGE( STATUS "Unknown Bitsize - CMAKE_SIZEOF_VOID_P not set to 4 or 8" )
    MESSAGE( STATUS "-DCMAKE_SIZEOF_VOID_P=4 for 32 bit" )
    MESSAGE( FATAL_ERROR "-DCMAKE_SIZEOF_VOID_P=8 for 64 bit" )
ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 4) 

Message( STATUS "Building application with pointer size ${BITSIZE} bits" )

#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation
#



include(include.cmake)
include(Project.cmake )
set(QT_LIBRARIES 
    Qt5::Gui
    Qt5::Core
    Qt5::Widgets
    Qt5::Sql
    )
QT5_ADD_RESOURCES( qtproject_QRC_SRCS ${qtproject_QRC} )

add_executable(CrapsSimulator 
    WIN32 
    ${qtproject_SRCS} 
    ${qtproject_UIS_H} 
    ${qtproject_H} 
    ${qtproject_MOC_SRCS} 
    ${qtproject_QRC_SRCS} 
    craps.rc
    ) 
set_target_properties( CrapsSimulator PROPERTIES FOLDER Applications )
SET(CMAKE_INSTALL_PREFIX "E:/Dropbox/CrapsSimulator" )

CreateUserProj()
SET(QT_DONT_USE_QTCORE FALSE)
    SET(QT_DONT_USE_QTGUI FALSE)
    SET(QT_USE_XML_PATTERNS TRUE)
    SET(QT_USE_XML TRUE)
    SET(QT_USE_QTNETWORK TRUE)
    SET(QT_USE_QTSQL TRUE)
    SET(QT_USE_QTSCRIPT TRUE)
    SET(QT_USE_QTSCRIPTTOOLS TRUE)
    SET(QT_USE_QTWEBKIT TRUE)
    BPS_Install_Qt()
IF(WIN32)
    SET( EXTRA_LIBS Iphlpapi.lib psapi.lib )
ELSE(WIN32)
    SET( EXTRA_LIBS "" )
ENDIF(WIN32)

IF(WIN32)
    SET_TARGET_PROPERTIES(CrapsSimulator PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcprt.lib"
        )
ENDIF(WIN32)

target_link_libraries( CrapsSimulator 
    ${QT_QTMAIN_LIBRARY} 
    ${QT_LIBRARIES} 
)

INSTALL(TARGETS CrapsSimulator RUNTIME DESTINATION bin )

IF(WIN32)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/CrapsSimulator.pdb DESTINATION bin CONFIGURATIONS Debug )
ENDIF()

#SET( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin )
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Craps Simulator")
SET(CPACK_PACKAGE_VENDOR "Towel 42 Software")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CrapsSimulator")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/resources\\\\craps.png")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "CrapsSimulator.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "Craps Simulator")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.towel42.com")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.towel42.com")
  SET(CPACK_NSIS_CONTACT "scott@towel42.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "CrapsSimulator")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "CrapsSimulator" "CrapsSimulator" )
INCLUDE(CPack)



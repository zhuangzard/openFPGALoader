# Install script for directory: /Users/taisen/Downloads/Working/github/openFPGALoader

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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/taisen/Downloads/Working/github/openFPGALoader/cmake-build-debug/openFPGALoader")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openFPGALoader" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openFPGALoader")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/Cellar/libusb/1.0.25/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openFPGALoader")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/Cellar/libftdi/1.5_2/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openFPGALoader")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/openFPGALoader")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openFPGALoader" TYPE FILE FILES
    "/Users/taisen/Downloads/Working/github/openFPGALoader/test_sfl.svf"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc6slx100fgg484.bit"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc6slx45csg324.bit"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7a100tfgg484.bit"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7a200tsbg484.bit"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7a35tcpg236.bit"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7a35tcsg324.bit"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7a35tftg256.bit"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7a50tcpg236.bit"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7a75tfgg484.bit"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_10cl025256.rbf"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_5ce223.rbf"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_ep4ce2217.rbf"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_5ce423.rbf.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_5ce523.rbf.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_ep4ce1523.rbf.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc3s500evq100.bit.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc6slx150tfgg484.bit.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc6slx16csg324.bit.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc6slx16ftg256.bit.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7a100tcsg324.bit.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7k325tffg676.bit.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7k325tffg900.bit.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7s25csga324.bit.gz"
    "/Users/taisen/Downloads/Working/github/openFPGALoader/spiOverJtag/spiOverJtag_xc7s50csga324.bit.gz"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/taisen/Downloads/Working/github/openFPGALoader/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

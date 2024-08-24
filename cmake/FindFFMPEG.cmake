# Distributed under the OSI-approved BSD 3-Clause License.
#
#.rst:
# FindFFMPEG
# --------
#
# Find the FFPMEG libraries
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# The following variables will be defined:
#
#  ``FFMPEG_FOUND``
#    True if FFMPEG found on the local system
#
#  ``FFMPEG_INCLUDE_DIRS``
#    Location of FFMPEG header files
#
#  ``FFMPEG_LIBRARY_DIRS``
#    Location of FFMPEG libraries
#
#  ``FFMPEG_LIBRARIES``
#    List of the FFMPEG libraries found
#
#

include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)
include(CMakeFindDependencyMacro)

if(NOT FFMPEG_FOUND)

# Compute the installation path relative to this file.
get_filename_component(SEARCH_PATH "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(SEARCH_PATH "${SEARCH_PATH}" PATH)
get_filename_component(SEARCH_PATH "${SEARCH_PATH}" PATH)
if(SEARCH_PATH STREQUAL "/")
  set(SEARCH_PATH "")
endif()

set(FFMPEG_VERSION "7.0")

function(append_dependencies out)
    cmake_parse_arguments(PARSE_ARGV 1 "arg" "DEBUG" "NAMES" "")
    if(${arg_DEBUG})
        set(config DEBUG)
        set(path "${CURRENT_INSTALLED_DIR}/debug/lib/")
    else()
        set(config RELEASE)
        set(path "${CURRENT_INSTALLED_DIR}/lib/")
    endif()
    if("${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES}" STREQUAL "")
        enable_language(CXX)
    endif()
    set(pass_through
        ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES}
        bcrypt gdi32 mfuuid ole32 oleaut32 psapi secur32 shlwapi strmiids user32 uuid vfw32 ws2_32
        -pthread -pthreads pthread atomic m
    )
    cmake_policy(SET CMP0057 NEW)
    foreach(lib_name IN LISTS arg_NAMES)
        if(lib_name IN_LIST CMAKE_C_IMPLICIT_LINK_LIBRARIES)
            continue() # implicit even for C
        elseif(lib_name IN_LIST pass_through)
            list(APPEND ${out} "${lib_name}")
        elseif(EXISTS "${lib_name}")
            list(APPEND ${out} "${lib_name}")
        else()
            # first look in ${path} specifically to ensure we find the right release/debug variant
            find_library(FFMPEG_DEPENDENCY_${lib_name}_${config} NAMES "${lib_name}" PATHS "${path}" NO_DEFAULT_PATH)
            # if not found there, must be a system dependency, so look elsewhere
            find_library(FFMPEG_DEPENDENCY_${lib_name}_${config} NAMES "${lib_name}" REQUIRED)
            list(APPEND ${out} "${FFMPEG_DEPENDENCY_${lib_name}_${config}}")
        endif()
    endforeach()
    set("${out}" "${${out}}" PARENT_SCOPE)
endfunction()

macro(FFMPEG_FIND varname shortname headername)
  if(NOT FFMPEG_${varname}_INCLUDE_DIRS)
    find_path(FFMPEG_${varname}_INCLUDE_DIRS NAMES lib${shortname}/${headername} ${headername} PATHS ${SEARCH_PATH}/include NO_DEFAULT_PATH)
  endif()
  if(NOT FFMPEG_${varname}_LIBRARY)
    find_library(FFMPEG_${varname}_LIBRARY_RELEASE NAMES ${shortname} PATHS ${SEARCH_PATH}/lib/ NO_DEFAULT_PATH)
    find_library(FFMPEG_${varname}_LIBRARY_DEBUG NAMES ${shortname}d ${shortname} PATHS ${SEARCH_PATH}/debug/lib/ NO_DEFAULT_PATH)
    get_filename_component(FFMPEG_${varname}_LIBRARY_RELEASE_DIR ${FFMPEG_${varname}_LIBRARY_RELEASE} DIRECTORY)
    get_filename_component(FFMPEG_${varname}_LIBRARY_DEBUG_DIR ${FFMPEG_${varname}_LIBRARY_DEBUG} DIRECTORY)
    select_library_configurations(FFMPEG_${varname})
    set(FFMPEG_${varname}_LIBRARY ${FFMPEG_${varname}_LIBRARY} CACHE STRING "")
  endif()
  if (FFMPEG_${varname}_LIBRARY AND FFMPEG_${varname}_INCLUDE_DIRS)
    set(FFMPEG_${varname}_FOUND TRUE BOOL)
    list(APPEND FFMPEG_INCLUDE_DIRS ${FFMPEG_${varname}_INCLUDE_DIRS})
    list(APPEND FFMPEG_LIBRARIES ${FFMPEG_${varname}_LIBRARY})
    list(APPEND FFMPEG_LIBRARY_DIRS ${FFMPEG_${varname}_LIBRARY_RELEASE_DIR} ${FFMPEG_${varname}_LIBRARY_DEBUG_DIR})
  endif()
endmacro(FFMPEG_FIND)

if(ON)
  FFMPEG_FIND(libavdevice   avdevice   avdevice.h)
endif()
if(ON)
  FFMPEG_FIND(libavfilter   avfilter   avfilter.h)
endif()
if(ON)
  FFMPEG_FIND(libavformat   avformat   avformat.h)
endif()
if(ON)
  FFMPEG_FIND(libavcodec    avcodec    avcodec.h)
endif()
if(OFF)
  FFMPEG_FIND(libpostproc   postproc   postprocess.h)
endif()
if(ON)
  FFMPEG_FIND(libswresample swresample swresample.h)
endif()
if(ON)
  FFMPEG_FIND(libswscale    swscale    swscale.h)
endif()
FFMPEG_FIND(libavutil       avutil     avutil.h)

if (FFMPEG_libavutil_FOUND)
  list(REMOVE_DUPLICATES FFMPEG_INCLUDE_DIRS)
  list(REMOVE_DUPLICATES FFMPEG_LIBRARY_DIRS)
  set(FFMPEG_libavutil_VERSION "59.8.100" CACHE STRING "")

  if(FFMPEG_libavcodec_FOUND)
    set(FFMPEG_libavcodec_VERSION "61.3.100" CACHE STRING "")
  endif()
  if(FFMPEG_libavdevice_FOUND)
    set(FFMPEG_libavdevice_VERSION "61.1.100" CACHE STRING "")
  endif()
  if(FFMPEG_libavfilter_FOUND)
    set(FFMPEG_libavfilter_VERSION "10.1.100" CACHE STRING "")
  endif()
  if(FFMPEG_libavformat_FOUND)
    set(FFMPEG_libavformat_VERSION "61.1.100" CACHE STRING "")
  endif()
  if(FFMPEG_libswresample_FOUND)
    set(FFMPEG_libswresample_VERSION "5.1.100" CACHE STRING "")
  endif()
  if(FFMPEG_libswscale_FOUND)
    set(FFMPEG_libswscale_VERSION "8.1.100" CACHE STRING "")
  endif()

  append_dependencies(FFMPEG_DEPS_LIBRARY_RELEASE NAMES "psapi;uuid;oleaut32;shlwapi;gdi32;vfw32;secur32;ws2_32;mfuuid;strmiids;ole32;user32;bcrypt")
  append_dependencies(FFMPEG_DEPS_LIBRARY_DEBUG   NAMES "psapi;uuid;oleaut32;shlwapi;gdi32;vfw32;secur32;ws2_32;mfuuid;strmiids;ole32;user32;bcrypt" DEBUG)
  if(FFMPEG_DEPS_LIBRARY_RELEASE OR FFMPEG_DEPS_LIBRARY_DEBUG)
    select_library_configurations(FFMPEG_DEPS)
    list(APPEND FFMPEG_LIBRARIES ${FFMPEG_DEPS_LIBRARY})
  endif()

  set(FFMPEG_LIBRARY ${FFMPEG_LIBRARIES})

  set(FFMPEG_FOUND TRUE CACHE BOOL "")
  set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} CACHE STRING "")
  set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIRS} CACHE STRING "")
  set(FFMPEG_LIBRARY_DIRS ${FFMPEG_LIBRARY_DIRS} CACHE STRING "")
endif()

find_package_handle_standard_args(FFMPEG REQUIRED_VARS FFMPEG_LIBRARIES FFMPEG_LIBRARY_DIRS FFMPEG_INCLUDE_DIRS)

set(z_vcpkg_using_vcpkg_find_ffmpeg ${FFMPEG_FOUND})

endif()

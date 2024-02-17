cmake_minimum_required(VERSION 3.15...3.26)

project(bliss_bind LANGUAGES CXX)
set(CMAKE_POSITION_INDEPENDENT_CODE True)

if (NOT SKBUILD)
  message(WARNING "\
  This CMake file is meant to be executed using 'scikit-build'. Running
  it directly will almost certainly not produce the desired result. If
  you are a user trying to install this package, please use the command
  below, which will install all necessary build dependencies, compile
  the package in an isolated environment, and then install it.
  =====================================================================
   $ pip install .
  =====================================================================
  If you are a software developer, and this is your own package, then
  it is usually much more efficient to install the build dependencies
  in your environment once and use the following command that avoids
  a costly creation of a new virtual environment at every compilation:
  =====================================================================
   $ pip install nanobind scikit-build-core[pyproject]
   $ pip install --no-build-isolation -ve .
  =====================================================================
  You may optionally add -Ceditable.rebuild=true to auto-rebuild when
  the package is imported. Otherwise, you need to re-run the above
  after editing C++ files.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Python 3.8
  REQUIRED COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

find_package(nanobind CONFIG REQUIRED)


include(FetchContent)

FetchContent_Declare(
  bliss
  URL https://users.aalto.fi/~tjunttil/bliss/downloads/bliss-0.77.zip 
  URL_HASH MD5=5707cbfd9fd00980571c64ab3584c505
)

FetchContent_MakeAvailable(bliss)

target_include_directories(bliss_static PUBLIC ${bliss_SOURCE_DIR}/src)

nanobind_add_module(
  bliss_bind_ext
  STABLE_ABI NB_STATIC
  src/bliss_bind.cpp
)
target_link_libraries(bliss_bind_ext PRIVATE bliss_static)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(bliss_bind_ext PRIVATE
    /W4 /permissive- /w14242 /w14254 /w14263 /w14265 /w14287 /we4289 /w14296
    /w14311 /w14545 /w14546 /w14547 /w14549 /w14555 /w14619 /w14640 /w14826
    /w14905 /w14906 /w14928)
else()
  target_compile_options(bliss_bind_ext PRIVATE
    -Werror -Wall -Wextra -Wconversion -Wsign-conversion -Wnull-dereference
    -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused
    -Woverloaded-virtual -Wpedantic -Wimplicit-fallthrough)
endif()

install(TARGETS bliss_bind_ext LIBRARY DESTINATION bliss_bind)

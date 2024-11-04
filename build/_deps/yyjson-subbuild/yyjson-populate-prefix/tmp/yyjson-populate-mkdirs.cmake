# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/anhld/Projects/nestalgic/build/_deps/yyjson-src"
  "/home/anhld/Projects/nestalgic/build/_deps/yyjson-build"
  "/home/anhld/Projects/nestalgic/build/_deps/yyjson-subbuild/yyjson-populate-prefix"
  "/home/anhld/Projects/nestalgic/build/_deps/yyjson-subbuild/yyjson-populate-prefix/tmp"
  "/home/anhld/Projects/nestalgic/build/_deps/yyjson-subbuild/yyjson-populate-prefix/src/yyjson-populate-stamp"
  "/home/anhld/Projects/nestalgic/build/_deps/yyjson-subbuild/yyjson-populate-prefix/src"
  "/home/anhld/Projects/nestalgic/build/_deps/yyjson-subbuild/yyjson-populate-prefix/src/yyjson-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/anhld/Projects/nestalgic/build/_deps/yyjson-subbuild/yyjson-populate-prefix/src/yyjson-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/anhld/Projects/nestalgic/build/_deps/yyjson-subbuild/yyjson-populate-prefix/src/yyjson-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

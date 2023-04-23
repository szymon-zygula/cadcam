cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cadappcam LANGUAGES C CXX)

add_executable(cadappcam main.cpp)

target_compile_options(cadappcam PUBLIC $<$<COMPILE_LANGUAGE:CXX>: -g >)
add_compile_definitions(__DEBUG__)

target_compile_features(cadappcam PUBLIC cxx_std_17)
target_include_directories(cadappcam PUBLIC "/usr/include/opencascade/")
target_link_libraries(
  cadappcam
  PUBLIC TKService
         TKernel
         TKMath
         TKFeat
         TKBO
         TKPrim
         TKMesh
         TKTopAlgo)

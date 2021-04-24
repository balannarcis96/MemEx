#################################################
# 			MemEx by Narcis96					#
#################################################

cmake_minimum_required (VERSION 3.8)

project ("MemEx" VERSION 1.0.0)

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#We remove exceptions and rtti and we add 
#	-multiprocessor compilation (/MP) 
#	-latest c++ version(/std:c++latest) 
#	-intrinsic functions(/Oi) 
#	-favor speed (/Ot)
set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /std:c++latest /MP /Oi /Ot")

# Include sub-projects.

#Libs
add_subdirectory ("MemEx")

#Executables
add_subdirectory ("Tests")
# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.3

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.3.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.3.1/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/antonio/Documents/ARGoS/iAnt-ARGoS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build

# Include any dependencies generated for this target.
include source/iAntBase/CMakeFiles/iAntBaseController.dir/depend.make

# Include the progress variables for this target.
include source/iAntBase/CMakeFiles/iAntBaseController.dir/progress.make

# Include the compile flags for this target's objects.
include source/iAntBase/CMakeFiles/iAntBaseController.dir/flags.make

source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o: source/iAntBase/CMakeFiles/iAntBaseController.dir/flags.make
source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o: ../source/iAntBase/iAntBaseController.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/antonio/Documents/ARGoS/iAnt-ARGoS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o"
	cd /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build/source/iAntBase && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o -c /Users/antonio/Documents/ARGoS/iAnt-ARGoS/source/iAntBase/iAntBaseController.cpp

source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.i"
	cd /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build/source/iAntBase && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/antonio/Documents/ARGoS/iAnt-ARGoS/source/iAntBase/iAntBaseController.cpp > CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.i

source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.s"
	cd /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build/source/iAntBase && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/antonio/Documents/ARGoS/iAnt-ARGoS/source/iAntBase/iAntBaseController.cpp -o CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.s

source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o.requires:

.PHONY : source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o.requires

source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o.provides: source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o.requires
	$(MAKE) -f source/iAntBase/CMakeFiles/iAntBaseController.dir/build.make source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o.provides.build
.PHONY : source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o.provides

source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o.provides.build: source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o


# Object files for target iAntBaseController
iAntBaseController_OBJECTS = \
"CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o"

# External object files for target iAntBaseController
iAntBaseController_EXTERNAL_OBJECTS =

source/iAntBase/libiAntBaseController.dylib: source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o
source/iAntBase/libiAntBaseController.dylib: source/iAntBase/CMakeFiles/iAntBaseController.dir/build.make
source/iAntBase/libiAntBaseController.dylib: source/iAntBase/CMakeFiles/iAntBaseController.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/antonio/Documents/ARGoS/iAnt-ARGoS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libiAntBaseController.dylib"
	cd /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build/source/iAntBase && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/iAntBaseController.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
source/iAntBase/CMakeFiles/iAntBaseController.dir/build: source/iAntBase/libiAntBaseController.dylib

.PHONY : source/iAntBase/CMakeFiles/iAntBaseController.dir/build

source/iAntBase/CMakeFiles/iAntBaseController.dir/requires: source/iAntBase/CMakeFiles/iAntBaseController.dir/iAntBaseController.cpp.o.requires

.PHONY : source/iAntBase/CMakeFiles/iAntBaseController.dir/requires

source/iAntBase/CMakeFiles/iAntBaseController.dir/clean:
	cd /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build/source/iAntBase && $(CMAKE_COMMAND) -P CMakeFiles/iAntBaseController.dir/cmake_clean.cmake
.PHONY : source/iAntBase/CMakeFiles/iAntBaseController.dir/clean

source/iAntBase/CMakeFiles/iAntBaseController.dir/depend:
	cd /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/antonio/Documents/ARGoS/iAnt-ARGoS /Users/antonio/Documents/ARGoS/iAnt-ARGoS/source/iAntBase /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build/source/iAntBase /Users/antonio/Documents/ARGoS/iAnt-ARGoS/build/source/iAntBase/CMakeFiles/iAntBaseController.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : source/iAntBase/CMakeFiles/iAntBaseController.dir/depend


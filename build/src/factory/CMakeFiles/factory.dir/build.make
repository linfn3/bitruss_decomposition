# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/Mywork/bitruss-decomposition-main

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/Mywork/bitruss-decomposition-main/build

# Include any dependencies generated for this target.
include src/factory/CMakeFiles/factory.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/factory/CMakeFiles/factory.dir/compiler_depend.make

# Include the progress variables for this target.
include src/factory/CMakeFiles/factory.dir/progress.make

# Include the compile flags for this target's objects.
include src/factory/CMakeFiles/factory.dir/flags.make

src/factory/CMakeFiles/factory.dir/factory.cc.o: src/factory/CMakeFiles/factory.dir/flags.make
src/factory/CMakeFiles/factory.dir/factory.cc.o: /mnt/d/Mywork/bitruss-decomposition-main/src/factory/factory.cc
src/factory/CMakeFiles/factory.dir/factory.cc.o: src/factory/CMakeFiles/factory.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Mywork/bitruss-decomposition-main/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/factory/CMakeFiles/factory.dir/factory.cc.o"
	cd /mnt/d/Mywork/bitruss-decomposition-main/build/src/factory && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/factory/CMakeFiles/factory.dir/factory.cc.o -MF CMakeFiles/factory.dir/factory.cc.o.d -o CMakeFiles/factory.dir/factory.cc.o -c /mnt/d/Mywork/bitruss-decomposition-main/src/factory/factory.cc

src/factory/CMakeFiles/factory.dir/factory.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/factory.dir/factory.cc.i"
	cd /mnt/d/Mywork/bitruss-decomposition-main/build/src/factory && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/Mywork/bitruss-decomposition-main/src/factory/factory.cc > CMakeFiles/factory.dir/factory.cc.i

src/factory/CMakeFiles/factory.dir/factory.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/factory.dir/factory.cc.s"
	cd /mnt/d/Mywork/bitruss-decomposition-main/build/src/factory && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/Mywork/bitruss-decomposition-main/src/factory/factory.cc -o CMakeFiles/factory.dir/factory.cc.s

# Object files for target factory
factory_OBJECTS = \
"CMakeFiles/factory.dir/factory.cc.o"

# External object files for target factory
factory_EXTERNAL_OBJECTS =

/mnt/d/Mywork/bitruss-decomposition-main/lib/libfactory.a: src/factory/CMakeFiles/factory.dir/factory.cc.o
/mnt/d/Mywork/bitruss-decomposition-main/lib/libfactory.a: src/factory/CMakeFiles/factory.dir/build.make
/mnt/d/Mywork/bitruss-decomposition-main/lib/libfactory.a: src/factory/CMakeFiles/factory.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/mnt/d/Mywork/bitruss-decomposition-main/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library /mnt/d/Mywork/bitruss-decomposition-main/lib/libfactory.a"
	cd /mnt/d/Mywork/bitruss-decomposition-main/build/src/factory && $(CMAKE_COMMAND) -P CMakeFiles/factory.dir/cmake_clean_target.cmake
	cd /mnt/d/Mywork/bitruss-decomposition-main/build/src/factory && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/factory.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/factory/CMakeFiles/factory.dir/build: /mnt/d/Mywork/bitruss-decomposition-main/lib/libfactory.a
.PHONY : src/factory/CMakeFiles/factory.dir/build

src/factory/CMakeFiles/factory.dir/clean:
	cd /mnt/d/Mywork/bitruss-decomposition-main/build/src/factory && $(CMAKE_COMMAND) -P CMakeFiles/factory.dir/cmake_clean.cmake
.PHONY : src/factory/CMakeFiles/factory.dir/clean

src/factory/CMakeFiles/factory.dir/depend:
	cd /mnt/d/Mywork/bitruss-decomposition-main/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/Mywork/bitruss-decomposition-main /mnt/d/Mywork/bitruss-decomposition-main/src/factory /mnt/d/Mywork/bitruss-decomposition-main/build /mnt/d/Mywork/bitruss-decomposition-main/build/src/factory /mnt/d/Mywork/bitruss-decomposition-main/build/src/factory/CMakeFiles/factory.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/factory/CMakeFiles/factory.dir/depend


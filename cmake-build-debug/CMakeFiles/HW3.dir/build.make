# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/HW3.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/HW3.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/HW3.dir/flags.make

CMakeFiles/HW3.dir/main.c.o: CMakeFiles/HW3.dir/flags.make
CMakeFiles/HW3.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/HW3.dir/main.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/HW3.dir/main.c.o   -c /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/main.c

CMakeFiles/HW3.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/HW3.dir/main.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/main.c > CMakeFiles/HW3.dir/main.c.i

CMakeFiles/HW3.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/HW3.dir/main.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/main.c -o CMakeFiles/HW3.dir/main.c.s

CMakeFiles/HW3.dir/main.c.o.requires:

.PHONY : CMakeFiles/HW3.dir/main.c.o.requires

CMakeFiles/HW3.dir/main.c.o.provides: CMakeFiles/HW3.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/HW3.dir/build.make CMakeFiles/HW3.dir/main.c.o.provides.build
.PHONY : CMakeFiles/HW3.dir/main.c.o.provides

CMakeFiles/HW3.dir/main.c.o.provides.build: CMakeFiles/HW3.dir/main.c.o


# Object files for target HW3
HW3_OBJECTS = \
"CMakeFiles/HW3.dir/main.c.o"

# External object files for target HW3
HW3_EXTERNAL_OBJECTS =

HW3: CMakeFiles/HW3.dir/main.c.o
HW3: CMakeFiles/HW3.dir/build.make
HW3: CMakeFiles/HW3.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable HW3"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/HW3.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/HW3.dir/build: HW3

.PHONY : CMakeFiles/HW3.dir/build

CMakeFiles/HW3.dir/requires: CMakeFiles/HW3.dir/main.c.o.requires

.PHONY : CMakeFiles/HW3.dir/requires

CMakeFiles/HW3.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/HW3.dir/cmake_clean.cmake
.PHONY : CMakeFiles/HW3.dir/clean

CMakeFiles/HW3.dir/depend:
	cd /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3 /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3 /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/cmake-build-debug /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/cmake-build-debug /Users/macboookair/CLionProjects/CSE244-SYSTEM-2017HW/HW3/cmake-build-debug/CMakeFiles/HW3.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/HW3.dir/depend

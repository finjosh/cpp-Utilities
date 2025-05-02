#* This makefile is made to work with my environment and may not work with others
#* ALL paths should start with a / and end without one

COMMA:=,
ifeq ($(OS),Windows_NT)
	SHELL=cmd.exe
	PROJECT_DIRECTORY:=$(shell cd)
	# Drive mount point directory (The drive letter)
	MOUNT_POINT:=$(firstword $(subst :,: ,${PROJECT_DIRECTORY}))
	NUM_THREADS:=$(shell echo %NUMBER_OF_PROCESSORS%)
	FIX_PATH=$(patsubst %\,%,$(subst /,\,$1))
	RM=del /q /f 
	RMDIR=rmdir /q /s 
	MKDIR=mkdir
	GET_SUB_DIRECTORIES=$(foreach path,$1,$(shell dir /b /s /ad "$(call FIX_PATH,$(PROJECT_DIRECTORY)${path})") $(PROJECT_DIRECTORY)${path})
	PATH_SEPARATOR:=\\
	IGNORE_STDERR=2>nul
	IGNORE_STDOUT:=>nul
	HOST_OS:=windows
else
	PROJECT_DIRECTORY:=$(shell pwd)
# Drive mount point directory
	MOUNT_POINT:=$(shell df -P . | tail -1 | awk '{print $$6}')
	NUM_THREADS:=$(shell nproc)
	FIX_PATH=$(patsubst %/,%,$(subst \,/,$1))
	RM=rm --preserve-root
	RMDIR=rm -r --preserve-root
	MKDIR=mkdir -p
	GET_SUB_DIRECTORIES=$(foreach path,$1,$(shell find "$(PROJECT_DIRECTORY)$(call FIX_PATH,${path})" -type d -path "*"))
	PATH_SEPARATOR:=/
	IGNORE_STDERR:=2>/dev/null
	IGNORE_STDOUT:=>/dev/null
	HOST_OS:=linux
endif
# flags to generate dependencies for all .o files
DEP_FLAGS:=-MP -MD
# Assign value only if not already set
# "make COMPILE_OS=<value> <target>" to set the COMPILE_OS to <value> before calling the make <target>
# <value> should be "windows" or "linux"
COMPILE_OS?=${HOST_OS}
ifeq ($(filter windows linux,${COMPILE_OS}),)
$(error COMPILE_OS must be 'windows' or 'linux', got '${COMPILE_OS}')
endif
#* Everything above here should not require any changes
#* Try to use COMPILE_OS instead of HOST_OS unless needed

#! "make" by default will compile in debug, to compile with release flags use "make release"
#! "make" by default compiles for the HOST_OS, "make COMPILE_OS=<value> <target>" to set the COMPILE_OS to <value> before calling the make <target>

#*****************************
#*** Cross Platform Values ***
#*****************************
MAKEFLAGS:=-j$(NUM_THREADS)
CPP_COMPILER:=g++
C_COMPILER:=gcc
DEBUG_FLAGS:=-g -D _DEBUG
RELEASE_FLAGS:=-O3
# any other compiler options (warning flags -Wextra -Wall)
GENERAL_COMPILER_FLAGS:=
EXE_NAME:=main
LIB_NAME:=utils
# Where the source files will be found (recursive)
SOURCE_DIRECTORIES=/src
# Where you don't want underlying folder/files to be included
NON_RECURSIVE_SOURCE_DIRECTORIES=/
LIB_SOURCE_DIRECTORIES=/src/Utils /src/External

LIBRARY_PREFIX:=${MOUNT_POINT}/dev-env/libraries/${COMPILE_OS}
GIT_LIB_PREFIX:=$(MOUNT_POINT)/dev-env/git-projects
GENERAL_INCLUDE_DIRECTORIES=${LIBRARY_PREFIX}/SFML-3.0.0/include ${LIBRARY_PREFIX}/TGUI-1.8.0/include\
							${PROJECT_DIRECTORY} ${PROJECT_DIRECTORY}/include
GENERAL_LIB_DIRECTORIES=${LIBRARY_PREFIX}/SFML-3.0.0/lib ${LIBRARY_PREFIX}/TGUI-1.8.0/lib

LIB_SOURCE_FILES_NO_GRAPHICS=$(call FIX_PATH,$(PROJECT_DIRECTORY)/src/Utils/CommandHandler.cpp $(PROJECT_DIRECTORY)/src/Utils/EventHelper.cpp $(PROJECT_DIRECTORY)/src/Utils/iniParser.cpp $(PROJECT_DIRECTORY)/src/Utils/Log.cpp $(PROJECT_DIRECTORY)/src/Utils/StringHelper.cpp $(PROJECT_DIRECTORY)/src/Utils/TerminatingFunction.cpp)
#*****************************
#*****************************
#*****************************

#********************************
#*** Platform Specific Values ***
#********************************
# note that directories are made during the build process if non existent
GENERAL_INCLUDE_DIRECTORIES:=$(call FIX_PATH,${GENERAL_INCLUDE_DIRECTORIES})
GENERAL_LIB_DIRECTORIES:=$(call FIX_PATH,${GENERAL_LIB_DIRECTORIES})

define windows_config
	EXE:=.exe
	INCLUDE_DIRECTORIES=$${GENERAL_INCLUDE_DIRECTORIES}
	LIB_DIRECTORIES=$${GENERAL_LIB_DIRECTORIES}
	LINKER_FLAGS:=-ltgui-s \
					-lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype \
					-lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 \
					-lsfml-audio-s -lsfml-system-s -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg \
					-lsfml-network-s -lsfml-system-s -lws2_32 \
					-lsfml-system-s -lwinmm \
					-lstdc++
	INCLUDE_FLAGS:=-D SFML_STATIC
	CPP_COMPILE_OPTIONS:=-std=c++20 -static $${GENERAL_COMPILER_FLAGS}
	C_COMPILE_OPTIONS=-std=c11 -static $${GENERAL_COMPILER_FLAGS}
	OBJECT_OUT_DIRECTORY=/bin/windows
	LIB_OUT_DIRECTORY=/lib/windows
	LIB_EXTENSION:=.a
	CREATE_LIB:=ar rcs
	LIB_COMPILE_FLAGS:=
endef

# any values that need to be changed specifically when compiling for windows on linux
define windows_via_linux_config
	CPP_COMPILER:=x86_64-w64-mingw32-g++
	C_COMPILER:=x86_64-w64-mingw32-gcc
	CREATE_LIB:=x86_64-w64-mingw32-ar rcs
endef

define linux_config
	EXE:=
	INCLUDE_DIRECTORIES=${GENERAL_INCLUDE_DIRECTORIES} /usr/include
	LIB_DIRECTORIES=$${GENERAL_LIB_DIRECTORIES} /lib
	# LIBS_SEARCH_PATHS is where the program will look for the shared libraries at runtime
	# LIB_DIRECTORIES is added here for simple testing
	LIBS_SEARCH_PATHS:=./ ./lib \
						$${LIB_DIRECTORIES}
	LINKER_FLAGS:=-ltgui \
					-lsfml-graphics -lsfml-window -lsfml-system \
					-lsfml-window -lsfml-system \
					-lsfml-audio -lsfml-system \
					-lsfml-network \
					-lsfml-system \
					-lstdc++ $$(patsubst %,-Wl$${COMMA}-rpath$${COMMA}%,$${LIBS_SEARCH_PATHS})
	INCLUDE_FLAGS:=
	CPP_COMPILE_OPTIONS:=-std=c++20 $${GENERAL_COMPILER_FLAGS}
	C_COMPILE_OPTIONS=-std=c11 $${GENERAL_COMPILER_FLAGS}
	OBJECT_OUT_DIRECTORY=/bin/linux
	LIB_OUT_DIRECTORY=/lib/linux
	LIB_EXTENSION:=.so
	CREATE_LIB:=g++ -shared -o
	LIB_COMPILE_FLAGS:=-fPIC
endef
#********************************
#********************************
#********************************

#* The following should not require any changes
# Apply configuration based on COMPILE_OS and HOST_OS
ifeq (${HOST_OS},windows)
ifeq (${COMPILE_OS},windows)
$(eval ${windows_config})
else
$(error "Compiling for linux on windows is not supported, try using WSL")
endif
else
ifeq (${COMPILE_OS},windows)
$(eval ${windows_config})
$(eval ${windows_via_linux_config})
else
$(eval ${linux_config})
endif
endif

OBJECT_OUT_DIRECTORY:=$(call FIX_PATH, ${OBJECT_OUT_DIRECTORY})
LIB_OUT_DIRECTORY:=$(call FIX_PATH, ${LIB_OUT_DIRECTORY})
LIB_OBJECT_FILES_NO_GRAPHICS:=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.cpp,%.o,${LIB_SOURCE_FILES_NO_GRAPHICS}))
SOURCE_DIRECTORIES:=$(call FIX_PATH, ${SOURCE_DIRECTORIES})
NON_RECURSIVE_SOURCE_DIRECTORIES:=$(call FIX_PATH, ${NON_RECURSIVE_SOURCE_DIRECTORIES})
OBJECT_OUT_DIRECTORY:=$(call FIX_PATH, ${OBJECT_OUT_DIRECTORY})
LIB_OUT_DIRECTORY:=$(call FIX_PATH, ${LIB_OUT_DIRECTORY})
LIB_SOURCE_DIRECTORIES:=$(call FIX_PATH, ${LIB_SOURCE_DIRECTORIES})
INCLUDE_DIRECTORIES:=$(call FIX_PATH,$(addprefix -I ,${INCLUDE_DIRECTORIES}))
LIB_DIRECTORIES:=$(call FIX_PATH,$(addprefix -L ,${LIB_DIRECTORIES}))

EVERY_SOURCE:=$(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(wildcard ${Directory}/*.cpp)) $(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(wildcard ${Directory}/*.c))
# # include the dependencies
-include $(patsubst %.cpp,%.d,${EVERY_SOURCE})

.PHONY = all info clean libs libs-r libs-d run run-r clean-objects clean-exe clean-libs debug release\
		windows-debug windows-release windows-run windows-run-r windows-libs windows-libs-r windows-libs-d\
		windows-clean windows-clean-objects windows-clean-exe windows-clean-libs windows-info help

all: debug

run: debug
	./${EXE_NAME}${EXE}

run-r: release
	./${EXE_NAME}${EXE}

# these have a ${PATH_SEPARATOR} at the end so that they follow the target rules
BIN_DIRECTORIES=$(addsuffix ${PATH_SEPARATOR},$(foreach dir,${EXPANDED_SOURCE_DIRECTORIES},$(patsubst $(PROJECT_DIRECTORY)%,$(PROJECT_DIRECTORY)$(OBJECT_OUT_DIRECTORY)%,$(dir))))
EXPANDED_SOURCE_DIRECTORIES:=$(PROJECT_DIRECTORY)$(NON_RECURSIVE_SOURCE_DIRECTORIES) $(call GET_SUB_DIRECTORIES,${SOURCE_DIRECTORIES})
SOURCE_FILES:=$(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(call FIX_PATH,$(wildcard ${Directory}/*.cpp)))
C_SOURCE_FILES:=$(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(call FIX_PATH,$(wildcard ${Directory}/*.c)))
OBJECT_FILES:=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.cpp,%.o,${SOURCE_FILES})) \
				$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.c,%.o,${C_SOURCE_FILES}))

# these have a ${PATH_SEPARATOR} at the end so that they follow the target rules
LIB_BIN_DIRECTORIES=$(addsuffix ${PATH_SEPARATOR},$(foreach dir,${LIB_EXPANDED_SOURCE_DIRECTORIES},$(patsubst $(PROJECT_DIRECTORY)%,$(PROJECT_DIRECTORY)$(OBJECT_OUT_DIRECTORY)%,$(dir))))
LIB_EXPANDED_SOURCE_DIRECTORIES:=$(call GET_SUB_DIRECTORIES,${LIB_SOURCE_DIRECTORIES})
LIB_SOURCE_FILES:=$(foreach Directory,${LIB_EXPANDED_SOURCE_DIRECTORIES},$(call FIX_PATH,$(wildcard ${Directory}/*.cpp)))
LIB_C_SOURCE:=$(foreach Directory,${LIB_EXPANDED_SOURCE_DIRECTORIES},$(call FIX_PATH,$(wildcard ${Directory}/*.c)))
LIB_OBJECT_FILES:=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.cpp,%.o,${LIB_SOURCE_FILES})) \
					$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.c,%.o,${C_LIB_SOURCE_FILES}))

# the case that we are compiling for windows on linux
ifeq (${HOST_OS},linux)
windows: windows-debug
windows-debug:
	$(MAKE) COMPILE_OS=windows debug
windows-release:
	$(MAKE) COMPILE_OS=windows release
windows-run:
	$(MAKE) COMPILE_OS=windows run
windows-run-r:
	$(MAKE) COMPILE_OS=windows run-r
windows-libs:
	$(MAKE) COMPILE_OS=windows libs
windows-libs-r:
	$(MAKE) COMPILE_OS=windows libs-r
windows-libs-d:
	$(MAKE) COMPILE_OS=windows libs-d
windows-clean:
	$(MAKE) COMPILE_OS=windows clean
windows-clean-objects:
	$(MAKE) COMPILE_OS=windows clean-objects
windows-clean-exe:
	$(MAKE) COMPILE_OS=windows clean-exe
windows-clean-libs:
	$(MAKE) COMPILE_OS=windows clean-libs
windows-info:
	$(MAKE) COMPILE_OS=windows info
endif

debug: CURRENT_FLAGS=${DEBUG_FLAGS}
debug: ${BIN_DIRECTORIES} ${OBJECT_FILES}
	${CPP_COMPILER} ${CPP_COMPILE_OPTIONS} ${C_COMPILE_OPTIONS} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} -o ${EXE_NAME} ${OBJECT_FILES} ${LIB_DIRECTORIES} ${LINKER_FLAGS}

release: CURRENT_FLAGS=${RELEASE_FLAGS}
release: ${BIN_DIRECTORIES} ${OBJECT_FILES}
	${CPP_COMPILER} ${CPP_COMPILE_OPTIONS} ${C_COMPILE_OPTIONS} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} -o ${EXE_NAME} ${OBJECT_FILES} ${LIB_DIRECTORIES} ${LINKER_FLAGS}

${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%.o:${PROJECT_DIRECTORY}%.cpp
	${CPP_COMPILER} ${CPP_COMPILE_OPTIONS} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} ${DEP_FLAGS} -c -o ${@} ${<}

${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%.o:${PROJECT_DIRECTORY}%.c
	${C_COMPILER} ${C_COMPILE_OPTIONS} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} ${DEP_FLAGS} -c -o ${@} ${<}

libs:
	make clean-objects && make libs-r && make clean-objects && make libs-d

# build the lib with the same compile options
libs-r: CURRENT_FLAGS = ${RELEASE_FLAGS} ${LIB_COMPILE_FLAGS}
libs-r: ${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}${PATH_SEPARATOR} ${LIB_BIN_DIRECTORIES} ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}/lib${LIB_NAME}${LIB_EXTENSION}) ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}/${LIB_OUT_DIRECTORY}/lib${LIB_NAME}-no-graphics${LIB_EXTENSION}) ${LIB_OBJECT_FILES_NO_GRAPHICS}
	@echo Release Libs created

libs-d: CURRENT_FLAGS = ${DEBUG_FLAGS} ${LIB_COMPILE_FLAGS}
libs-d: ${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}${PATH_SEPARATOR} ${LIB_BIN_DIRECTORIES} ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}/lib${LIB_NAME}-d${LIB_EXTENSION}) ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}/${LIB_OUT_DIRECTORY}/lib${LIB_NAME}-no-graphics-d${LIB_EXTENSION}) ${LIB_OBJECT_FILES_NO_GRAPHICS}
	@echo Debug Libs created

${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%${PATH_SEPARATOR}:
	-@${MKDIR} ${@}
	@echo Created Object Directory: ${@}

${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}${PATH_SEPARATOR}:
	-@${MKDIR} ${@}
	@echo Created Object Directory: ${@}

${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}${PATH_SEPARATOR}:
	-@${MKDIR} ${@}
	@echo Created Lib Directory: ${@}

clean: clean-objects clean-exe clean-libs
	@echo Finished Cleaning

clean-objects:
	-@${RMDIR} ${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY} ${IGNORE_STDOUT} ${IGNORE_STDERR}
	@echo Cleaned Objects

clean-exe:
	-@${RM} ${PROJECT_DIRECTORY}${PATH_SEPARATOR}${EXE_NAME}${EXE} ${IGNORE_STDOUT} ${IGNORE_STDERR}
	@echo Cleaned Executable

clean-libs:
	-@${RMDIR} ${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY} ${IGNORE_STDOUT} ${IGNORE_STDERR}
	@echo Cleaned Libs

info:
	@echo -----------------------------------------
	@echo __LIB SPECIFIC FILES__
	@echo Source Directories: ${LIB_EXPANDED_SOURCE_DIRECTORIES}
	@echo Bin Directories: ${LIB_BIN_DIRECTORIES}
	@echo Sources: ${LIB_SOURCE_FILES}
	@echo Objects: ${LIB_OBJECT_FILES}
	@echo -----------------------------------------
	@echo -----------------------------------------
	@echo __EXECUTABLE SPECIFIC FILES__
	@echo Source Directories: ${EXPANDED_SOURCE_DIRECTORIES}
	@echo Bin Directories: ${BIN_DIRECTORIES}
	@echo Sources: ${SOURCE_FILES}
	@echo Objects: ${OBJECT_FILES}
	@echo -----------------------------------------
	@echo -----------------------------------------
	@echo Debug Flags: $(DEBUG_FLAGS)
	@echo Release Flags: $(RELEASE_FLAGS)
	@echo C++ Compiler Options \(Platform Specific\): $(CPP_COMPILE_OPTIONS)
	@echo C Compiler Options \(Platform Specific\): $(C_COMPILE_OPTIONS)
	@echo Linker Flags \(Platform Specific\): $(LINKER_FLAGS)
	@echo Include Directories: $(INCLUDE_DIRECTORIES)
	@echo Library Directories: $(LIB_DIRECTORIES)
	@echo Bin Directory: $(OBJECT_OUT_DIRECTORY)
	@echo Lib Directory: $(LIB_OUT_DIRECTORY)
	@echo -----------------------------------------
	@echo -----------------------------------------
	@echo Host OS \(OS the script is running on\): ${HOST_OS}
	@echo Compiled OS \(OS being compiled for\): ${COMPILE_OS}
	@echo Working Directory: $(PROJECT_DIRECTORY)
	@echo Number of Threads: $(NUM_THREADS)
	@echo C++ Compiler: $(CPP_COMPILER)
	@echo C Compiler: $(C_COMPILER)
	@echo Mount Point: $(MOUNT_POINT)
	@echo -----------------------------------------
ifneq ($(COMPILE_OS),windows)
	@echo -----------------------------------------
	@echo Required Shared Libs \(if ${EXE_NAME} exists\):
	@echo $(shell ldd ${EXE_NAME} | grep -o '/[^ ]*')
	@echo ${LIBS_SEARCH_PATHS}
	@echo $(patsubst %,-Wl${COMMA}-rpath${COMMA}%,/ ./lib ${GENERAL_LIB_DIRECTORIES})
	@echo ${GENERAL_LIB_DIRECTORIES}
	@echo ${LINKER_FLAGS}
	@echo -----------------------------------------
endif 

help:
	@echo -----------------------------------------
	@echo ------------- Makefile Help -------------
	@echo -----------------------------------------
	@echo ---------------- Common -----------------
	@echo -----------------------------------------
	@echo make: Build the project with debug flags \(same as "make debug"\)
	@echo make debug: Build the project with debug flags
	@echo make release: Build the project with release flags
	@echo make run: Build the project with debug flags and run it \(Does not clean first\)
	@echo make run-r: Build the project with release flags and run it \(Does not clean first\)
	@echo make clean: Clean the project \(Does clean-exe, clean-libs, clean-objects\)
	@echo make clean-exe: Clean the project \(remove the executable\)
	@echo make clean-libs: Clean the project \(remove the libs\)
	@echo make clean-objects: Clean the project \(remove all object files\)
	@echo make libs: Clean objects, build release libs, clean objects, build debug libs
	@echo make libs-r: Build the project with release flags and create the libs \(Does not clean first\)
	@echo make libs-d: Build the project with debug flags and create the libs \(Does not clean first\)
	@echo make info: Print the current configuration and all the source files
ifeq (${HOST_OS},linux)
	@echo -----------------------------------------
	@echo -------- Windows Build Via Linux --------
	@echo -----------------------------------------
	@echo make windows: Build the project with debug flags for windows \(same as "make windows-debug" just shorter\)
	@echo make windows-debug: Build the project with debug flags for windows
	@echo make windows-release: Build the project with release flags for windows
	@echo make windows-run: Build the project with debug flags for windows and run it \(Does not clean first\)
	@echo make windows-run-r: Build the project with release flags for windows and run it \(Does not clean first\)
	@echo make windows-libs: Clean objects, build release libs, clean objects, build debug libs \(For windows via the defined compiler for Linux\)
	@echo make windows-libs-r: Build the project with release flags for windows and create the libs
	@echo make windows-libs-d: Build the project with debug flags for windows and create the libs
	@echo make windows-clean: Clean the project \(Does clean-exe, clean-libs, clean-objects\)
	@echo make windows-clean-exe: Clean the project \(remove the executable\)
	@echo make windows-clean-libs: Clean the project \(remove the libs\)
	@echo make windows-clean-objects: Clean the project \(remove all object files\)
	@echo make windows-info: Print the current configuration and all the source files
endif
	@echo -----------------------------------------

#* This makefile is made to work with my environment and may not work with others
#* Try to use COMPILE_OS instead of HOST_OS unless needed
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
# debug if not already set
# should be either "debug" or "release"
BUILD_RELEASE?=debug
ifeq ($(filter debug release,${BUILD_RELEASE}),)
$(error COMPILE_TYPE must be 'debug' or 'release', got '${BUILD_RELEASE}')
endif
# executable if not already set
# should be either "executable" or "library"
BUILD_TYPE?=executable
ifeq ($(filter executable library,${BUILD_TYPE}),)
$(error BUILD_TYPE must be 'executable' or 'library', got '${BUILD_TYPE}')
endif

#! "make" by default compiles for the HOST_OS, "make COMPILE_OS=<value> <target>" to set the COMPILE_OS to <value> before calling the make <target>

-include make-env-vars.mk

$(eval ${executable_config})
ifeq (${COMPILE_TYPE},library)
$(eval ${lib_config})
endif

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

OBJECT_OUT_DIRECTORY:=$(call FIX_PATH,${OBJECT_OUT_DIRECTORY})
PROJECT_OUT_DIRECTORY:=$(call FIX_PATH,${PROJECT_OUT_DIRECTORY})
OBJECT_FILES_NO_GRAPHICS:=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}$(call FIX_PATH,${OBJECT_OUT_DIRECTORY})%,$(patsubst %.cpp,%.o,$(call FIX_PATH,${LIB_SOURCE_FILES_NO_GRAPHICS})))
SOURCE_DIRECTORIES:=$(call FIX_PATH,${SOURCE_DIRECTORIES})
NON_RECURSIVE_SOURCE_DIRECTORIES:=$(call FIX_PATH,${NON_RECURSIVE_SOURCE_DIRECTORIES})
INCLUDE_DIRECTORIES:=$(call FIX_PATH,$(addprefix -I ,${INCLUDE_DIRECTORIES}))
LIB_DIRECTORIES:=$(call FIX_PATH,$(addprefix -L ,${LIB_DIRECTORIES}))

# these have a ${PATH_SEPARATOR} at the end so that they follow the target rules
BIN_DIRECTORIES=$(addsuffix ${PATH_SEPARATOR},$(patsubst $(PROJECT_DIRECTORY)%,$(PROJECT_DIRECTORY)$(OBJECT_OUT_DIRECTORY)%,${EXPANDED_SOURCE_DIRECTORIES}))
EXPANDED_SOURCE_DIRECTORIES:=$(PROJECT_DIRECTORY)$(NON_RECURSIVE_SOURCE_DIRECTORIES) $(call GET_SUB_DIRECTORIES,${SOURCE_DIRECTORIES})
SOURCE_FILES:=$(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(call FIX_PATH,$(wildcard ${Directory}/*.cpp)))
C_SOURCE_FILES:=$(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(call FIX_PATH,$(wildcard ${Directory}/*.c)))
OBJECT_FILES:=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}$(call FIX_PATH,${OBJECT_OUT_DIRECTORY})%,$(patsubst %.cpp,%.o,${SOURCE_FILES})) \
				$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}$(call FIX_PATH,${OBJECT_OUT_DIRECTORY})%,$(patsubst %.c,%.o,${C_SOURCE_FILES}))

EVERY_OBJECT:=${OBJECT_FILES} ${LIB_OBJECT_FILES}

.PHONY = all info clean libs libs-r libs-d run run-r clean-objects clean-exe clean-libs debug release\
		 windows-debug windows-release windows-run windows-run-r windows-libs windows-libs-r windows-libs-d\
		 windows-clean windows-clean-objects windows-clean-exe windows-clean-libs windows-info help

all: debug

run: debug
	./${PROJECT_NAME}${EXE}

run-r: release
	./${PROJECT_NAME}${EXE}

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
debug: OBJECT_OUT_DIRECTORY:=${OBJECT_OUT_DIRECTORY}/debug
debug: ${BIN_DIRECTORIES} ${OBJECT_FILES}
	${CPP_COMPILER} ${CPP_COMPILE_OPTIONS} ${C_COMPILE_OPTIONS} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} -o ${PROJECT_NAME} ${OBJECT_FILES} ${LIB_DIRECTORIES} ${LINKER_FLAGS}

release: CURRENT_FLAGS=${RELEASE_FLAGS}
release: OBJECT_OUT_DIRECTORY:=${OBJECT_OUT_DIRECTORY}/release
release: ${BIN_DIRECTORIES} ${OBJECT_FILES}
	${CPP_COMPILER} ${CPP_COMPILE_OPTIONS} ${C_COMPILE_OPTIONS} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} -o ${PROJECT_NAME} ${OBJECT_FILES} ${LIB_DIRECTORIES} ${LINKER_FLAGS}

${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%.o:${PROJECT_DIRECTORY}%.cpp
	${CPP_COMPILER} ${CPP_COMPILE_OPTIONS} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} ${DEP_FLAGS} -c -o ${@} ${<}

${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%.o:${PROJECT_DIRECTORY}%.c
	${C_COMPILER} ${C_COMPILE_OPTIONS} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} ${DEP_FLAGS} -c -o ${@} ${<}

libs:
	make clean-objects && make libs-r && make clean-objects && make libs-d

# build the lib with the same compile options
libs-r: CURRENT_FLAGS=${RELEASE_FLAGS} ${LIB_COMPILE_FLAGS}
libs-r: OBJECT_OUT_DIRECTORY:=${OBJECT_OUT_DIRECTORY}/debug
libs-r: ${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}${PATH_SEPARATOR} ${LIB_BIN_DIRECTORIES} ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}/lib${PROJECT_NAME}${LIB_EXTENSION}) ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}/${LIB_OUT_DIRECTORY}/lib${PROJECT_NAME}-no-graphics${LIB_EXTENSION}) ${LIB_OBJECT_FILES_NO_GRAPHICS}
	@echo Release Libs created

libs-d: CURRENT_FLAGS=${DEBUG_FLAGS} ${LIB_COMPILE_FLAGS}
libs-d: OBJECT_OUT_DIRECTORY:=${OBJECT_OUT_DIRECTORY}/release
libs-d: ${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}${PATH_SEPARATOR} ${LIB_BIN_DIRECTORIES} ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}/lib${PROJECT_NAME}-d${LIB_EXTENSION}) ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}/${LIB_OUT_DIRECTORY}/lib${PROJECT_NAME}-no-graphics-d${LIB_EXTENSION}) ${LIB_OBJECT_FILES_NO_GRAPHICS}
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
	-@${RM} ${PROJECT_DIRECTORY}${PATH_SEPARATOR}${PROJECT_NAME}${EXE} ${IGNORE_STDOUT} ${IGNORE_STDERR}
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
	@echo Required Shared Libs \(if ${PROJECT_NAME} exists\):
	@echo $(shell ldd ${PROJECT_NAME} | grep -o '/[^ ]*')
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

# include the dependencies
-include $(patsubst %.o,%.d,${EVERY_OBJECT})
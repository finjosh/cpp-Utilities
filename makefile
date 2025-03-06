#* This makefile is made to work with my environment and may not work with others
#* ALL paths should be based on the makefile directory as the root and start with a / and end without one

ifeq ($(OS),Windows_NT)
	PROJECT_DIRECTORY:=$(shell cd)
	NUM_THREADS:=$(shell echo %NUMBER_OF_PROCESSORS%)
	FIX_PATH=$(patsubst %\,%,$(subst /,\,$1))
	RM:=del /q /f
	RMDIR:=rmdir /S /Q
	MKDIR:=mkdir
	GET_SUB_DIRECTORIES=$(shell dir $(call FIX_PATH,$1) /s /b /ad) $(call FIX_PATH,${PROJECT_DIRECTORY}/${SRC})
else
	PROJECT_DIRECTORY:=$(shell pwd)
	NUM_THREADS:=$(shell nproc)
	FIX_PATH=$(patsubst %/,%,$(subst \,/,$1))
	RM:=rm -r --preserve-root
	RMDIR:=rm -r --preserve-root
	MKDIR:=mkdir -p
	GET_SUB_DIRECTORIES=$(foreach path,$1,$(shell find "$(PROJECT_DIRECTORY)$(call FIX_PATH,${path})" -type d -path "*"))
endif
# flags to generate dependencies for all .o files
DEPFLAGS:=-MP -MD
# include the dependencies
-include ${DEPFILES}

#! "make" by default will compile in debug to compile with release flags use "make release"

#*** Cross Platform Values ***
#*****************************
MAKEFLAGS:=-j$(NUM_THREADS)
CPP_COMPILER:=g++
DEBUG_FLAGS = -g -D _DEBUG
RELEASE_FLAGS = -O3
# any other compiler options (warning flags -Wextra -Wall)
GENERAL_COMPILER_FLAGS:=-std=c++20
EXE_NAME:=main
LIB_NAME:=utils
# Where the source files will be found (recursive)
SOURCE_DIRECTORIES:=/src
# Where you don't want underlying folder/files to be included
NON_RECURSIVE_SOURCE_DIRECTORIES:=/
# Where the object files will be saved
# Will be generated if non existent
OBJECT_OUT_DIRECTORY:=/bin
# Will be generated if non existent
LIB_DIRECTORY:=/lib
LIB_SOURCE_DIRECTORIES:=/src/Utils /src/External
LIB_NON_RECURSIVE_SOURCE_DIRECTORIES:=
#*****************************
#*****************************

#*** Platform Specific Values ***
#********************************
ifeq ($(OS),Windows_NT)
	EXE:=.exe
	INCLUDES:=$(call FIX_PATH,${WINDOWS_INCLUDE_DIRS}) ${INCLUDE_FLAGS} -I ${PROJECT_DIR} -I ${PROJECT_DIR}/include
	LIBS:=$(call FIX_PATH,${WINDOWS_LIB_DIRS})
	LINKER_FLAGS:=-ltgui-s \
					-lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype \
					-lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 \
					-lsfml-audio-s -lsfml-system-s -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg \
					-lsfml-network-s -lsfml-system-s -lws2_32 \
					-lsfml-system-s -lwinmm \
					-lstdc++ 
	INCLUDE_FLAGS:=-D SFML_STATIC
	COMPILE_OPTIONS:=${GENERAL_COMPILER_FLAGS} -static
	
	LIB_EXTENSION:=.a
	CREATE_LIB:=ar rcs 
	LIB_COMPILE_FLAGS:=
else
	EXE:=
	INCLUDES:=$(call FIX_PATH,${LINUX_INCLUDE_DIRS}) ${INCLUDE_FLAGS} -I ${PROJECT_DIR} -I ${PROJECT_DIR}/include
	LIBS:=$(call FIX_PATH,${LINUX_LIB_DIRS})
	LINKER_FLAGS:=-ltgui \
					-lsfml-graphics -lsfml-window -lsfml-system \
					-lsfml-window -lsfml-system \
					-lsfml-audio -lsfml-system \
					-lsfml-network \
					-lsfml-system \
					-lstdc++
	INCLUDE_FLAGS:=
	COMPILE_OPTIONS:=${GENERAL_COMPILER_FLAGS} 

	LIB_EXTENSION:=.so
	CREATE_LIB:=g++ -shared -o
	LIB_COMPILE_FLAGS:=-fPIC
endif
#********************************
#********************************

info:
	@echo "Working Directory: $(PROJECT_DIRECTORY)"
	@echo "Number of Threads: $(NUM_THREADS)"
	@echo "Compiler: $(CPP_COMPILER)"
	@echo "Debug Flags: $(DEBUG_FLAGS)"
	@echo "Release Flags: $(RELEASE_FLAGS)"
	@echo "Compiler Options (Platform Specific): $(COMPILE_OPTIONS)"
	@echo "Linker Flags (Platform Specific): $(LINKER_FLAGS)"
	@echo ""
	$(eval EXPANDED_SOURCE_DIRECTORIES=$(call GET_SUB_DIRECTORIES,${SOURCE_DIRECTORIES}) $(call FIX_PATH,$(PROJECT_DIRECTORY)$(NON_RECURSIVE_SOURCE_DIRECTORIES)))
	@echo "Source Directories: ${EXPANDED_SOURCE_DIRECTORIES}"
	$(eval SOURCE_FILES=$(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(wildcard ${Directory}/*.cpp)))
	@echo "Sources: ${SOURCE_FILES}"
	$(eval OBJECT_FILES=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.cpp,%.o,${SOURCE_FILES})))
	@echo "Objects: ${OBJECT_FILES}"
	@echo ""
	@echo "__LIB SPECIFIC FILES__"
	$(eval EXPANDED_SOURCE_DIRECTORIES=$(call GET_SUB_DIRECTORIES,${LIB_SOURCE_DIRECTORIES}))
	@echo "Source Directories: ${EXPANDED_SOURCE_DIRECTORIES}"
	$(eval SOURCE_FILES=$(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(wildcard ${Directory}/*.cpp)))
	@echo "Sources: ${SOURCE_FILES}"
	$(eval OBJECT_FILES=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.cpp,%.o,${SOURCE_FILES})))
	@echo "Objects: ${OBJECT_FILES}"
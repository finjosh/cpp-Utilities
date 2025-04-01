#* This makefile is made to work with my environment and may not work with others
#* ALL paths should start with a / and end without one

ifeq ($(OS),Windows_NT)
	PROJECT_DIRECTORY:=$(shell echo %CD%)
	NUM_THREADS:=$(shell echo %NUMBER_OF_PROCESSORS%)
	FIX_PATH=$(patsubst %\,%,$(subst /,\,$1))
	RM:=powershell -Command "Remove-Item -Force -ErrorAction SilentlyContinue"
	RMDIR:=powershell -Command "Remove-Item -Force -Recurse -ErrorAction SilentlyContinue"
	MKDIR:=powershell -Command "New-Item -ItemType Directory -Force -Path"
	GET_SUB_DIRECTORIES=$(foreach path,$1,$(shell powershell -Command "Get-ChildItem -Path '$(PROJECT_DIRECTORY)$(call FIX_PATH,${path})' -Directory -Recurse | Select-Object -ExpandProperty FullName"))
	PATH_SEPARATOR:=\\
else
	PROJECT_DIRECTORY:=$(shell pwd)
	NUM_THREADS:=$(shell nproc)
	FIX_PATH=$(patsubst %/,%,$(subst \,/,$1))
	RM:=rm --preserve-root
	RMDIR:=rm -r --preserve-root
	MKDIR:=mkdir -p
	GET_SUB_DIRECTORIES=$(foreach path,$1,$(shell find "$(PROJECT_DIRECTORY)$(call FIX_PATH,${path})" -type d -path "*"))
	PATH_SEPARATOR:=/
endif
# flags to generate dependencies for all .o files
DEP_FLAGS:=-MP -MD

#! "make" by default will compile in debug to compile with release flags use "make release"

#*** Cross Platform Values ***
#*****************************
MAKEFLAGS:=-j$(NUM_THREADS)
CPP_COMPILER:=g++
DEBUG_FLAGS:=-g -D _DEBUG
RELEASE_FLAGS:=-O3
# any other compiler options (warning flags -Wextra -Wall)
GENERAL_COMPILER_FLAGS:=-std=c++20
EXE_NAME:=main
LIB_NAME:=utils
# Where the source files will be found (recursive)
SOURCE_DIRECTORIES=/src
# Where you don't want underlying folder/files to be included
NON_RECURSIVE_SOURCE_DIRECTORIES=/
LIB_SOURCE_DIRECTORIES=/src/Utils /src/External
#*****************************
#*****************************

#*** Platform Specific Values ***
#********************************
ifeq ($(OS),Windows_NT)
	EXE:=.exe
	INCLUDE_DIRECTORIES=\VSCodeFolder\Libraries\SFML-3.0.0\include \VSCodeFolder\Libraries\TGUI-1.7\include ${PROJECT_DIRECTORY} ${PROJECT_DIRECTORY}\include
	LIB_DIRECTORIES=\VSCodeFolder\Libraries\SFML-3.0.0\lib \VSCodeFolder\Libraries\TGUI-1.7\lib
	LIB_SOURCE_FILES_NO_GRAPHICS=$(PROJECT_DIRECTORY)\src\Utils\CommandHandler.cpp $(PROJECT_DIRECTORY)\src\Utils\EventHelper.cpp $(PROJECT_DIRECTORY)\src\Utils\iniParser.cpp $(PROJECT_DIRECTORY)\src\Utils\Log.cpp $(PROJECT_DIRECTORY)\src\Utils\StringHelper.cpp $(PROJECT_DIRECTORY)\src\Utils\TerminatingFunction.cpp
	LIB_DIRECTORIES_NO_GRAPHICS=
	LINKER_FLAGS:=-ltgui-s \
					-lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype \
					-lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 \
					-lsfml-audio-s -lsfml-system-s -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg \
					-lsfml-network-s -lsfml-system-s -lws2_32 \
					-lsfml-system-s -lwinmm \
					-lstdc++ 
	LINKER_FLAGS_NO_GRAPHICS:=
	INCLUDE_FLAGS_NO_GRAPHICS:=
	INCLUDE_FLAGS:=-D SFML_STATIC
	COMPILE_OPTIONS:=${GENERAL_COMPILER_FLAGS} -static

# Where the object files will be saved
# Will be generated if non existent
	OBJECT_OUT_DIRECTORY=/bin/windows
# Will be generated if non existent
	LIB_OUT_DIRECTORY=/lib/windows
	LIB_EXTENSION:=.a
	CREATE_LIB:=ar rcs 
	LIB_COMPILE_FLAGS:=
else
	EXE:=
	INCLUDE_DIRECTORIES=/usr/include ${PROJECT_DIRECTORY} ${PROJECT_DIRECTORY}/include
	LIB_DIRECTORIES=/usr/lib
	LIB_SOURCE_FILES_NO_GRAPHICS=$(PROJECT_DIRECTORY)/src/Utils/CommandHandler.cpp $(PROJECT_DIRECTORY)/src/Utils/EventHelper.cpp $(PROJECT_DIRECTORY)/src/Utils/iniParser.cpp $(PROJECT_DIRECTORY)/src/Utils/Log.cpp $(PROJECT_DIRECTORY)/src/Utils/StringHelper.cpp $(PROJECT_DIRECTORY)/src/Utils/TerminatingFunction.cpp
	LIB_DIRECTORIES_NO_GRAPHICS=
	LINKER_FLAGS:=-ltgui \
					-lsfml-graphics -lsfml-window -lsfml-system \
					-lsfml-window -lsfml-system \
					-lsfml-audio -lsfml-system \
					-lsfml-network \
					-lsfml-system \
					-lstdc++
	LINKER_FLAGS_NO_GRAPHICS:=
	INCLUDE_FLAGS_NO_GRAPHICS:=
	INCLUDE_FLAGS:=
	COMPILE_OPTIONS:=${GENERAL_COMPILER_FLAGS} 

# Where the object files will be saved
# Will be generated if non existent
	OBJECT_OUT_DIRECTORY=/bin/linux
# Will be generated if non existent
	LIB_OUT_DIRECTORY=/lib/linux
	LIB_EXTENSION:=.so
	CREATE_LIB:=g++ -shared -o
	LIB_COMPILE_FLAGS:=-fPIC
endif
#********************************
#********************************

OBJECT_OUT_DIRECTORY:=$(call FIX_PATH, ${OBJECT_OUT_DIRECTORY})
LIB_OUT_DIRECTORY:=$(call FIX_PATH, ${LIB_OUT_DIRECTORY})
SOURCE_DIRECTORIES:=$(call FIX_PATH, ${SOURCE_DIRECTORIES})
NON_RECURSIVE_SOURCE_DIRECTORIES:=$(call FIX_PATH, ${NON_RECURSIVE_SOURCE_DIRECTORIES})
OBJECT_OUT_DIRECTORY:=$(call FIX_PATH, ${OBJECT_OUT_DIRECTORY})
LIB_OUT_DIRECTORY:=$(call FIX_PATH, ${LIB_OUT_DIRECTORY})
LIB_SOURCE_DIRECTORIES:=$(call FIX_PATH, ${LIB_SOURCE_DIRECTORIES})
INCLUDE_DIRECTORIES:=$(call FIX_PATH,$(addprefix -I ,${INCLUDE_DIRECTORIES}))
LIB_DIRECTORIES:=$(call FIX_PATH,$(addprefix -L ,${LIB_DIRECTORIES}))

EVERY_SOURCE:=$(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(wildcard ${Directory}/*.cpp)) $(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(wildcard ${Directory}/*.cpp))
# # include the dependencies
-include $(patsubst %.cpp,%.d,${EVERY_SOURCE})

.PHONY = all info clean libs libs-r libs-d run clean_object clean_exe clean_libs debug release check_install_dirs install_clean install

all: debug
run: debug
	./${EXE_NAME}${EXE}

run-r: release
	./${EXE_NAME}${EXE}

# these have a ${PATH_SEPARATOR} at the end so that they follow the target rules
BIN_DIRECTORIES=$(addsuffix ${PATH_SEPARATOR},$(foreach dir,${EXPANDED_SOURCE_DIRECTORIES},$(patsubst $(PROJECT_DIRECTORY)%,$(PROJECT_DIRECTORY)$(OBJECT_OUT_DIRECTORY)%,$(dir))))
EXPANDED_SOURCE_DIRECTORIES=$(PROJECT_DIRECTORY)$(NON_RECURSIVE_SOURCE_DIRECTORIES) $(call GET_SUB_DIRECTORIES,${SOURCE_DIRECTORIES})
SOURCE_FILES=$(foreach Directory,${EXPANDED_SOURCE_DIRECTORIES},$(wildcard ${Directory}/*.cpp))
OBJECT_FILES=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.cpp,%.o,${SOURCE_FILES}))

# these have a ${PATH_SEPARATOR} at the end so that they follow the target rules
LIB_BIN_DIRECTORIES=$(addsuffix ${PATH_SEPARATOR},$(foreach dir,${LIB_EXPANDED_SOURCE_DIRECTORIES},$(patsubst $(PROJECT_DIRECTORY)%,$(PROJECT_DIRECTORY)$(OBJECT_OUT_DIRECTORY)%,$(dir))))
LIB_EXPANDED_SOURCE_DIRECTORIES=$(call GET_SUB_DIRECTORIES,${LIB_SOURCE_DIRECTORIES})
LIB_SOURCE_FILES=$(foreach Directory,${LIB_EXPANDED_SOURCE_DIRECTORIES},$(wildcard ${Directory}/*.cpp))
LIB_OBJECT_FILES=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.cpp,%.o,${LIB_SOURCE_FILES}))
LIB_OBJECT_FILES_NO_GRAPHICS=$(patsubst ${PROJECT_DIRECTORY}%,${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%,$(patsubst %.cpp,%.o,${LIB_SOURCE_FILES_NO_GRAPHICS}))

debug: CURRENT_FLAGS=${DEBUG_FLAGS} ${COMPILE_OPTIONS} 
debug: ${BIN_DIRECTORIES} ${OBJECT_FILES}
	${CPP_COMPILER} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} -o ${EXE_NAME} ${OBJECT_FILES} ${LIB_DIRECTORIES} ${LINKER_FLAGS}

release: CURRENT_FLAGS=${RELEASE_FLAGS} ${COMPILE_OPTIONS} 
release: ${BIN_DIRECTORIES} ${OBJECT_FILES}
	${CPP_COMPILER} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} -o ${EXE_NAME} ${OBJECT_FILES} ${LIB_DIRECTORIES} ${LINKER_FLAGS}

${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%.o:${PROJECT_DIRECTORY}%.cpp
	${CPP_COMPILER} ${CURRENT_FLAGS} ${INCLUDE_DIRECTORIES} ${INCLUDE_FLAGS} ${DEP_FLAGS} -c -o ${@} ${<}

libs: 
	make libs-r 
	make libs-d

# build the lib with the same compile options
libs-r: CURRENT_FLAGS = ${COMPILE_OPTIONS} ${RELEASE_FLAGS} ${LIB_COMPILE_FLAGS}
libs-r: clean_object ${LIB_BIN_DIRECTORIES} ${LIB_OBJECT_FILES} ${LIB_OBJECT_FILES_NO_GRAPHICS}
	$(shell ${MKDIR} ${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY})
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}/lib${LIB_NAME}${LIB_EXTENSION}) ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}/${LIB_OUT_DIRECTORY}/lib${LIB_NAME}_no_graphics${LIB_EXTENSION}) ${LIB_OBJECT_FILES_NO_GRAPHICS}
	@echo Release Libs created

libs-d: CURRENT_FLAGS = ${COMPILE_OPTIONS} ${DEBUG_FLAGS} ${LIB_COMPILE_FLAGS}
libs-d: clean_object ${LIB_BIN_DIRECTORIES} ${LIB_OBJECT_FILES} ${LIB_OBJECT_FILES_NO_GRAPHICS}
	$(shell ${MKDIR} ${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY})
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}/lib${LIB_NAME}-d${LIB_EXTENSION}) ${LIB_OBJECT_FILES}
	${CREATE_LIB} $(call FIX_PATH,${PROJECT_DIRECTORY}/${LIB_OUT_DIRECTORY}/lib${LIB_NAME}_no_graphics-d${LIB_EXTENSION}) ${LIB_OBJECT_FILES_NO_GRAPHICS}
	@echo Debug Libs created

${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}%${PATH_SEPARATOR}:
	$(shell ${MKDIR} ${@})

${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY}${PATH_SEPARATOR}:
	$(shell ${MKDIR} ${@})

clean: clean_object clean_exe clean_libs
	@echo Cleaned

clean_object:
	$(shell ${RMDIR} ${PROJECT_DIRECTORY}${OBJECT_OUT_DIRECTORY})
	@echo Cleaned Objects

clean_exe:
	$(shell ${RM} ${PROJECT_DIRECTORY}${PATH_SEPARATOR}${EXE_NAME}${EXE})
	@echo Cleaned Executable

clean_libs:
	$(shell ${RMDIR} ${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY})
	@echo Cleaned Libs

install:
	$(shell sudo cp -rf ${PROJECT_DIRECTORY}/include/Utils /usr/include)
	$(shell sudo cp -f ${PROJECT_DIRECTORY}${LIB_OUT_DIRECTORY}/* /usr/lib)

info:
	@echo Working Directory: $(PROJECT_DIRECTORY)
	@echo Number of Threads: $(NUM_THREADS)
	@echo Compiler: $(CPP_COMPILER)
	@echo Debug Flags: $(DEBUG_FLAGS)
	@echo Release Flags: $(RELEASE_FLAGS)
	@echo Compiler Options (Platform Specific): $(COMPILE_OPTIONS)
	@echo Linker Flags (Platform Specific): $(LINKER_FLAGS)
	@echo Include Directories: $(INCLUDE_DIRECTORIES)
	@echo Library Directories: $(LIB_DIRECTORIES)
	@echo -----------------------------------------
	@echo __EXECUTABLE SPECIFIC FILES__
	@echo Source Directories: ${EXPANDED_SOURCE_DIRECTORIES}
	@echo Sources: ${SOURCE_FILES}
	@echo Objects: ${OBJECT_FILES}
	@echo -----------------------------------------
	@echo __LIB SPECIFIC FILES__
	@echo Source Directories: ${EXPANDED_SOURCE_DIRECTORIES}
	@echo Sources: ${SOURCE_FILES}
	@echo Objects: ${OBJECT_FILES}

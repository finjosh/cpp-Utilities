# directory that the makefile was called from
PROJECT_DIR_TEMP:=$(patsubst %/makefile,%, $(abspath $(lastword ${MAKEFILE_LIST})))
#! DONT EDIT ABOVE THIS

# the number is how many tasks are going to be used for compiling (should be the number of threads your cpu has for best performance)
MAKEFLAGS=-j16
# exe name
PROJECT:=utils
# the directory in which all .o and .d files will be made
OBJ_O_DIR:=bin
# the include flags for compilation by default includes the project directory and include directory
INCLUDE_DIRS=/VSCodeFolder/Libraries/SFML-3.0.0/include /VSCodeFolder/Libraries/TGUI-1.7/include
# extra include flags
INCLUDE_FLAGS=-D SFML_STATIC
# the paths to libs for linking
LIB_DIRS=/VSCodeFolder/Libraries/SFML-3.0.0/lib V:/VSCodeFolder/Libraries/TGUI-1.7/lib
# source files directory (the project directory is automatically added)
SRC:=src
# the directory for lib files that are made with "make libs"
LIB_DIR:=libs
# the directories where all the source files that you want in the lib are
LIB_SOURCE:=src/Utils src/External
# entirely hard coded sources
LIB_NO_GRAPHICS_SOURCE:=V:\Git_projects\cpp-Utilities\src\Utils\CommandHandler.cpp V:\Git_projects\cpp-Utilities\src\Utils\EventHelper.cpp V:\Git_projects\cpp-Utilities\src\Utils\iniParser.cpp V:\Git_projects\cpp-Utilities\src\Utils\Log.cpp V:\Git_projects\cpp-Utilities\src\Utils\StringHelper.cpp V:\Git_projects\cpp-Utilities\src\Utils\TerminatingFunction.cpp

# compiler command
CC:=g++
# linker flags for compilation
# add "-mwindows" to disable the terminal
LINKERFLAGS:=-ltgui-s \
			-lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype \
			-lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 \
			-lsfml-audio-s -lsfml-system-s -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg \
			-lsfml-network-s -lsfml-system-s -lws2_32 \
			-lsfml-system-s -lwinmm \
			-lstdc++ 
# only when making the ignore lib (ONLY INCLUDE THE GIVEN HERE)
LINKERFLAGS_IGNORE:=
# flags to generate dependencies for all .o files
DEPFLAGS:=-MP -MD
DEBUG_FLAGS = -g
RELEASE_FLAGS = -O3
# any compiler options -Wextra -Wall
COMPILE_OPTIONS:=-std=c++20 -static ${RELEASE_FLAGS}

#! DONT EDIT ANYTHING FROM HERE DOWN

ifeq ($(OS),Windows_NT)
	EXE:=.exe
	FIXPATH=$(subst /,\,$1)
	PROJECT_DIR:=$(call FIXPATH,${PROJECT_DIR_TEMP})
	SOURCEDIRS:=$(call FIXPATH,$(shell dir ${SRC} /s /b /ad)) $(call FIXPATH,${PROJECT_DIR}/${SRC}) $(call FIXPATH,${PROJECT_DIR})
	LIBSOURCEDIRS:=$(foreach dir,${LIB_SOURCE},$(call FIXPATH,$(shell dir "${dir}" /s /b /ad)) $(call FIXPATH,${PROJECT_DIR}/${dir}))
	RM=del /q /f
	RMDIR=rmdir /S /Q
	MD:=mkdir
# if you are using wsl or something which requires a more exact path add it here (this path will be added in front of all lib_dirs and include_dirs)
	DRIVEPATH:=
else #! this does not work
	EXE:=
	FIXPATH=$(subst \,/,$1)
	PROJECT_DIR:=$(call FIXPATH,${PROJECT_DIR_TEMP})
	SOURCEDIRS:=$(shell find "$$PWD" -type d -path "*${PROJECT_DIR}/${SRC}*") ${PROJECT_DIR}
	RM=rm -r --preserve-root
	RMDIR=rm -r --preserve-root
	MD:=mkdir -p
# if you are using wsl or something which requires a more exact path add it here (this path will be added in front of all lib_dirs and include_dirs)
	DRIVEPATH:=/mnt/v
endif

INCLUDES:=$(addprefix -I ${DRIVEPATH},$(call FIXPATH,${INCLUDE_DIRS})) ${INCLUDE_FLAGS} -I ${PROJECT_DIR} -I ${PROJECT_DIR}/include
LIBS:=$(addprefix -L ${DRIVEPATH},$(call FIXPATH,${LIB_DIRS}))

# all .cpp file paths
SRC:=$(foreach D,${SOURCEDIRS},$(wildcard ${D}/*.cpp))
LIBSRC:=$(foreach D,${LIBSOURCEDIRS},$(wildcard ${D}/*.cpp))
# Create an object file of every cpp file
OBJECTS:=$(patsubst ${PROJECT_DIR}%,${PROJECT_DIR}/${OBJ_O_DIR}%,$(call FIXPATH,$(patsubst %.cpp,%.o,${SRC})))
LIBOBJECTS:=$(patsubst ${PROJECT_DIR}%,${PROJECT_DIR}/${OBJ_O_DIR}%,$(call FIXPATH,$(patsubst %.cpp,%.o,${LIBSRC})))
LIBOBJECTS_NO_GRAPHICS=$(patsubst ${PROJECT_DIR}%,${PROJECT_DIR}/${OBJ_O_DIR}%,$(call FIXPATH,$(patsubst %.cpp,%.o,${LIB_NO_GRAPHICS_SOURCE})))
# Creating dependency files
DEPFILES=$(patsubst %.o,%.d,${OBJECTS})
# All bin directories
BIN_DIRS=$(foreach dir,$(call FIXPATH,$(SOURCEDIRS)),$(patsubst $(call FIXPATH,$(PROJECT_DIR)%),$(call FIXPATH,$(PROJECT_DIR)/$(OBJ_O_DIR)%),$(dir)))

# so there is no file that gets mistake with the tasks listed
.PHONY = all info clean lib run ignore_lib


all: ${BIN_DIRS} ${PROJECT}

# main build
${PROJECT}: ${OBJECTS}
	${CC} ${COMPILE_OPTIONS} ${INCLUDES} -o ${@} ${^} ${LIBS} ${LINKERFLAGS}

# build objects for the main build
${PROJECT_DIR}/${OBJ_O_DIR}%.o:${PROJECT_DIR}%.cpp
	${CC} ${COMPILE_OPTIONS} ${INCLUDES} ${DEPFLAGS} -c -o ${@} ${<}

# build the lib with the same compile options
lib: ${BIN_DIRS} ${LIB_DIR} ${LIBOBJECTS}
	ar rcs $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}/lib${PROJECT}.a) ${LIBOBJECTS}
	ar rcs $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}/lib${PROJECT}_no_graphics.a) ${LIBOBJECTS_NO_GRAPHICS}
	@echo Libs created

# include the dependencies
-include ${DEPFILES}

# on linux it confuses the create directory target with dependency files
$(call FIXPATH,${PROJECT_DIR}/${OBJ_O_DIR})%.d:
	@echo -n ""

$(call FIXPATH,${PROJECT_DIR}/${OBJ_O_DIR})%:
	$(call makeDir,$(call FIXPATH,${@}))

${LIB_DIR}:
	$(call makeDir,$(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}))

clean:
	$(shell ${RMDIR} ${OBJ_O_DIR} $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}))
	$(shell ${RM} ${PROJECT}${EXE})

# builds and runs the program
run: ${BIN_DIRS} ${PROJECT}
	./${PROJECT}${EXE}

# makes a dir for the given path
makeDir=$(shell ${MD} $(call FIXPATH,$1))
# make bin directories 
# makeBinDir=$(foreach dir,${BIN_DIRS},$(call makeDir,${dir}))

info:
	@echo Project Directory: ${PROJECT_DIR}
	@echo Source Directories: ${SOURCEDIRS} 
	@echo Lib Source Directories: ${LIBSOURCEDIRS} 
	@echo temp: ${COMPILE_OPTIONS} ${INCLUDES}
	@echo Object Files: ${OBJECTS}
	@echo Dependencies: ${DEPFILES}

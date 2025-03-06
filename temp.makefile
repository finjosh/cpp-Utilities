# directory that the makefile was called from
PROJECT_DIR_TEMP:=$(patsubst %/makefile,%, $(abspath $(lastword ${MAKEFILE_LIST})))
#! DONT EDIT ABOVE THIS

# the number is how many tasks are going to be used for compiling (should be the number of threads your cpu has for best performance)
MAKEFLAGS=-j16
# exe name
PROJECT:=utils
# the directory in which all .o and .d files will be made
OBJ_O_DIR:=bin
# source files directory (the project directory is automatically added)
SRC:=src
# the directory for lib files that are made with "make libs"
LIB_DIR:=lib
# the directories where all the source files that you want in the lib are
LIB_SOURCE:=src/Utils src/External
# entirely hard coded sources
LIB_NO_GRAPHICS_SOURCE:=$(PROJECT_DIR_TEMP)/src\Utils\CommandHandler.cpp $(PROJECT_DIR_TEMP)/src\Utils\EventHelper.cpp $(PROJECT_DIR_TEMP)/src\Utils\iniParser.cpp $(PROJECT_DIR_TEMP)/src\Utils\Log.cpp $(PROJECT_DIR_TEMP)/src\Utils\StringHelper.cpp $(PROJECT_DIR_TEMP)/src\Utils\TerminatingFunction.cpp

# compiler command
CC:=g++

#************* windows/linux specific settings *************
#***********************************************************
# the include flags for compilation by default includes the project directory and include directory
WINDOWS_INCLUDE_DIRS=/VSCodeFolder/Libraries/SFML-3.0.0/include /VSCodeFolder/Libraries/TGUI-1.7/include
LINUX_INCLUDE_DIRS=/usr/include/SFML /usr/include/TGUI
# extra include flags
WINDOWS_INCLUDE_FLAGS=-D SFML_STATIC
# the paths to libs for linking
WINDOWS_LIB_DIRS=/VSCodeFolder/Libraries/SFML-3.0.0/lib V:/VSCodeFolder/Libraries/TGUI-1.7/lib
LINXUX_LIB_DIRS=/usr/lib
# extra include flags
WINDOWS_INCLUDE_FLAGS:=-D SFML_STATIC
LINUX_INCLUDE_FLAGS:=
# linker flags for compilation
# add "-mwindows" to disable the terminal
WINDOWS_LINKERFLAGS:=-ltgui-s \
					 -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype \
					 -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 \
					 -lsfml-audio-s -lsfml-system-s -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg \
					 -lsfml-network-s -lsfml-system-s -lws2_32 \
					 -lsfml-system-s -lwinmm \
					 -lstdc++ 
LINUX_LINKERFLAGS:=-ltgui \
					-lsfml-graphics -lsfml-window -lsfml-system \
					-lsfml-window -lsfml-system \
					-lsfml-audio -lsfml-system \
					-lsfml-network \
					-lsfml-system \
					-lstdc++

# flags speicfic to the os that is being compiled on
WINDOWS_FLAGS:=-static
LINUX_FLAGS:=
# flags for only used why compiling the libs
WINDOWS_LIB_FLAGS:=
LINUX_LIB_FLAGS:=-fPIC
#***********************************************************
#***********************************************************

# only when making the ignore lib (only include libs that are not for graphics)
LINKERFLAGS_IGNORE:=
# flags to generate dependencies for all .o files
DEPFLAGS:=-MP -MD
DEBUG_FLAGS = -g -D _DEBUG
RELEASE_FLAGS = -O3
CURRENT_FLAGS = ${DEBUG_FLAGS}
# any compiler options -Wextra -Wall
OTHER_COMPILE_OPTIONS=-std=c++20 ${CURRENT_FLAGS}

#! DONT EDIT ANYTHING FROM HERE DOWN

ifeq ($(OS),Windows_NT)
	EXE:=.exe
	LIB_EXTENSION:=.a
	CREATE_LIB:=ar rcs 
	FIXPATH=$(subst /,\,$1)
	PROJECT_DIR:=$(call FIXPATH,${PROJECT_DIR_TEMP})
	SOURCEDIRS:=$(call FIXPATH,$(shell dir ${SRC} /s /b /ad)) $(call FIXPATH,${PROJECT_DIR}/${SRC}) $(call FIXPATH,${PROJECT_DIR})
	LIBSOURCEDIRS:=$(foreach dir,${LIB_SOURCE},$(call FIXPATH,$(shell dir "${dir}" /s /b /ad)) $(call FIXPATH,${PROJECT_DIR}/${dir}))
	RM=del /q /f
	RMDIR=rmdir /S /Q
	MD:=mkdir
# if you are using wsl or something which requires a more exact path add it here (this path will be added in front of all lib_dirs and include_dirs)
	DRIVEPATH:=

	INCLUDES:=$(addprefix -I ${DRIVEPATH},$(call FIXPATH,${WINDOWS_INCLUDE_DIRS})) ${INCLUDE_FLAGS} -I ${PROJECT_DIR} -I ${PROJECT_DIR}/include
	LIBS:=$(addprefix -L ${DRIVEPATH},$(call FIXPATH,${WINDOWS_LIB_DIRS}))
	LINKERFLAGS:=$(WINDOWS_LINKERFLAGS)
	INCLUDE_FLAGS:=$(WINDOWS_INCLUDE_FLAGS)
	COMPILE_OPTIONS:=${OTHER_COMPILE_OPTIONS} ${WINDOWS_FLAGS}
	LIB_FLAGS:=${WINDOWS_LIB_FLAGS}
else #! this does not work
	EXE:=
	LIB_EXTENSION:=.so
	CREATE_LIB:=${CC} -shared -o
	FIXPATH=$(subst \,/,$1)
	PROJECT_DIR:=$(call FIXPATH,${PROJECT_DIR_TEMP})
	SOURCEDIRS:=$(shell find "$$PWD" -type d -path "*${PROJECT_DIR}/${SRC}*") ${PROJECT_DIR}
	LIBSOURCEDIRS:=$(foreach dir,${LIB_SOURCE},$(call FIXPATH,$(shell dir "${dir}" /s /b /ad)) $(call FIXPATH,${PROJECT_DIR}/${dir}))
	RM=rm -r --preserve-root
	RMDIR=rm -r --preserve-root
	MD:=mkdir -p
# if you are using wsl or something which requires a more exact path add it here (this path will be added in front of all lib_dirs and include_dirs)
	DRIVEPATH:=
	INCLUDES:=$(addprefix -I ${DRIVEPATH},$(call FIXPATH,${LINUX_INCLUDE_DIRS})) ${INCLUDE_FLAGS} -I ${PROJECT_DIR} -I ${PROJECT_DIR}/include
	LIBS:=$(addprefix -L ${DRIVEPATH},$(call FIXPATH,${LINUX_LIB_DIRS}))
	LINKERFLAGS:=$(LINUX_LINKERFLAGS)
	INCLUDE_FLAGS:=$(LINUX_INCLUDE_FLAGS)
	COMPILE_OPTIONS:=${OTHER_COMPILE_OPTIONS} ${LINUX_FLAGS}
	LIB_FLAGS:=${LINUX_LIB_FLAGS}
endif

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
.PHONY = all info clean libs libs-r libs-d run clean_object clean_exe clean_libs

all: ${BIN_DIRS} ${PROJECT}

# main build
${PROJECT}: ${OBJECTS}
	${CC} ${CURRENT_FLAGS} ${INCLUDES} -o ${@} ${^} ${LIBS} ${LINKERFLAGS}

# build objects for the main build
${PROJECT_DIR}/${OBJ_O_DIR}%.o:${PROJECT_DIR}%.cpp
	${CC} ${CURRENT_FLAGS} ${INCLUDES} ${DEPFLAGS} -c -o ${@} ${<}

libs: 
	make libs-r 
	make libs-d

# build the lib with the same compile options
libs-r: CURRENT_FLAGS = ${COMPILE_OPTIONS} ${RELEASE_FLAGS} ${LIB_FLAGS}
libs-r: clean_object ${BIN_DIRS} ${LIB_DIR} ${LIBOBJECTS}
	${CREATE_LIB} $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}/lib${PROJECT}${LIB_EXTENSION}) ${LIBOBJECTS}
	${CREATE_LIB} $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}/lib${PROJECT}_no_graphics${LIB_EXTENSION}) ${LIBOBJECTS_NO_GRAPHICS}
	@echo Release Libs created

libs-d: CURRENT_FLAGS = ${COMPILE_OPTIONS} ${DEBUG_FLAGS} ${LIB_FLAGS}
libs-d: clean_object ${BIN_DIRS} ${LIB_DIR} ${LIBOBJECTS}
	${CREATE_LIB} $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}/lib${PROJECT}-d${LIB_EXTENSION}) ${LIBOBJECTS}
	${CREATE_LIB} $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}/lib${PROJECT}_no_graphics-d${LIB_EXTENSION}) ${LIBOBJECTS_NO_GRAPHICS}
	@echo Debug Libs created

# include the dependencies
-include ${DEPFILES}

# on linux it confuses the create directory target with dependency files
$(call FIXPATH,${PROJECT_DIR}/${OBJ_O_DIR})%.d:
	@echo -n ""

$(call FIXPATH,${PROJECT_DIR}/${OBJ_O_DIR})%:
	$(call makeDir,$(call FIXPATH,${@}))

${LIB_DIR}:
	$(call makeDir,$(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}))

clean: clean_object clean_exe clean_libs	

clean_object:
	$(shell ${RMDIR} ${OBJ_O_DIR})

clean_exe:
	$(shell ${RM} ${PROJECT}${EXE})

clean_libs:
	$(shell ${RMDIR} $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}))

check_install_dirs:
	$(shell find /usr/include -maxdepth 1 -type d -name "Utils")
	@echo "if nothing prints from the finds then there are no conflicting files that will be overwritten"
	@echo "note there could be conflicting lib names that are not checked"

# this is only implemented for linux
install_clean: libs
	$(shell sudo cp -rf $(PROJECT_DIR)/include/Utils /usr/include)
	$(shell sudo cp -f $(PROJECT_DIR)/lib/* /usr/lib)

# this is only implemented for linux
install:
	$(shell sudo cp -rf $(PROJECT_DIR)/include/Utils /usr/include)
	$(shell sudo cp -f $(PROJECT_DIR)/lib/* /usr/lib)

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

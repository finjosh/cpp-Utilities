# directory that the makefile was called from
mkfile_path:=$(abspath $(lastword $(MAKEFILE_LIST)))
project_name:=$(strip $(notdir $(patsubst %/,%,$(dir $(mkfile_path)))))
project_dir:=$(patsubst %/makefile,%, $(mkfile_path))
#! DONT EDIT ABOVE THIS

# exe name
PROJECT:=main
# the directory in which all .o and .d files will be made
OBJ_O_DIR:=bin
# assumes that source directories are the same as include
SRCDIRS:=. src src/Utils src/Utils/Debug src/Examples
# the include flags for compilation
INCLUDES:=-I /VSCodeFolder/Libraries/SFML-2.6.1/include -D SFML_STATIC -I /VSCodeFolder/Libraries/TGUI-1.1/include -I $(project_dir) -I $(project_dir)/include
# the paths to libs for linking
LIBS=-L /VSCodeFolder/Libraries/SFML-2.6.1/lib -L /VSCodeFolder/Libraries/TGUI-1.1/lib -L $(project_dir)

# the directory for lib files
LIB_DIR:=libs

#! DONT EDIT ANYTHING FROM HERE DOWN
# any of the dirs for source files
# DIRS:=dir /AD /B /S	
# findSubDirs=$(shell ${DIRS} $1)

# all .cpp file paths
SRC:=$(foreach D,$(SRCDIRS),$(wildcard $(D)/*.cpp))
# Create an object file of every cpp file
OBJECTS=$(addprefix $(OBJ_O_DIR)/,$(patsubst %.cpp,%.o,$(SRC)))
# Creating dependency files
DEPFILES=$(patsubst %.o,%.d,$(OBJECTS))

OUTPATHS:=$(foreach d,$(filter-out ./,$(SRCDIRS)),$(OBJ_O_DIR)/$(d)/)

# so there is no file that gets mistaked with the tasks listed
.PHONY = all info clean

# compiler to use
CC=g++

# static (remove -static for it to no longer be static)
STATIC_BUILD=-static
# debugging build (remove -g for the compiled project to not be a debugging build)
DEBUG_BUILD=-g
# linker flags for compilation
# add "-mwindows" to disable the terminal
LINKERFLAGS=$(LIBS) \
			-ltgui-s -lsfml-graphics-s -lsfml-window-s \
			-lsfml-system-s -lsfml-audio-s -lsfml-network-s \
			-lws2_32 -lflac -lvorbisenc -lvorbisfile -lvorbis \
			-logg -lopenal32 -lopengl32 -lwinmm -lgdi32 -lfreetype \
			-lstdc++ 
# flags to generate dependencies for all .o files
DEPFLAGS=-MP -MD
# any compiler options
COMPILE_OPTIONS=-std=c++20 $(DEBUG_BUILD) $(STATIC_BUILD) $(INCLUDES) $(DEPFLAGS)

all: $(OUTPATHS) $(PROJECT)

libs: $(patsubst $(OBJ_O_DIR)/%,$(LIB_DIR)/%,$(OUTPATHS)) $(patsubst $(OBJ_O_DIR)/%.o,$(LIB_DIR)/%.a,$(OBJECTS))
	@echo Libs created

$(LIB_DIR)/%.a:$(OBJ_O_DIR)/%.o
	ar rcs $@ $^

$(LIB_DIR)/%/:
	$(call makeDir,$(patsubst %/,%,$@))

# try interlacing the objects and header files
$(PROJECT): $(OBJECTS)
	$(CC) -o $@ $(STATIC_BUILD) $^ $(DEBUG_BUILD) $(LINKERFLAGS)

$(OBJ_O_DIR)/%.o:%.cpp
	$(CC) $(COMPILE_OPTIONS) -c -o $@ $<

# include the dependencies
-include $(DEPFILES)

$(OBJ_O_DIR)/%/:
	$(call makeDir,$(patsubst %/,%,$@))

# shell command for removing a dir
RMDIR:=rmdir /s /q
# shell command for removing a file
RMFILE:=del /q /f

clean:
	$(shell ${RMFILE} $(PROJECT).exe)
	$(shell ${RMDIR} $(OBJ_O_DIR))
	$(call makeBinDir)

# the shell command for making a dir
MKDIR_P:=mkdir
# makes a dir for the given path
makeDir=$(shell ${MKDIR_P} $(subst /,\,$1))
# make bin directories 
makeBinDir=$(foreach d,$(filter-out ./,$(SRCDIRS)),$(call makeDir,$(patsubst $(OBJ_O_DIR)/%/,$(OBJ_O_DIR)/%,$(OBJ_O_DIR)/$(d))))

info:
	@echo Project Name: $(project_name)
	@echo Makefile Dir: $(mkfile_path)
	@echo Project Dir: $(project_dir)
	@echo Compiler: $(CC)
	@echo Object Folder: $(OBJ_O_DIR)
	@echo Output Paths: $(OUTPATHS)
	@echo Source Files: $(SRC)
	@echo Linker Flags: $(LINKERFLAGS)
	@echo Libraries: $(LIBS)
	@echo Includes: $(INCLUDES)

# directory that the makefile was called from
mkfile_path:=$(abspath $(lastword $(MAKEFILE_LIST)))
project_name:=$(strip $(notdir $(patsubst %/,%,$(dir $(mkfile_path)))))
project_dir:=$(patsubst %/makefile,%, $(mkfile_path))

# any of the dirs for source files
SRCDIRS=. src src/Utils src/Utils/Debug

# exe name
PROJECT:=main
# all .cpp file paths
SRC=$(foreach D,$(SRCDIRS),$(wildcard $(D)/*.cpp))
# the include flags for compilation
INCLUDES=-I /VSCodeFolder/Libraries/SFML-2.6.0/include -D SFML_STATIC -I /VSCodeFolder/Libraries/TGUI-1.0/include -I $(project_dir)
# the paths to libs for linking
LIBS=-L /VSCodeFolder/Libraries/SFML-2.6.0/lib -L /VSCodeFolder/Libraries/TGUI-1.0/lib -L $(project_dir)

OBJ_O_DIR=bin
# Create an object file of every cpp file
OBJECTS=$(addprefix $(OBJ_O_DIR)/,$(patsubst %.cpp,%.o,$(SRC)))
# Creating dependency files
DEPFILES=$(patsubst %.o,%.d,$(OBJECTS))

# so there is no file that gets mistaked with the tasks listed
.PHONY = all info bin help

# compiler to use
CC=g++

# static (remove -static for it to no longer be static)
STATIC_BUILD=-static
# debugging build (remove -g for the compiled project to not be a debugging build)
DEBUG_BUILD=-g
# linker flags for compilation
# add "-mwindows" to disable the terminal
LINKERFLAGS=$(DEBUG_BUILD) \
			$(LIBS) \
			-ltgui-s -lsfml-graphics-s -lsfml-window-s \
			-lsfml-system-s -lsfml-audio-s -lsfml-network-s \
			-lws2_32 -lflac -lvorbisenc -lvorbisfile -lvorbis \
			-logg -lopenal32 -lopengl32 -lwinmm -lgdi32 -lfreetype \
			-lstdc++ 
# flags to generate dependencies for all .o files
DEPFLAGS=-MP -MD
# any compiler options
COMPILE_OPTIONS=-std=c++20 $(DEBUG_BUILD) $(STATIC_BUILD) $(INCLUDES) $(DEPFLAGS)

all: $(PROJECT)

# try interlacing the objects and header files
$(PROJECT): $(OBJECTS)
	$(CC) -o $@ $(STATIC_BUILD) $^ $(LINKERFLAGS)

$(OBJ_O_DIR)/%.o:%.cpp
	$(CC) $(COMPILE_OPTIONS) -c -o $@ $<

# include the dependencies
-include $(DEPFILES)

bin:
	$(foreach d,$(filter-out ./,$(SRCDIRS)),$(call makeDir,$(patsubst $(OBJ_O_DIR)/%/,$(OBJ_O_DIR)/%,$(OBJ_O_DIR)/$(d))))

MKDIR_P:=mkdir
makeDir=$(shell ${MKDIR_P} $(subst /,\,$1))

info:
	@echo Project Name: $(project_name)
	@echo Makefile Dir: $(mkfile_path)
	@echo Project Dir: $(project_dir)
	@echo Source Files: $(SRC)
	@echo Compiler: $(CC)
	@echo Linker Flags: $(LINKERFLAGS)
	@echo Libraries: $(LIBS)
	@echo Includes: $(INCLUDES)
	@echo Object Folder: $(OBJ_O_DIR)

help:
	@echo - Before trying to "make" the project run "make bin"
	@echo - After a new directory is added make sure to update the make file and run "make bin" again

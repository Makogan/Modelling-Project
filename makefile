EXEFILE := $(shell basename $(CURDIR))


DIRECTORIES = $(filter-out ./ ./.%, $(shell find ./ -maxdepth 10 -type d))
IFLAGS= -I/usr/include/freetype2
LOCAL_I_DIRS =$(addprefix -I, $(DIRECTORIES))
LFLAGS= -L/usr/lib/nvidia-375 -L/usr/local/lib -L/usr/include/GL -L/usr/local/include/freetype2 -L/usr/local/lib/
LIBS = -lglfw -lGL -lGLU -lOpenGL -lGLEW -pthread -lfreetype

SRC := $(wildcard *.cpp) $(wildcard **/*.cpp)

$(EXEFILE): $(EXEFILE).cpp
	@g++ -std=c++11 -o $(EXEFILE) -Wall -Wno-comment $(SRC) $(IFLAGS) $(LOCAL_I_DIRS) $(LFLAGS) $(LIBS) 

all: run clean

run: $(EXEFILE)
	@./$(EXEFILE)

clean:
	@rm $(EXEFILE)

print-%: ; @echo $* = $($*)

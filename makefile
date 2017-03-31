EXEFILE = template
VSHADER = VertexShader.glsl
FSHADER = FragmentShader.glsl
SHADERDIR = Shaders

IFLAGS= -I/usr/include/freetype2
LFLAGS= -L/usr/lib/nvidia-367 -L/usr/local/lib -L/usr/include/GL -L/usr/local/include/freetype2 -L/usr/local/lib/
LIBS = -lglfw -lGL -lGLU -lOpenGL -lGLEW -pthread -lfreetype

SRC=*.cpp

$(EXEFILE): template.cpp
	g++ -std=c++11 -o $(EXEFILE) -Wall -Wno-comment $(SRC) $(IFLAGS) $(LFLAGS) $(LIBS)

all: run

run: $(EXEFILE)
	./$(EXEFILE) $(SHADERDIR)/$(VSHADER) $(SHADERDIR)/$(FSHADER)

clean:
	rm $(EXEFILE)

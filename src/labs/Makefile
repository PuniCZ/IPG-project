##################################################
# nastaveni
CFLAGS_COMMON=-pipe -Wall -ggdb
CC=gcc
CXX=g++

##################################################
# nastaveni knihoven s OpenGL
ifeq ($(OS), Windows_NT)
	CFLAGS=$(CFLAGS_COMMON) -Wl,--enable-auto-import
	LIBS=-lopengl32 -lglew32 -lSDL
	RM=del
else
	USE_GLEE=1
	CFLAGS=$(CFLAGS_COMMON) `sdl-config --cflags` -I. -IGL -DUSE_GLEE
	LIBS=-L/usr/lib `sdl-config --libs` -lGL
	RM=rm -f
endif

CXXFLAGS=$(CFLAGS)

##################################################
# obecne cile

LAB1=lab01_01_kostra.exe lab01_02_kresleni.exe lab01_03_primitiva.exe lab01_04_interpolace.exe lab01_05_indexy.exe lab01_06_blending.exe lab01_07_3D.exe lab01_08_poradi.exe lab01_task.exe

LAB2=lab02_01_osvetleni.exe lab02_02_shadertex.exe lab02_03_textury.exe lab02_04_mipmapy.exe lab02_05_obrazky.exe lab02_task.exe

LAB3=lab03_01_animace.exe lab03_02_zrcadlo.exe lab03_03_stin.exe lab03_task.exe

ifdef USE_GLEE
	DEPS=pgr.o GLee.o
else
	DEPS=pgr.o
endif

.PHONY: all lab1 lab2 lab3 clean

all: lab1 lab2 lab3

lab1: $(LAB1)

lab2: $(LAB2)

lab3: $(LAB3)

clean:
	$(RM) $(LAB1) $(LAB2) $(LAB3) $(DEPS)

##################################################
# pravidla

%.exe: %.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.exe: %.cpp
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)

##################################################
# zavislosti

pgr.o: pgr.cpp pgr.h

# LAB1

lab01_01_kostra.exe: lab01_01_kostra.cpp $(DEPS)

lab01_02_kresleni.exe: lab01_02_kresleni.cpp $(DEPS)

lab01_03_primitiva.exe: lab01_03_primitiva.cpp $(DEPS)

lab01_04_interpolace.exe: lab01_04_interpolace.cpp $(DEPS)

lab01_05_indexy.exe: lab01_05_indexy.cpp $(DEPS)

lab01_06_blending.exe: lab01_06_blending.cpp $(DEPS)

lab01_07_3D.exe: lab01_07_3D.cpp $(DEPS)

lab01_08_poradi.exe: lab01_08_poradi.cpp $(DEPS)

lab01_task.exe: lab01_task.cpp $(DEPS)

# LAB2

lab02_01_osvetleni.exe: lab02_01_osvetleni.cpp $(DEPS) bunny.h

lab02_02_shadertex.exe: lab02_02_shadertex.cpp $(DEPS) bunny.h

lab02_03_textury.exe: lab02_03_textury.cpp $(DEPS)

lab02_04_mipmapy.exe: lab02_04_mipmapy.cpp $(DEPS)

lab02_05_obrazky.exe: lab02_05_obrazky.cpp $(DEPS)

lab02_task.exe: lab02_task.cpp $(DEPS) sphere.h

# LAB3

lab03_01_animace.exe: lab03_01_animace.cpp $(DEPS) sphere.h

lab03_02_zrcadlo.exe: lab03_02_zrcadlo.cpp $(DEPS) sphere.h

lab03_03_stin.exe: lab03_03_stin.cpp $(DEPS) sphere.h

lab03_task.exe:lab03_task.cpp $(DEPS) sphere.h

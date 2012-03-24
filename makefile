CC=g++
CFLAGS=-g -Wall
LFLAGS=
OBJS=MergeTest.o
VOBJS=MergeTestVisual.o

all:  MergeTest MergeTestVisual

# rule for building the executable
MergeTest: $(OBJS)
	$(CC) -o MergeTest $(OBJS)

# rule for building the executable
MergeTestVisual: $(VOBJS)
	$(CC) -o MergeTestVisual $(VOBJS)

MergeTest.o: MergeTest.cpp NodeList.h
	$(CC) -c -o MergeTest.o MergeTest.cpp

MergeTestVisual.o: MergeTest.cpp NodeList.h
	$(CC) -c -o MergeTestVisual.o -DVISUAL MergeTest.cpp


# the following removes all .o files, but leaves the executable
clean:
	rm -f *.o

# Makefile for C++ kdtrees

CC = g++ 
CFLAGS = -g -std=c++11 -Wall
EDIR = Extension
TDIR = Types

all: build_kdtree query_kdtree
int: build_kdtree_int query_kdtree_int
taxi: query_kdtree_taxicab

build_kdtree: build_kdtree.o KDTree.o
	$(CC) $(CFLAGS) build_kdtree.o KDTree.o -o build_kdtree
	
build_kdtree_int: $(TDIR)/build_kdtree_int.o KDTree.o
	$(CC) $(CFLAGS) $(TDIR)/build_kdtree_int.o KDTree.o -o $(TDIR)/build_kdtree_int
	
query_kdtree: query_kdtree.o KDTree.o 
	$(CC) $(CFLAGS) query_kdtree.o KDTree.o -o query_kdtree
	
query_kdtree_taxicab: $(EDIR)/query_kdtree_taxicab.o KDTree.o 
	$(CC) $(CFLAGS) $(EDIR)/query_kdtree_taxicab.o KDTree.o -o $(EDIR)/query_kdtree_taxicab	
	
query_kdtree_int: $(TDIR)/query_kdtree_int.o KDTree.o 
	$(CC) $(CFLAGS) $(TDIR)/query_kdtree_int.o KDTree.o -o $(TDIR)/query_kdtree_int
	
%.o: %.cpp
	$(CC) -MMD $(CFLAGS) -c -o $@ $< 

-include *.d

clean:
	$(RM) *.o *.d build_kdtree query_kdtree 

clean_int:
	$(RM) $(TDIR)/*.o $(TDIR)/*.d $(TDIR)/build_kdtree_int $(TDIR)/query_kdtree_int
	
clean_taxi:
	$(RM) $(EDIR)/*.o $(EDIR)/*.d $(EDIR)/query_kdtree_taxicab

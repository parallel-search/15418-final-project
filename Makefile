CXX=g++ -m64
CXXFLAGS=-I. -O3 -Wall -std=c++11

APP_NAME=map_gen
OBJDIR=objs

map:
		$(CXX) $(CXXFLAGS) -o $(APP_NAME) map_gen.cpp map_gen.h utility.h

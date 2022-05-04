CXX=g++ -m64
CXXFLAGS=-I. -O3 -Wall -std=c++11

APP_NAME=parallel_astar
OBJDIR=objs

default:
		$(CXX) $(CXXFLAGS) -o $(APP_NAME) parallel/parallel_main.cpp *.h
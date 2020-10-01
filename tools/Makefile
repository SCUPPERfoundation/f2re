# ibgames database tools makefile

SRC=./
INC=./
OBJ=./
BIN=./

CPP=gcc -E
CPPFLAGS=-D_XOPEN_SOURCE -DNDEBUG
CXX=g++
CXXCPP=g++ -E
CXXFLAGS=-Wall -Wcast-qual -Wcast-align -Wwrite-strings -Wpointer-arith -O2

all:$(BIN)/avatar2ascii $(BIN)/billing2ascii $(BIN)/ascii2avatar


# object files
AVOBJ=$(OBJ)/avatar2ascii.o
ASCAVOBJ=$(OBJ)/ascii2avatar.o
BILLOBJ=$(OBJ)/billing2ascii.o

# dependencies for executables etc
$(BIN)/avatar2ascii: $(AVOBJ)
	$(CXX) $(CXXFLAGS) -L/usr/lib -ldb_cxx $(AVOBJ) -o $(BIN)/avatar2ascii

$(BIN)/ascii2avatar: $(ASCAVOBJ)
	$(CXX) $(CXXFLAGS) -L/usr/lib -ldb_cxx $(ASCAVOBJ) -o $(BIN)/ascii2avatar

$(BIN)/billing2ascii: $(BILLOBJ)
	$(CXX) $(CXXFLAGS) -L/usr/lib -ldb_cxx $(BILLOBJ) -o $(BIN)/billing2ascii


# avatar obj dependencies
$(OBJ)/avatar2ascii.o:$(SRC)/avatar2ascii.cc $(INC)/avatar.h
	$(CXX) $(CXXFLAGS) -I$(INC) -c $(SRC)/avatar2ascii.cc -o $(OBJ)/avatar2ascii.o

$(OBJ)/ascii2avatar.o:$(SRC)/ascii2avatar.cc $(INC)/avatar.h
	$(CXX) $(CXXFLAGS) -I$(INC) -c $(SRC)/ascii2avatar.cc -o $(OBJ)/ascii2avatar.o

# billing obj dependencies
$(OBJ)/billing2ascii.o:$(SRC)/billing2ascii.cc $(INC)/billing.h
	$(CXX) $(CXXFLAGS) -I$(INC) -c $(SRC)/billing2ascii.cc -o $(OBJ)/billing2ascii.o

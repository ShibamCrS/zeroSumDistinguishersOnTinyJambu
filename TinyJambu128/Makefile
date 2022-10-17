#TARGET=tinyJambu_degree
#TARGET=tinyJambu_degree_inverse
TARGET=tinyJambu_degree_condition_23_80
#TARGET=tinyJambu_degree_inverse_condition
#TARGET=verify_backexpand
CC=g++
OPT=-m64 -std=c++11 -O3
INC=-I/$$GUROBI_HOME/include/
LIB=-L/$$GUROBI_HOME/lib/ -lgurobi_c++ -lgurobi95 -lm


SRC=$(TARGET).cpp
#SRC=$(shell ls *.cpp)
HED=$(shell ls *.h)

all: $(TARGET)
$(TARGET): $(SRC) $(HED)
	$(CC) $(OPT) -o $(TARGET)  $(SRC)  $(INC) $(LIB) 

clean:
	rm -f $(TARGET) 


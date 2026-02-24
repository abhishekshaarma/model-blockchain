CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iincludes
SRC = main.cpp src/Block.cpp src/BlockChain.cpp src/TransactionData.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = blockchain

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o src\*.o blockchain.exe 2>nul || rm -f *.o src/*.o $(TARGET)

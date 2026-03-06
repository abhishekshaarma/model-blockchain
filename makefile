CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iincludes
LDFLAGS := -lssl -lcrypto
TARGET := blockchain

SRC := main.cpp \
       src/Block.cpp \
       src/BlockChain.cpp \
       src/TransactionData.cpp

OBJ := $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

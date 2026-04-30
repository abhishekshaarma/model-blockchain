CXX      = g++
CXXFLAGS = -std=c++17 -Wall -O2
LIBS     = -lssl -lcrypto

SRC = src/FieldElement.cpp \
      src/ECPoint.cpp \
      src/ScalarMul.cpp

# Part 1 -- benchmark binary
bench: main_bench.cpp $(SRC)
	$(CXX) $(CXXFLAGS) main_bench.cpp $(SRC) -o bench $(LIBS)

# Part 2 -- blockchain demo binary
chain: main_chain.cpp $(SRC)
	$(CXX) $(CXXFLAGS) main_chain.cpp $(SRC) -o chain $(LIBS)

# build both
all: bench chain

clean:
	rm -f bench chain

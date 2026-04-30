CXX      = g++
CXXFLAGS = -std=c++17 -Wall -O2
LIBS     = -lssl -lcrypto

# Part 1 -- benchmark binary
bench: main_bench.cpp
	$(CXX) $(CXXFLAGS) mainbench.cpp -o bench $(LIBS)

# Part 2 -- blockchain demo binary
chain: main_chain.cpp
	$(CXX) $(CXXFLAGS) mainchain.cpp -o chain $(LIBS)

# build both
all: bench chain

clean:
	rm -f bench chain

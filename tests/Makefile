LLVM=~/llvm80
CC=${LLVM}/bin/clang
#CXX=${LLVM}/bin/clang++
CXX=g++
SOURCES=test02.cpp

all: create_bin test02

create_bin:
	mkdir -p ./bin

test02: $(SOURCES)
	$(CXX) -I../include $(SOURCES) -L../ -lRedox -o ./bin/$@

test03: test03.cpp create_bin
	$(CXX) -I../include test03.cpp -L../ -lRedoxBnorm -o ./bin/$@

clean:
	rm -rf ./bin

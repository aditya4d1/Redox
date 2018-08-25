LLVM=~/llvm80
CC=${LLVM}/bin/clang
#CXX=${LLVM}/bin/clang++
CXX=g++
INC=./include
LLVM_CONFIG=${LLVM}/bin/llvm-config
CXXFLAGS=`$(LLVM_CONFIG) --cxxflags` -std=c++14 -c
LDFLAGS=`$(LLVM_CONFIG) --ldflags --system-libs --libs core all` -lstdc++

SOURCES_REDUCE=\
src/Redox.cpp \
src/Buffer.cpp \
src/CodeGen/Reduce.cpp \
src/CodeGen/Buffer.cpp \
src/CodeGen/ReduceGen.cpp
OBJECTS_REDUCE=$(SOURCES_REDUCE:.cpp=.o)

SOURCES_BNORM=\
src/BatchNorm.cpp \
src/CodeGen/BatchNormGen.cpp \
src/CodeGen/KernelGenHeader.cpp \
src/CodeGen/KernelGenUtil.cpp
OBJECTS_BNORM=$(SOURCES_BNORM:.cpp=.o)

SHAREDLIB_REDUCE=libRedoxReduce.so
SHAREDLIB_BNORM=libRedoxBnorm.so

Reduce: $(SOURCES_REDUCE) $(SHAREDLIB_REDUCE)
Bnorm: $(SOURCES_BNORM) $(SHAREDLIB_BRNORM)

$(SHAREDLIB_REDUCE): $(OBJECTS_REDUCE)
	$(CXX) -fPIC $(OBJECTS_REDUCE) -shared $(LDFLAGS) -o $@

$(SHAREDLIB_BNORM): $(OBJECTS_BNORM)
	$(CXX) -fPIC $(OBJECTS_BNORM) -shared $(LDFLAGS) -o $@

.cpp.o:
	$(CXX) -I$(INC) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(SHAREDLIB_REDUCE) $(SHAREDLIB_BNORM) *.ll *.s ./src/*.o ./src/CodeGen/*.o

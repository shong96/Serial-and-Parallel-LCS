# Makefile for Cilk++ example.  See source code for details.

CILKPP	= g++
LIBARG	= -O2 -g -lcilkrts -fcilkplus
TARGET	= LCS
SRC	= $(addsuffix .cpp,$(TARGET))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CILKPP) $(SRC) $(LIBARG) -o $@

clean:
	rm -f $(TARGET)

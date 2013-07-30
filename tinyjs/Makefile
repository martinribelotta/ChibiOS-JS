TARGET=tjs

all: $(TARGET)

CXXSRCS = Script.cpp \
       TinyJS.cpp \
       TinyJS_Functions.cpp \
       TinyJS_MathFunctions.cpp
CSRC = rdline.c

OBJS=$(CXXSRCS:.cpp=.o) $(CSRC:.c=.o)

CFLAGS+=-ggdb3
CXXFLAGS+=-ggdb3

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)

clean:
	rm -fR $(TARGET) $(OBJS)

TARGET=tjs

all: $(TARGET)

SRCS = Script.cpp \
       TinyJS.cpp \
       TinyJS_Functions.cpp \
       TinyJS_MathFunctions.cpp

OBJS=$(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) -ggdb3 -c -o $@ $^

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)

clean:
	rm -fR $(TARGET) $(OBJS)

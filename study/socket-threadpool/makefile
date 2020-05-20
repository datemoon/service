
CC:= g++
TARGET:= server
INCLUDE:= -I./
LIBS:= -lpthread -lstdc++
# C++......  
CXXFLAGS:= -std=c++0x -g -Wall -D_REENTRANT
# C.....
# CPPFLAGS:=
OBJECTS :=service.o thread_pool.o  


all:$(TARGET) client
$(TARGET): $(OBJECTS)	
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)  

%.o:%.cpp   	
	$(CC) -c $(CXXFLAGS) $(INCLUDE) $< -o $@  

client:client.o
	$(CC) -o $@ $^ 

.PHONY : clean
clean:   	
	-rm -f $(OBJECTS) $(TARGET) client *.o

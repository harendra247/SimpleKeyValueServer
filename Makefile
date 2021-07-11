CXX        = g++
#CXXFLAGS   = -std=c++17 -Wall -Wextra -O0 -g
CXXFLAGS   = -std=c++17 -Wall -Wextra -O2
LDFLAGS    = -pthread
TARGET     = kvserver

all: init clean client link end
init:
	$(info * [Starting build])

link: simplekvserver.o socket.o
	$(info * [Linking $^])
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $(TARGET)

simplekvserver.o: simplekvserver.cpp
	$(info * [Creating $@ from $<])
	$(CXX) $(CXXFLAGS) -c $<

socket.o: socket.cpp
	$(info * [Creating $@ from $<])
	$(CXX) $(CXXFLAGS) -c $<

client: client.o
	$(info * [Linking $^ to generate client])
	$(CXX) $(CXXFLAGS) $^ -o client

client.o: client.cpp
	$(info * [Creating $@ from $<])
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(info * [Cleaning old build])
	rm -f *.o $(TARGET) client
end:
	$(info * [Build successful. $(TARGET) was created])
.PHONY: all init clean client link end
.SILENT:

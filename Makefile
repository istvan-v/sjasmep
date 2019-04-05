
CXXFLAGS = -Wall -Wno-write-strings -Wno-char-subscripts -O2
ifneq ($(findstring mingw,$(MAKE_HOST)),)
	CXX = g++.exe
	CXXFLAGS += -DMINGW
	EXTENSION = .exe
	RM = del /q
else
    ifneq ($(ISMINGW),)
	CXX = x86_64-w64-mingw32-g++
	CXXFLAGS += -DMINGW
	EXTENSION = .exe
	RM = rm -f
    else
	CXX = g++
	CXXFLAGS += -DMAX_PATH=MAXPATHLEN
	EXTENSION = 
	RM = rm -f
    endif
endif

TARGETS  = sjasm$(EXTENSION)
OBJECTS  = direct.o loose.o parser.o piz80.o reader.o sjasm.o sjio.o tables.o
HEADERS  = direct.h loose.h parser.h piz80.h reader.h sjasm.h sjio.h tables.h
BINDIR   = /usr/local/bin
LDFLAGS  = -static

all: $(TARGETS)

sjasm$(EXTENSION): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) -s

clean:
	-$(RM) $(OBJECTS) $(TARGETS)

install: all
	cp -f $(TARGETS) $(BINDIR)

$(OBJECTS): %.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<


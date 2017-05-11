CC=gcc
#CC=C:\Tools\tcc\tcc.exe

EXECUTABLE=rob.exe

# To be removed later:
#BUILD=debug

# Add your source files here:
SOURCES=dom.c html.c css.c style.c \
    refcnt.c refalist.c refhash.c \
    stream.c layout.c display.c \
    bmpcanvas.c bmp.c \
    print.c main.c
OBJECTS=$(SOURCES:.c=.o)

CFLAGS=-c -Wall
LDFLAGS=
#LDFLAGS=-mwindows

ifeq ($(BUILD),debug)
  # Debug
  CFLAGS += -O0 -g -I/local/include
  LDFLAGS +=
else
  # Release mode
  CFLAGS += -O2 -DNDEBUG -I/local/include
  LDFLAGS += -s
endif

all: $(EXECUTABLE)
	@echo Done
    
debug:
	@echo Building Debug version...
	@make BUILD=debug
    
$(EXECUTABLE) : $(OBJECTS)
	@echo Linking $@
	@$(CC) $(LDFLAGS) -o $@ $^
	
.c.o:
	@echo Compiling $@
	@$(CC) $(CFLAGS) $< -o $@

stream.o : stream.c stream.h
dom.o : dom.c dom.h refcnt.h refalist.h refhash.h
html.o : html.c html.h refcnt.h dom.h refalist.h refhash.h stream.h
css.o : css.c css.h refcnt.h refalist.h stream.h
style.o : style.c style.h dom.h css.h refhash.h refcnt.h refalist.h
layout.o : layout.c layout.h refcnt.h refalist.h refhash.h style.h dom.h css.h 
display.o : display.c display.h layout.h refcnt.h refalist.h refhash.h style.h dom.h css.h 
canvas.o : bmpcanvas.c canvas.h bmp.h display.h layout.h refcnt.h refalist.h refhash.h style.h dom.h css.h 
main.o : main.c html.h dom.h refalist.h refhash.h print.h \
		stream.h refcnt.h css.h style.h layout.h display.h canvas.h
print.o : print.c print.h html.h dom.h refalist.h refhash.h \
		stream.h refcnt.h css.h style.h layout.h display.h 
refcnt.o: refcnt.c refcnt.h
refalist.o : refalist.c refalist.h refcnt.h
refhash.o : refhash.c refhash.h refcnt.h
bmp.o : bmp.c bmp.h

.PHONY : clean

clean:
	@-rm -f *.o
	@-rm -rf $(EXECUTABLE)
	@-rm -rf *.log
	@echo Cleaned

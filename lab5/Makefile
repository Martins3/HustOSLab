CC = g++
IDIR = ./header
CFLAGS = -g -Wall -I$(IDIR)

ODIR = ./obj
SOURCE_DIR = ./source

FILE =  app.h debug.h io.h kernel.h tools.h


# 添加config.h
_DEPS = $(FILE) config.h 
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

# 添加一个mian.c 
SOURCE = $(patsubst %.h, %.c,$(FILE))
SOURCE_PLUS_MAIN = main.c $(SOURCE)


_OBJ = $(patsubst %.c, %.o ,$(SOURCE_PLUS_MAIN))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o :  $(SOURCE_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main.out: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o
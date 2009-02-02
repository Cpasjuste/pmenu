NAME	  = pmenu

CC        = gcc

LDFLAGS   = -Llib
CFLAGS    = -Werror `sdl-config --cflags` -Iinclude
LIBS	  = -lpnd -lSDL_image -lSDL_mixer -lSDL_ttf -lfreetype `sdl-config --libs`

OBJECTS   = main.o get_apps.o

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
	strip $(NAME)

.cc.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(NAME)

NAME	  = pmenu

ifndef PANDORA
CC        = gcc
STRIP     = strip
else
CC        = arm-none-linux-gnueabi-gcc
STRIP     = arm-none-linux-gnueabi-strip
endif

ifndef PANDORA
CFLAGS    = -Wall -Werror `sdl-config --cflags` -Iinclude
LIBS	  = -lconfig_pc -lpnd_pc -lSDL_image -lSDL_ttf -lSDL_gfx -lfreetype `sdl-config --libs` -lstdc++
else
CFLAGS    = -Wall -Werror `arm-none-linux-gnueabi-sdl-config --cflags` -Iinclude -DPANDORA
LIBS	  = -lconfig_pnd -lpnd_pnd -lSDL_image -lSDL_ttf -lSDL_gfx -lfreetype `arm-none-linux-gnueabi-sdl-config --libs` -lpng -lstdc++ -lts -ldl -lz -lm -static

endif

LDFLAGS   = -Llib


OBJECTS   = main.o graphics.o get_apps.o mouse.o

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
	$(STRIP) $(NAME)

.cc.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(NAME)

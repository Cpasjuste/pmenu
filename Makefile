ifndef PANDORA
CC        = gcc
STRIP     = strip
NAME	  = pmenu_pc
else
CC        = arm-none-linux-gnueabi-gcc
STRIP     = arm-none-linux-gnueabi-strip
NAME	  = pmenu
endif

ifndef PANDORA
CFLAGS    = -Wall -Werror `sdl-config --cflags` -Iinclude
LIBS	  = lib/libconfig_pc.a lib/libpnd_pc.a lib/libsprig_pc.a -lSDL_image -lSDL_ttf -lSDL_gfx -lfreetype `sdl-config --libs` -lstdc++
else
CFLAGS    = -Wall -Werror `arm-none-linux-gnueabi-sdl-config --cflags` -Iinclude -DPANDORA
LIBS	  = -lconfig_pnd -lpnd_pnd -lsprig_pnd -lSDL_image -lSDL_ttf -lSDL_gfx -lfreetype `arm-none-linux-gnueabi-sdl-config --libs` -lpng -lstdc++ -lts -ldl -lz -lm -static

endif

LDFLAGS   = -Llib


OBJECTS   = main.o graphics.o fav_config.o gui_config.o get_apps.o mouse.o

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
	$(STRIP) $(NAME)

.cc.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(NAME)

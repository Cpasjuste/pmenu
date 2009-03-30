PANDORA=1

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
CFLAGS    += -g -Wall `sdl-config --cflags` -Iinclude
#-Werror 
LIBS	  += lib/libconfig_pc.a lib/libpnd_pc.a -lSDL_image -lSDL_ttf -lSDL_gfx -lfreetype `sdl-config --libs` -lstdc++
else
CFLAGS    += -g -Wall `arm-none-linux-gnueabi-sdl-config --cflags` -I$(PNDSDK)/usr/include -Iinclude -DPANDORA -Werror 
LIBS	  += lib/libconfig_pnd.a lib/libpnd_pnd.a -lSDL_image -lSDL_ttf -lSDL_gfx -lfreetype `arm-none-linux-gnueabi-sdl-config --libs` -lpng -lGLES_CM lib/glues/libglues.a -lstdc++

endif

LDFLAGS   = -Llib


OBJECTS   = main.o graphics.o graphics_3d.o fav_config.o gui_config.o setting_config.o get_apps.o mouse.o

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
ifdef PANDORA
	$(STRIP) $(NAME)
endif

.cc.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(NAME)
	rm -f pmenu

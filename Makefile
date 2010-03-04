#
# Pmenu Makefile
#

# tools
CC = ${CROSSCOMPILE}gcc
CXX = ${CROSSCOMPILE}g++
AR = ${CROSSCOMPILE}ar
RANLIB = ${CROSSCOMPILE}ranlib
STRIP = ${CROSSCOMPILE}strip
RM = rm

# environment
LDFLAGS = -L$(PNDSDK)/usr/lib -lSDL -lSDL_image -lSDL_ttf -lfreetype -lGLES2D \
		-lGLES_CM -lIMGegl -lsrv_um -lXau -lXdmcp -lXi -lXext -lX11 \
		-lpnd -lconfig -lts-1.0 -lstdc++

CFLAGS = -Wall -Werror -g -Ilibpnd -I$(PNDSDK)/usr/include
CFLAGS += -O3 -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfloat-abi=softfp \
		-mfpu=neon -ftree-vectorize -ffast-math -fomit-frame-pointer \
		-fno-strict-aliasing -fsingle-precision-constant

# code
TARGET = pmenu
ALLOBJ = category_media.o category_settings.o config_favourite.o \
	config_pmenu.o config_skin.o get_apps.o main.o utils.o \
	utils_cpu.o utils_mplayer.o

all: ${TARGET}

clean:
	${RM} -f ${ALLOBJ} ${TARGET}
	${RM} -rf obj bin
	${RM} -rf deployment
	${RM} -rf sd1/pandora/pmenu
	${RM} -rf sd2/pandora/pmenu
	find . -name "*~*" -exec rm {} \; -print


# component targets
#

pmenu:	${ALLOBJ}
	${CC} ${ALLOBJ} ${LDFLAGS} -o ${TARGET}
	${STRIP} ${TARGET}

# deployment and assembly components
#

deploy: 
	# populate deployment directory for copying into image bakes
	# make dirs
	mkdir -p deployment/etc/pandora/pmenu
	# copy in goodies
	cp -rf skins deployment/etc/pandora/pmenu/
	cp pmenu deployment/etc/pandora/pmenu/
	cp pmenu.cfg deployment/etc/pandora/pmenu/


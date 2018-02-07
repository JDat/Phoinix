###########################################################################
#  Makefile
#			Makefile, needs GNUmake
#
###########################################################################
#
# Phoinix,
# Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
#
# (c)2000-2007 Bodo Wenzel
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
###########################################################################
#  History:
#
#  $Log: Makefile,v $
#  Revision 1.19  2007/02/27 13:28:11  bodowenzel
#  High resolution graphics
#
#  Revision 1.18  2006/10/23 15:39:15  bodowenzel
#  Better compiling of eggs helper program
#
#  Revision 1.17  2006/04/25 18:51:40  bodowenzel
#  Corrections around the easter egg
#
#  Revision 1.16  2005/05/03 08:38:22  bodowenzel
#  Enable compilation from scratch
#
#  Revision 1.15  2005/02/27 16:38:24  bodowenzel
#  Added module Eggs for easter eggs
#
#  Revision 1.14  2004/12/28 13:56:31  bodowenzel
#  Split up all C-Code to multi-segmented
#
#  Revision 1.13  2004/10/24 09:11:05  bodowenzel
#  New button mapping
#
#  Revision 1.12  2004/10/18 17:48:51  bodowenzel
#  Dropped macro LITE, no need to maintain smaller code
#
#  Revision 1.11  2004/01/11 19:04:54  bodowenzel
#  Saving bitmaps again
#  New COPYRIGHT and ADDRESS placeholder
#  Added module 'ram' for on-board memory
#
#  Revision 1.10  2003/04/27 09:36:17  bodowenzel
#  Added Lite edition
#
#  Revision 1.9  2003/04/12 15:23:33  bodowenzel
#  Added GlueLib
#
#  Revision 1.8  2002/12/07 08:57:27  bodowenzel
#  Tweaked button mapping form like proposed by HJ
#
#  Revision 1.7  2002/11/02 15:49:25  bodowenzel
#  Simplified resource compilation; added version and build automatics
#  Moved bitmaps into separate directory
#  Reduced number of button bitmaps to one
#
#  Revision 1.6  2002/10/19 08:08:10  bodowenzel
#  History cleanup, Javadoc-style header
#
#  Revision 1.5  2002/05/10 15:17:39  bodowenzel
#  Till's VFS contribution
#
#  Revision 1.4  2002/01/11 18:14:08  bodowenzel
#  Forgot localized texts
#
#  Revision 1.3  2002/01/05 10:20:57  bodowenzel
#  COPYING and README separated for languages
#
#  Revision 1.2  2001/12/30 18:47:11  bodowenzel
#  CVS keyword Log was faulty
#
#  Revision 1.1.1.1  2001/12/16 13:38:06  bodowenzel
#  Import
#
###########################################################################
#  Pre-CVS History (developed as PalmBoy):
#
#  2001-09-16	Bodo Wenzel	Saving states now possible
#  2001-07-27	Bodo Wenzel	Set up for C version
#  2001-07-12	Bodo Wenzel	Tools now in their own zip
#  2001-03-18	Bodo Wenzel	Changed for Linux, added docs and utils
#  2000-11-17	Bodo Wenzel	Including of Pila files
#  2000-09-30	Bodo Wenzel	PalmBoy logo no longer needed
#  2000-09-17	Bodo Wenzel	Separating runmode.asm
#  2000-05-03	Bodo Wenzel	Splitting in modules, first working version
#  2000-04-22	Bodo Wenzel	Start animation like real Gameboy
#  2000-04-09	Bodo Wenzel	New target "palmboy.tgz" for saving
#  2000-03-11	Bodo Wenzel	Creation, default target "palmboy.prc"
###########################################################################

.PHONY:		all prc clean dist zip

### Shortcuts #############################################################

NAME=		Phoinix
RELEASE=	$(shell echo $(NAME)-`cat version`)
LC_NAME=	$(LC_DIR)/$(NAME)

### Main targets ##########################################################

all:
	@for d in LC_* ; do $(MAKE) LC_DIR=$$d prc ; done

prc:	$(LC_NAME).prc

clean:
	rm -f $(NAME) hidden.inc *.d *.o *~ LC_*/*.rcp LC_*/*.ro LC_*/*~

dist:
	@for d in LC_* ; do $(MAKE) LC_DIR=$$d zip ; done
	$(MAKE) $(RELEASE).tgz

zip:	$(LC_DIR)/$(RELEASE).zip

### if some simple files are missing ######################################

version:
	echo "none" > $@
build:
	echo "1" > $@
copyright:
	echo "Bodo Wenzel" `date "+%Y"` > $@
address:
	echo "(No address available)" > $@

### Programs to use #######################################################

CC=		m68k-palmos-gcc
CPPFLAGS=	-Wall
CFLAGS=		-Wall -O2 -g
ASFLAGS=	-Wall
LDFLAGS=	-Wall -g
LDLIBS=		-lPalmOSGlue
RC=		pilrc
RC_FLAGS=	-q -allowEditID -ro -I bitmaps
BUILD-PRC=	build-prc
BUILD-FLAGS=
ZIP=		zip
TAR=		tar

### Lists of files ########################################################

C_SOURCES=	$(NAME).c misc.c prefs.c manager.c eggs.c states.c	\
		ram.c vfs.c emulation_c.c memory_c.c video_c.c

S_SOURCES=	emulation_s.S gbemu.S memory_s.S video_s.S sections.S

H_SOURCES=	$(NAME).h misc.h prefs.h manager.h eggs.h states.h	\
		ram.h vfs.h emulation.h gbemu.h memory.h video.h	\
		sections.h

INC_SOURCES=	gbemu_op.inc gbemu_cb.inc gbemu_rw.inc gbemu_io.inc	\
		gbemu_xt.inc

RSRC_SOURCES=	bitmaps/icon1.bmp bitmaps/icon2.bmp			\
		bitmaps/icon8.bmp bitmaps/icon8hr.bmp			\
		bitmaps/smic1.bmp bitmaps/smic2.bmp			\
		bitmaps/smic8.bmp bitmaps/smic8hr.bmp			\
		bitmaps/$(NAME).bmp bitmaps/$(NAME)hr.bmp		\
		bitmaps/eggtimer.bmp bitmaps/eggtimerhr.bmp

OBJS=		$(C_SOURCES:.c=.o) $(S_SOURCES:.S=.o)

FILES2TGZ=	COPYING README Makefile					\
		LC_*/GPL.txt LC_*/$(NAME).txt				\
		$(C_SOURCES) $(S_SOURCES) $(H_SOURCES) $(INC_SOURCES)	\
		LC_*/$(NAME).rcp.in $(RSRC_SOURCES)			\
		$(NAME).def $(NAME).ld					\
		version build copyright address

### Hidden game ###########################################################

# The file hidden.gb doesn't have to exist.
hidden.inc:	eggs
	./eggs hidden.gb $@

# This line is needed for a successful make from scratch:
gbemu.d:	hidden.inc

eggs:		eggs.d
	cc eggs.c -o eggs

### Compiling #############################################################

$(LC_NAME).prc:	$(NAME).def $(NAME) $(LC_NAME).ro
	$(BUILD-PRC) $(BUILD-FLAGS) $^ -o $@

$(NAME):	$(OBJS) $(NAME).ld
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	echo "$$((`cat build`+1))" > build

%.d:		%.c
	set -e; $(CC) -MM $(CPPFLAGS) $<				\
	  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@ ;			\
	  [ -s $@ ] || rm -f $@

include $(C_SOURCES:.c=.d)

%.o:		%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.d:		%.S
	set -e; $(CC) -MM $(CPPFLAGS) $<				\
	  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@ ;			\
	  [ -s $@ ] || rm -f $@

include $(S_SOURCES:.S=.d)

%.o:		%.S
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c $< -o $@

$(LC_NAME).ro:	$(LC_NAME).rcp.in $(NAME).h $(RSRC_SOURCES)		\
		version build copyright address
	sed "s/##VERSION##/`cat version`/g" < $(LC_NAME).rcp.in		\
	  | sed "s/##BUILD##/`cat build`/g"				\
	  | sed "s/##COPYRIGHT##/`cat copyright`/g"			\
	  | sed "s/##ADDRESS##/`cat address`/g"				\
	  > $(LC_NAME).rcp
	$(RC) $(RC_FLAGS) -o $@ $(LC_NAME).rcp

### Saving  ###############################################################

$(LC_DIR)/$(RELEASE).zip:	$(LC_DIR)/GPL.txt $(LC_NAME).txt	\
				$(LC_NAME).prc
	cd $(LC_DIR) ;							\
	  $(ZIP) $(RELEASE).zip GPL.txt $(NAME).txt $(NAME).prc

$(RELEASE).tgz:	$(FILES2TGZ)
	rm -rf $(RELEASE)
	mkdir $(RELEASE)
	cp --parent $(FILES2TGZ) $(RELEASE)
	$(TAR) chzf $(RELEASE).tgz $(RELEASE)

### The end ###############################################################

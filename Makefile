#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 3 of the License, or
#       (at your option) any later version.
#       
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#       
#       You should have received a copy of the GNU General Public License
#       along with this program; if not, write to the Free Software
#       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#       MA 02110-1301, USA.

objects = color.o image.o pixel.o draw.o image_save.o point.o font.o \
          image_new_from.o effect.o value.o miscellaneous.o image_fill.o \
          info_cut.o

version = 0.3.0
all_flags = $(flags)
soname = libimel.so.$(version)
aname = libimel.a.$(version)
pkgconfig = imel.pc

prefix = /usr
library_path = $(prefix)/lib
include_path = $(prefix)/include
private_lib = -lfreetype -lfreeimage -lm -lz -lstdc++

freetype_header = -I$(include_path)/freetype2

ifeq ($(debug), true)
      all_flags += -g -Ddebug_enable -pg
endif

ifeq ($(extra), true)
      all_flags += -Wall -Wextra -ansi -pedantic
endif

%.o: imel_src/%.c
	gcc -c $< -o $@ $(all_flags) $(freetype_header)

library: $(objects)
	 gcc -shared -Wl,-soname,libimel.so -o $(soname) $(objects) $(private_lib)
	 ar rcs $(aname) $(objects)
	 @echo -n "Generating .pc file... "
	 @echo -e "prefix=$(prefix)\n"\
	          "exec_prefix=$(prefix)\n"\
	          "libdir=$(library_path)\n"\
	          "includedir=$(include_path)\n"\
	          "\n"\
	          "Name: Imel\n"\
	          "Description: A free and easy image manipulation library\n"\
	          "Version: $(version)\n"\
	          "Requires: \n"\
	          "Libs: -L$(library_path) -limel $(private_lib)\n"\
	          "Libs.private: $(private_lib)\n"\
	          "Cflags: -I$(include_path)/imel -I$(include_path)" > $(pkgconfig)
	 @echo "Done"

clean:
	rm -vf $(objects) $(soname) $(aname) $(pkgconfig)

install: $(soname)
	 install -D $(soname) $(destdir)/$(library_path)/$(soname)
	 link $(destdir)/$(library_path)/$(soname) $(destdir)/$(library_path)/libimel.so
	 install -D $(aname) $(destdir)/$(library_path)/$(aname)
	 link $(destdir)/$(library_path)/$(aname) $(destdir)/$(library_path)/libimel.a
	 install -D imel.h $(destdir)/$(include_path)/imel.h
	 install -D imel_src/header.h $(destdir)/$(include_path)/imel_src/header.h
	 install -D imel_src/image_enum.h $(destdir)/$(include_path)/imel_src/image_enum.h
	 install -D imel_src/freetype_export_types.h $(destdir)/$(include_path)/imel_src/freetype_export_types.h
	 install -D $(pkgconfig) $(destdir)/$(library_path)/pkgconfig/$(pkgconfig)

uninstall:
	 rm -vrf $(destdir)/$(library_path)/$(soname) $(destdir)/$(include_path)/imel.h \
	   $(destdir)/$(include_path)/imel_src $(destdir)/$(library_path)/$(aname) \
	   $(destdir)/$(library_path)/pkgconfig/$(pkgconfig)
	 unlink $(destdir)/$(library_path)/libimel.so
	 unlink $(destdir)/$(library_path)/libimel.a

dev-pkg:
	 make -j2 
	 rm *.o -v
	 tar -cvjf ../imel~dev-pkg.tar.bz2 ../imel

imel-doc:
	cd imel_src && doxygen doxygen.conf && cd ..

imel-doc-clean:
	-rm doc/html/search -rfv 2> /dev/null
	-rm doc/latex/* -fv 2> /dev/null
	-rm doc/latex/images/*.pdf -fv 2> /dev/null
	-rm doc/man -rfv 2> /dev/null
	-rm doc/html/* -fv 2> /dev/null


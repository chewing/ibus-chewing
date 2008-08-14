# vim:set noet ts=4:
#
# ibus-chewing - The Chewing engine for IBus
#
# Copyright (c) 2007-2008 Huang Peng <shawn.p.huang@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

engine_chewing_PYTHON = \
	engine.py \
	factory.py \
	main.py \
	$(NULL)
engine_chewingdir = $(pkgdatadir)

chewing_DATA = \
	chewing.py \
	$(NULL)
chewing_LTLIBRARIES = _chewing.la
chewingdir = @pyexecdir@

_chewing_la_SOURCES = \
	$(NULL)

nodist__chewing_la_SOURCES = \
	chewing_wrap.c \
	$(NULL)

_chewing_la_CFLAGS = \
	@CHEWING_CFLAGS@ \
	-DCHEWING_DATADIR=\"@CHEWING_DATADIR@\" \
	@PYTHON_CFLAGS@ \
	$(NULL)

_chewing_la_LDFLAGS = \
	@CHEWING_LIBS@ \
	-rpath $(chewingdir) \
	-avoid-version \
	-module \
	$(NULL)

bin_SCRIPTS = ibus-engine-chewing
engine_DATA = chewing.engine
enginedir = $(datadir)/ibus/engine

chewing.py chewing_wrap.c: chewing.i
	$(SWIG) -python -I/usr/include -o chewing_wrap.c $(srcdir)/chewing.i

test:
	$(ENV) \
		IBUS_CHEWING_PKGDATADIR=$(abs_top_srcdir)\
		PYTHONPATH=$(builddir)/.libs \
		DBUS_DEBUG=true \
		$(PYTHON) \
		$(srcdir)/main.py

EXTRA_DIST = \
	chewing.i \
	chewing.engine.in \
	ibus-engine-chewing.in \
	$(NULL)

CLEANFILES = \
	chewing.py \
	chewing_wrap.* \
	*.pyc \
	$(MULL)

DISTCLEANFILES = \
	$(MULL)

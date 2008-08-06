# vim:set noet ts=4:
#
# ibus-anthy - The Anthy engine for IBus
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

engine_anthy_PYTHON = \
	engine.py \
	factory.py \
	main.py \
	tables.py \
	test.py \
	$(NULL)
engine_anthydir = $(pkgdatadir)

anthy_DATA = \
	anthy.py \
	$(NULL)
anthy_LTLIBRARIES = _anthy.la
anthydir = @pyexecdir@

_anthy_la_SOURCES = \
	$(NULL)

nodist__anthy_la_SOURCES = \
	anthy_wrap.c \
	$(NULL)

_anthy_la_CFLAGS = \
	@ANTHY_CFLAGS@ \
	@PYTHON_CFLAGS@ \
	$(NULL)

_anthy_la_LDFLAGS = \
	@ANTHY_LIBS@ \
	-rpath $(anthydir) \
	-avoid-version \
	-module \
	$(NULL)

libexec_SCRIPTS = ibus-engine-anthy
engine_DATA = anthy.engine
enginedir = $(datadir)/ibus/engine

anthy.py anthy_wrap.c: anthy.i
	$(SWIG) -python -I/usr/include -o anthy_wrap.c $(srcdir)/anthy.i

test:
	$(ENV) IBUS_ANTHY_LOCATION=$(abs_top_srcdir) PYTHONPATH=$(builddir)/.libs $(PYTHON) $(srcdir)/main.py

EXTRA_DIST = \
	anthy.i \
	anthy.engine.in \
	ibus-engine-anthy.in \
	$(NULL)

CLEANFILES = \
	anthy.py \
	anthy_wrap.* \
	*.pyc \
	$(MULL)

DISTCLEANFILES = \
	$(MULL)

#!/bin/sh

xgettext -ktranslatable -k_ \
    -o po/ibus-chewing.pot \
    src/setup/ibus-setup-chewing-window.ui \
    src/setup/*.c \
    src/*.c

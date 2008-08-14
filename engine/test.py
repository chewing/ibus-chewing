#!/usr/bin/env python
# -*- coding: utf-8 -*-
import chewing
import sys

ctx = chewing.ChewingContext()
ctx.Configure (18, 16, 0, 1, 0);
ctx.set_ChiEngMode(1)
ctx.handle_Default(ord ("g"))
ctx.handle_Default(ord ("j"))
ctx.handle_Space()
ctx.handle_Default(ord ("b"))
ctx.handle_Default(ord ("j"))
ctx.handle_Default(ord ("4"))
ctx.handle_Default(ord ("z"))
ctx.handle_Default(ord ("8"))
ctx.handle_Default(ord ("3"))
ctx.handle_Enter()
for c in ctx.commitStr:
	print c
ctx = None

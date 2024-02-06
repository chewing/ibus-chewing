# IBus-Chewing

IBus-Chewing is an IBus front-end of Chewing, an intelligent Chinese input
method for Zhuyin (BoPoMoFo) users.

Following documents provide further instruction:

 * COPYING: GPLv2 Licence
 * INSTALL: Installation instruction.
 * USER-GUIDE: How to use ibus-chewing

Bugs and enhancement can be filed to either:

 * [Red Hat Bugzilla](https://bugzilla.redhat.com/enter_bug.cgi?product=Fedora&component=ibus-chewing)
 * [GitHub ibus-chewing](https://github.com/definite/ibus-chewing/issues)


# Development

We use LLVM coding style to make our review easier, so if you like to
contribute, please install clang-format and set the git pre-commit hook by:

```sh
ln -s ../../pre-commit .git/hooks/pre-commit
```

## For debugging ibus-chewing

1. Quit existing ibus-daemon
	* By right click on IBus systray icon, then quit, or
	* kill the process, or
	* with command: `ibus exit`

2. Edit `/usr/share/ibus/component/chewing.xml`, add `-v 7` to turn verbose level 7

3. Run ibus-daemon in command line with following command:
```
export G_MESSAGES_DEBUG=all
ibus-daemon -rvx
```

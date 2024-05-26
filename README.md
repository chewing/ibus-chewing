# IBus-Chewing

IBus-Chewing is an IBus front-end of Chewing, an intelligent Chinese input
method for Zhuyin (BoPoMoFo) users.

Following documents provide further instruction:

 * COPYING: GPLv2 Licence
 * INSTALL: Installation instruction.
 * USER-GUIDE: How to use ibus-chewing

Bugs and enhancement can be filed to:

 * [GitHub](https://github.com/chewing/ibus-chewing/issues)


ibus-chewing releases can be verified with the following [minisign][] public key

    RWRzJFnXiLZleAyCIv1talBjyRewelcy9gzYQq9pd3SKSFBPoy57sf5s

For example, to verify the 2.0.0 release

    minisign -Vm ibus-chewing-2.0.0-Source.tar.xz -P RWRzJFnXiLZleAyCIv1talBjyRewelcy9gzYQq9pd3SKSFBPoy57sf5s

[minisign]: https://jedisct1.github.io/minisign/

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

# Translation

There is a weblate instance at
https://translate.fedoraproject.org/projects/ibus-chewing/ for easier
contribution.

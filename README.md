IBus-Chewing is an IBus front-end of Chewing, an intelligent Chinese input
method for Zhuyin (BoPoMoFo) users.

Following documents provides further instruction:

COPYING   : GPLv2 Licence
USER-GUIDE: How to use ibus-chewing
INSTALL   : Installation instruction.

Bugs and enhancement can be filed to either:
 * [Red Hat Bugzilla](https://bugzilla.redhat.com/enter_bug.cgi?product=Fedora&component=ibus-chewing)
 * [GitHub ibus-chewing](https://github.com/definite/ibus-chewing/issues)

We use [K&R coding style](https://en.wikipedia.org/wiki/Indent_style#K.26R_style) to make our review easier, so if you like to contribute, please set the git pre-commit hook by:
```sh
ln -s ../../pre-commit .git/hooks/pre-commit
```

For debugging ibus-chewing:
1. Quit existing ibus-daemon
   By right click on IBus systray icon, then quit, or kill the process.
2. Edit `/usr/share/ibus/component/chewing.xml`, add `-v 7`
   to turn verbose level 7
3. Run ibus-daemon in command line with following command
   ```
   export G_MESSAGES_DEBUG=all
   ibus-daemon -rvx
   ```

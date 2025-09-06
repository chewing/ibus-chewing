# IBus-Chewing

[![CI](https://github.com/chewing/ibus-chewing/actions/workflows/ci.yml/badge.svg)](https://github.com/chewing/ibus-chewing/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/chewing/ibus-chewing/graph/badge.svg?token=r1piKsG5uF)](https://codecov.io/gh/chewing/ibus-chewing)

IBus-Chewing is an IBus front-end of Chewing, an intelligent Chinese input
method for Zhuyin (BoPoMoFo) users.

Following documents provide further instruction:

 * COPYING: GPLv2 Licence
 * INSTALL: Installation instruction.
 * USER-GUIDE: How to use ibus-chewing

Bugs and enhancement can be filed to:

 * [GitHub](https://github.com/chewing/ibus-chewing/issues)


ibus-chewing releases can be verified with the following OpenPGP public key

&emsp;[083B3CAB64267E5BAB7159673EF0C673DADCC30C][pgp_key] Libchewing Signing Key &lt;release@chewing.im&gt;

or the following [minisign][] public key

&emsp;RWRzJFnXiLZleAyCIv1talBjyRewelcy9gzYQq9pd3SKSFBPoy57sf5s

For example, to verify the 2.0.0 release

    gpgv ibus-chewing-2.1.7.tar.xz.asc ibus-chewing-2.1.7.tar.xz

or

    minisign -Vm ibus-chewing-2.1.7.tar.xz \
        -P RWRzJFnXiLZleAyCIv1talBjyRewelcy9gzYQq9pd3SKSFBPoy57sf5s

[pgp_key]: https://chewing.im/.well-known/openpgpkey/hu/y84sdmnksfqswe7fxf5mzjg53tbdz8f5?l=release
[minisign]: https://jedisct1.github.io/minisign/

# Development

We use LLVM coding style to make our review easier, so if you like to
contribute, please install clang-format and set the git pre-commit hook by:

```sh
ln -s ../../pre-commit .git/hooks/pre-commit
```

## Debug ibus-chewing

Run ibus-chewing in command line with verbose debug logging:

    G_MESSAGES_DEBUG=ibus-chewing /usr/libexec/ibus-engine-chewing -v 4

This will temporarily replace the current ibus-chewing engine with
the one launched from the command line.

# Translation

There is a weblate instance at
https://translate.fedoraproject.org/projects/ibus-chewing/ for easier
contribution.

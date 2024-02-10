# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- On non-gnome desktop with systray icon enabled, IBus will display the
  currently selected input mode.

### Changed

- IBus-Chewing Preferences has been rewritten in GTK4 and libadwaita.
- Removed GOB2 dependency. Now gobjects are written in plain C99.
- Removed cmake-fedora dependency. Now CMakeLists.txt only depends on built-in
  modules.
- New CMake presets can be used for building with different configurations.

### Deprecated

- X11 support has been deprecated. IBus-chewing no longer requires X11 to build.
  Running in X11 environment will continue to work as long as IBus still
  supports X11/XIM.

### Removed

- GtkStatusIcon based systray icon has been removed. Now there is only one icon
  provided by IBus to avoid confusion.
- Sync IM status back to CapsLock has been removed.

### Fixed

- Modifier key handling has been improved. Super key will no longer conflict
  with desktop shortcut handling.
- Shift key handling in English mode has been improved. Now Shift+Arrow key will
  work correctly for highlighting text or scroll window.
- Shift_L or Shift_R can be individually asssigned as Eng/Chi toggle key.

## 1.x releases

See [ChangeLog-1.x](./ChangeLog-1.x)

[unreleased]: https://github.com/chewing/ibus-chewing/compare/1.6.2...HEAD
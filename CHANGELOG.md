# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [unreleased]

### Added

- Add Carpalx, Colemak-DH ANSI, and Colemak-DH Orth layout.

### Changed

- Clear pre-edit buffer before commit it to engine to avoid showing both buffer
  on the screen.

## [v2.0.0] - 2024-02-17

### Added

- On non-gnome desktop with systray icon enabled, IBus will display the
  currently selected input mode. Clicking on the icon shows a menu to switch
  input modes.

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

- GtkStatusIcon (X11) based systray icon has been removed. Now there is only one
  icon provided by IBus to avoid confusion. The new systray does not support
  using left mouse button to switch between Chinese and English, and the right
  button to switch between full and half width mode.
- Sync IM status back to CapsLock has been removed which also depends on X11.

### Fixed

- Modifier key handling has been improved. Super key will no longer conflict
  with desktop shortcut handling.
- Shift key handling in English mode has been improved. Now Shift+Arrow key will
  work correctly for highlighting text or scroll window.
- Shift_L or Shift_R can be individually asssigned as Eng/Chi toggle key.

## 1.x releases

See [ChangeLog-1.x](./ChangeLog-1.x)

[v2.0.0]: https://github.com/chewing/ibus-chewing/compare/1.6.2...HEAD

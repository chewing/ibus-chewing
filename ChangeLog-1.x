* Sun Dec 03 2023 Hiunnhue <hiunnhue108 at ymail.com> - 1.6.2
- Enhancement:
    + GitHub Issue: #119 Refresh property list (language bar) only when needed
    + GitHub Issue: #122 Add a setup button to the ibus indicator (and the language bar)
    + GitHub Issue: #131 Port to GTK+-3
      Thank for contribution from cosimoc
    + GitHub Issue: #147 Use arrow keys to choose candidate
    + GitHub Issue: #148 Display page number of the lookup table
    + GitHub Issue: #152 Show cursor on lookup table
    + GitHub Issue: #165 Support vertical lookup table and optimize turn page keys
    + GitHub Issue: #166 Tweak ibus-setup dialog window and replace some deprecated APIs
    + GitHub Issue: #167 Add dropdown menu for Chi-Eng-Mode toggle key
- Fix:
    + GitHub Issue: #130 Destroy *preEditText, *auxText and *outgoingText when disable to prevent leak
    + GitHub Issue: #144, 149 Ignore keystrokes in English mode
- Acknowledge:
    + Cosimo Cecchi contributed pull request #131

* Mon Aug 07 2017 Ding-Yi Chen <dchen at redhat.com> - 1.6.1
- Fixed RHBZ#1411189 - ibus-engine-chewing killed by SIGSEGV when receiving KP_Insert
- Update cmake-fedora to 2.9.3
- Fixed pre-commit so it does not chop directory

* Tue Jul 18 2017 Ding-Yi Chen <dchen at redhat.com> - 1.6.0
- Enhancement:
  + GitHub Issue: #35 Support KB_THL_PINYIN and KB_MPS2_PINYIN
    RHBZ: #1182819: - RFE: Support KB_THL_PINYIN and KB_MPS2_PINYIN
- Fix:
  + GitHub Issue: #51 Hard-coded path in data/ibus-setup-chewing.desktop
  + GitHub Issue: #85 Adding phrase using Ctrl + Num failed to respond
  + GitHub Issue: #93 Remove XTST from CMakeLists.txt

* Thu Jun 16 2016 Ding-Yi Chen <dchen at redhat.com> - 1.5.1
- Enhancement:
- Fix:
  + GH issue #73: Ctrl, Alt, Ins causes the pre-edit duplicate commit
  + GH issue #74: Failed to clean the PreEdit when changing input focus
  + GH issue #79: Failed to input space when "space-as-selection"
  + GH issue #87 Modify dropdown menu for selecting keys
  + GH issue #88 Numpad should be able to select
  + RHBZ #1337185 - Cannot add phrases with Ctrl
    A.k.a GH issue #63
    Thank for contribution from hiunnhue
- Acknowledge:
  + Obata Akio contributed pull request 71, 72, 73
  + hiunnhue contributed pull request 75, 76, 77, 78, 80, 81, 82, 83, 84, 86,
    87, 90, 91, 92, 94
  + hiunnhue fixed issue 73, 74, 79, 87, 88, 90, 91, 92, 94

* Thu May 05 2016 Ding-Yi Chen <dchen at redhat.com> - 1.5.0
- Enhancement:
  * New option: "Clean pre-edit buffer when focus out"
- Fix:
  * Fixed the issues found in static checks
  * Fixed Bug 1182813 ibus-chewing crashes gedit in search box while window out of focus
    a.k.a GitHub issue #28
  * Fixed GitHub issue #37 Cannot use cmake out of source build Cannot use cmake out of source build
  * Fixed GitHub issue #47 CHEWING_DATADIR CMake variable name mismatch
  * Fixed GitHub issue #61 GConf2 should be removed from INSTALL document
  * Fixed GitHub issue #64 systray should be able to show the Chinese/English mode
  * Fixed GitHub issue #68 CPU usage very high when switch to Chewing Engine
  * Fixed Bug 1177198 - [abrt] ibus-chewing: XGetKeyboardControl(): ibus-engine-chewing killed by SIGSEGV
  * Fixed Bug 1319403 - [RFE] ibus-chewing: New option: Caps Lock behavior
    a.k.a. GitHub issue #66
  * Fixed Bug 1330194 - Ctrl-c failed to output as Ctrl-c
    a.k.a. GitHub issue #69
  * Pull Request #67 Fix property "easy-symbol-input" is covered by "shift-toggle-chinese"
    Thanks southernbear for providing this fix.

* Mon Dec 15 2014 Ding-Yi Chen <dchen at redhat.com> - 1.4.14
- Fixed the issues found in static checks

* Thu Dec 11 2014 Ding-Yi Chen <dchen at redhat.com> - 1.4.12
- Fix the GitHub download link

* Thu Dec 11 2014 Ding-Yi Chen <dchen at redhat.com> - 1.4.11.1
- Resolves Bug 1013977 - Slow focus change with ibus-chewing
- Resolves Bug 1062133 - ibus-chewing may not handle key event after focus change
- Resolves Bug 1073797 - Cannot identify input mode for Chinese IME (ibus-chewing)
- Fixed Bug 902866 - ibus-chewing uses dconf but still installs gconf schemas
  (Also listed as GitHub ibus-chewing #36)
- MakerDialog GUI is only loaded when setting dialog is invoked.
- Setting is removed from ibus-properties because it can be launched
  from either:
  + Executable ibus-setup-chewing: Usually under /usr/libexec
  + Input method preference in IBus preference.
- Fixed github issue #43: First space not able to input
    Thanks FreedomKnight.
- Fixed github issue #44: Update input-events (for libchewing-0.4.0)
    Thanks hiunnhue.
- Fixed github issue #45: Optimize properties handling.
    Thanks Ueno.
- github is now hosting the ibus-chewing tarballs.

* Sat Feb 01 2014 Ding-Yi Chen <dchen at redhat.com> - 1.4.10.1
- Resolves Bug 1054937 - Broken defattr in ibus-chewing

* Sat Feb 01 2014 Ding-Yi Chen <dchen at redhat.com> - 1.4.10
- Resolves Bug 1054937 - Broken defattr in ibus-chewing

* Sat Feb 01 2014 Ding-Yi Chen <dchen at redhat.com> - 1.4.9
- Resolves Bug 1054937 - Broken defattr in ibus-chewing

* Thu Jan 16 2014 Ding-Yi Chen <dchen at redhat.com> - 1.4.7
- Target "translation" is built separately with all,
  in order to tame multiple job make.

* Thu Jan 16 2014 Ding-Yi Chen <dchen at redhat.com> - 1.4.6
- Fixed Build for RHEL7

* Wed Jan 15 2014 Ding-Yi Chen <dchen at redhat.com> - 1.4.5
- Resolves Bug 1013977 - ibus-chewing needs to have ibus write-cache --system in post and postun
- Resolves Bug 1027031 - CVE-2013-4509 ibus-chewing: ibus: visible password entry flaw [rhel-7.0]
- Resolves Bug 1028911 - [zh_TW]'Chinese<->English' switch does not work when clicking on the Chewing menu list.
- Resolves Bug 1045868 - ibus-chewing *again* not built with $RPM_OPT_FLAGS
- Option "Sync between caps lock and IM":
  + Default of  is changed to "disable",  because the previous default
    "keyboard" cause bug 1028911 for GNOME Shell.
  + Now Sync from "input method" can control Caps LED in GNOME shell.
- Translation added: de_DE, es_ES, it_IT, pt_BR, uk_UA
- Set environment IBUS_CHEWING_LOGFILE for ibus-chewing log.

* Mon Dec 23 2013 Ding-Yi Chen <dchen at redhat.com> - 1.4.4
- Resolves Bug 842856 - ibus-chewing 1.4.3-1 not built with $RPM_OPT_FLAGS
- Resolves Bug 1027030 - CVE-2013-4509 ibus-chewing: ibus: visible
  password entry flaw [fedora-all]
  Thanks czchen for the GitHub pull request 39.
- Added translations: fr_FR, ja_JP, ko_KR
- Adopt cmake-fedora-1.2.0

* Mon Nov 26 2012 Ding-Yi Chen <dchen at redhat.com> - 1.4.3
- Fixed GitHub issue #30: Rework decorate_preedit
  by merging pull request #30 from buganini
- Fixed GitHub issue #31: Properly refresh property
  by merging pull request #31 from buganini

* Thu Aug 23 2012 Ding-Yi Chen <dchen at redhat.com> - 1.4.2
- Fixed GitHub issue #7: highlighted text be cut after switch back to pure ibus
  by merging pull request #24 from buganini
- Fixed GitHub issue #20: Shift key will send duplicated strings
  by merging pull request #22 from buganini
- Fixed GitHub issue #21: somethings wrong with cmake
- Fixed GitHub issue #25: Weird symbol when input with somethings highlighted
  by merging pull request #26 from buganini
- Fixed GitHub issue #27: Local path committed into tree
- Fixed: Bug 713033 -  [zh_TW] ibus-chewing problem
- Fixed: Bug 745371 -  ibus-chewing: mode confusion In Temporary English mode and Chinese mode later on
- Fixed: Google Issue 1172: [ibus-chewing] move elf file to standard directory.
- Fixed: Google Issue 1426: ibus-chewing-1.3.10 installs directory /gconf to root filesystem
- Fixed: Google Issue 1428: ibus-chewing-1.3.10 fails to save it's settings
- Fixed: Google Issue 1481: Some characters are missing when a long string in preedit buffer.
- Fixed: Google Issue 1490: Cannot change INSTAL prefix for ibus-chewing-1.4.0

* Mon Jul 23 2012 Ding-Yi Chen <dchen at redhat.com> - 1.4.0
- Merge pull request #13 from hiroshiyui to Fix wrong data type conversion
- Fixed: Google Issue 1079: Use shift key to switch to English mode in ibus-chewing
  Also list as GitHub pull request #17
- Fixed: Google Issue 1089: Ibus-chewing cause window flicker when compiz enabled
- Fixed: Google Issue 1329, Github Issue 3: Merge with buganini at gmail.com
- Fixed: Google Issue 1351: ibus-chewing 1.3.10 mistakenly send uncommitted charactor.
- Fixed: Google Issue 1374: ibus-chewing: cannot save the preference with gnomeshell
- Fixed: Google Issue 1427: ibus-chewing-1.3.10 is not compatible with ibus-1.4.0 and higher
  Also list as GitHub pull request #16
- Fixed: GitHub Issue 5: Word missing when with libchewing-0.3.3 and  ibus-chewing 1.3.10
  Also list as GitHub pull request #15
- Fixed: Launchpad bug: 1014456 bus-chewing deletes characters if too many of them are entered
  Also list as GitHub pull request #19

* Thu Dec 15 2011 - Ding-Yi Chen <dchen at redhat.com> - 1.3.10
- Fixed Bug 726335 (Google issue 1281)- [abrt] ibus-chewing-1.3.9.2-3.fc15: g_atomic_int_get:
  Process /usr/libexec/ibus-engine-chewing was killed by signal 11 (SIGSEGV) using patch from Scott Tsai
- Fixed Bug 727018 - ibus compose file needs a symbol tag for gnome-shell
- Fixed characters duplication problem (Google issue 1245, GitHub ibus-chewing issue 2)
- Fixed KP-Enter not been able to commit preedit buffer. (Google issue 1303, GitHub ibus-chewing issue 4)
- Depends on cmake-fedora now.
- Fixed issue 1274, which is addressed by yusake's comments on d9009bf.
- Add compile flag for GNOME3.
- Add command line option: showFlags
- ibus-gnome special symbol
- Thanks Fred Chien's patch against that candidate window cannot be closed
  with escape key since selected tone.
- Thanks Fourdollar's patch for Fix plain zhuyin with space as selection problem.
- Remove support for ibus-1.2 and prior.

* Fri Nov 19 2010 Ding-Yi Chen <dchen at redhat.com> - 1.3.9.2
- Fixed Bug 652909
- Added back a Changlog item that mention Nils Philippsen's change.
- Apply Jim Huang's patch for zh_TW.po

* Fri Nov 12 2010 Ding-Yi Chen <dchen at redhat.com> - 1.3.8
- Quick fix for f15 and ibus-1.4 build
- Version scheme change.

* Fri Sep 10 2010 Ding-Yi Chen <dchen at redhat.com> - 1.3.7.20100910
- Input style of ibus-chewing is decommissioned under ibus >=1.3.
  Now the input style is determined solely on the setting of
  "Embed preedit in application window" in IBus.
- Resolves: #608991, #632043
- Fixed Issue 1022: chewing commit some text in reset method
  (patched by Peng Huang).
- Fixed Issue 1032: [ibus-chewing] Chewing not commit some single Chinese
  char into application when press enter.
- Rewrite CMake modules to make them cleaner, and documents in cmake module
  help format.
- [For developer and distro maintainer]
  Various targets changed. Use 'make help' to obtain a list of available
  targets.

* Fri Jul 30 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.3.6.20100730
- Resolves: #608991
- Sort of fix upstream issue 993.
- Include USER-GUIDE
- Remove NEWS, as this project does not use it.
- Fix upstream Issue 1016: [ibus-chewing] Chewing should commit the complete string before disable chewing. But only for ibus-1.3.0 and later.
- Mouse candidate selection now work in plain Zhuyin mode.
- Default setting changes: (Won't affect current user though).
  + Auto move cursor: TRUE
  + Add phrases in front: TRUE
  + spaceAsSelection: FALSE

* Wed Jul 14 2010 Ding-Yi Chen <dchen at redhat.com> - 1.3.5.20100714
- Resolves: #608991
- Removes Ctrl-v/V Hotkey

* Wed Jul 07 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.3.5.20100706
- Fixed google issue 965:
  Candidate missing if both "Plain Zhuyin" and "Space As selection" are enabled.
- Revised Basic.macro
- Resolved: #608991

* Tue Jun 08 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.3.4.20100608
- ibus-chewing can now use mouse to click on mouse. Thus
  Fix Issue 951: Chewing does not support selection of candidates via mouseclick
  Thanks zork@chromium.org for the patch.

* Fri Jun 04 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.3.4.20100605
- Fix Issue 942: Fix unsunk references in ibus-chewing
  Applied the patch provided by zork@chromium.org.
- Rename CVS_DIST_TAGS to FEDORA_DIST_TAGS, and move its
  definition to cmake_modules/
- Gob2 generated file is now removed, because
  Bug 519108 is fixed from Fedora 11.

* Wed Mar 17 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.2.99.20100317
- Fix google 796: English input for dvorak
- Fix google 797: Zhuyin input for dvorak
- Fix google 807: ibus-chewing shows the over-the-spot panel
  even when not necessary

* Fri Feb 19 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.2.99.20100217
- Fixed the CMake description that leads summary incorrect.

* Tue Feb 16 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.2.99.20100216
- Fixed when typing English immediately after incomplete Chinese character.
- Add zh_TW summary.
- Revised description and write its zh_TW translation.

* Tue Feb 16 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.2.99.20100215
- "Macroize" rpm spec.
- Resolves: #565388

* Fri Feb 12 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.2.99.20100212
- Fixed Google issue 505.
- Google issue 755 is fixed in libchewing-0.3.2-22,
  See Chewing Google issue 10
- Fixed behavior of Del, Backspace,  Home, End
- Revert the change that fix Google issue 758.
- Change the default input style to "in candidate window",
  because not all application handle the on-the-spot mode well.
- Fixed Google issue 776

* Tue Feb 09 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20100210
- Revert the change that fix Google issue 758.
- Remove "tag" target, add "commit" which do commit and tag.

* Tue Feb 09 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20100209
- Fixed Google issue 754: commit string is missing when inputting
  long English text in the end.
- Fixed Google issue 758: Space is irresponsive in Temporary English mode
  if no Chinese in preedit buffer.
- Fixed Google Issue 763: [ibus-chewing] [qt] Shift-Up/Down does not mark
  text area properly.
- Change the String "on the spot" to "in application window",
  Chinese translation change to "在輸入處組詞"
- Change the "over the spot" to "in candidate window",
  Chinese translation remain the same
- Fixed bodhi submission.

* Mon Jan 25 2010 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20100125
- Add over-the-spot editing mode.
- Possible fixed of Google issue 505: ibus acts strange in Qt programs.
- Implemented Google issue 738:  Add a mode that allow editing in candidate window
  (thus over-the-spot mode).

* Fri Dec 11 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20091211
- Fix Google issue 608: ibus-chewing does not show cursor in xim mode.
- Fix Google issue 611: ibus-chewing keyboard setting reverts to default unexpectlly.
- Fix Google issue 660: failed to build with binutils-gold.
- Remove make target commit.
- Add make target tag

* Fri Oct 02 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20091002
- Bug 518901 - ibus-chewing would not work with locale zh_TW.Big
- Fix Google issue 501: ibus-chewing buffer doesn't get cleared when
toggling ibus on/off
- Fix Google issue 502: ibus-chewing: character selection window stays
behind when toggling ibus off- Use WM's revised ibus-chewing icon.
- Debug output now marked with levels.

* Thu Sep 17 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20090917
- Addressed Upstream (IBUS Google code) issue 484:
  + Find the source that why the / and . are not working.
- Pack the gob2 generation source to avoid the [Bug 519108]:
  [gob2] class and enum names convert incorrectly in mock / koji.

* Wed Sep 09 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20090831
- IBusProperty and IBusPropList are free upon destruction.
- Fixed Red Hat Bugzilla [Bug 519328] [ibus-chewing] inconsistent between normal mode and plain Zhuyin mode.
- Addressed Upstream (IBUS Google code) issue 484:
  Arithmetic symbols (+-*/) on number pad does not input properly.

* Wed Aug 26 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20090818
- Merged 1.2 and 1.1 source code.
- Addressed Upstream (IBUS Google code) issue 471.
- Remove libX11 dependency.

* Mon Aug 17 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20090624
- Lookup table now shows the selection key.

* Mon Jun 22 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.2.0.20090622
- For IBus 1.2
- Revised dialog look.

* Fri May 22 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.0.10.20090523
- Applied Lubomir Rintel's patch

* Fri May 22 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.0.10.20090522
- Now the 1st down key brings the longest possible phrases.
  The 2nd down key brings the 2nd longest possible phrases from the back,
  unlike the previous versions where the cursor stays in the head of longest phrase.
- Add force lowercase in English mode option.
- Fix double free issue when destroy ibus-chewing.
- Hide ibus-chewing-panel when ibus-chewing is focus-out

* Mon May 11 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.0.9.20090508
  Now commit is forced when switch of ibus-chewing or current application loses focus.
- New ibus-chewing.png is contribute by WM.
- input-keyboard.png is no longer needed and removed.
- ibus-engine-chewing -v option now need an integer as verbose level.
- ibus-chewing.schemas is now generated.
- Fix some CMake modules bugs.

* Tue Apr 28 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.0.8.20090428
Fix the errors which Funda Wang as pointing out:
- Move src/chewing.xml.in to data/
- Fixed some typo in next_version targets.
- Remove GConf2 package requirement, while add gconftool-2 requirement.

* Mon Mar 30 2009 - Ding-Yi Chen <dchen at redhat.com> - 1.0.5.20090330
- Added tooltips.
- Revealed the sync caps lock setting.
- Fixed Right key bug.
- Added CMake policy 0011 as OLD.

* Mon Mar 23 2009 Ding-Yi Chen <dchen at redhat.com> - 1.0.4.20090323
- Various Settings are now in dialog.
- Integer settings are now revealed.
- MakerDialog.gob is now available.
- Work around of easy symbol input.
- Fix iBus Google issue 310.

* Wed Mar 11 2009 Ding-Yi Chen <dchen at redhat.com> - 1.0.3.20090311
- IBus Google issue 305:  ibus-chewing.schema -> ibus-chewing.schemas
- IBus Google issue 307:  hardcoded chewing datadir
- Sync chewing candPerPage and IBusTable->page_size
- Sync between IM and keyboard (Experimental)

* Tue Mar 03 2009 Ding-Yi Chen <dchen at redhat.com> - 1.0.2.20090303
- Required gconf2 -> GConf2.
- Fix RPM scriptlet.

* Fri Feb 27 2009 Ding-Yi Chen <dchen at redhat.com> - 1.0.1.20090227
- Add gconf schema.
- Fix some memory leaking checked.
- Move some function to cmake_modules.
- Fix Google code issue 281

* Fri Feb 20 2009 - Ding-Yi Chen <dchen at redhat.com>  - 1.0.0.20090220
- This is a barely working version for with IBus C.
- The build system is converted from autotool to CMake.

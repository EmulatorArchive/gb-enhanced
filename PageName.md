# 0.5 Current Status #

This is the current status of GB Enhanced:
  * Emulated Z80 CPU implemented
  * Emulated LCD implemented (Background, Window, Sprites in 8x8 and 8x16)
  * Keyboard input
  * Cartridge Types 0x0 & 0x1 (ROM Only & ROM+MBC1)

In short, only enough right now to start playing a number of games from start to finish. A lot of ground has already been covered, however, thus the 0.5 release.

# 1.0 Milestones #
  * ~~Implement sound~~ : Done
  * ~~Add joystick input~~ : Done
  * ~~Optionally use OpenGL for blitting instead of SDL~~ : Done
  * ~~Implement various cartridge types~~ :  Done
  * ~~Re-implement support for custom sprites~~ : Done
  * ~~Support reading options from a .ini file~~ : Done

The above tasks must be done before the project can be considered 1.0 release worthy. Most of them aren't especially difficult. Sound is the only sticking point.

# Future Milestones #
  * Implement serial transfer emulation (requires SDL\_net)
  * Custom GUI (GBE is CLI only at the moment)
  * Emulate the Game Boy Color (down the road...)
  * Create advanced debugging features

These tasks aren't as immediate and should be the focus of releases after 1.0. As the project moves on, future milestones will turn into 2.0 milestones, and more milestones are sure to be added.

# Long term/continuing goals #
  * Implement various scaling filters
  * Improve the codebase
  * Improve emulator accuracy

These goals may really have no objective end to them, however they are always worth pursuing.
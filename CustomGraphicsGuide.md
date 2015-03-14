# What are custom graphics? #

Custom graphics are graphics that the user can dynamically replace with his or her own pixel data. Game Boy Enhanced allows for users to "dump" or "rip" sprites and background tiles directly from the game. The user can then edit these graphics and tell Game Boy Enhanced to load them when playing a game.

This method allows for users to easily modify a game's graphical elements without having to actually edit the game's code itself. The game's graphics can merely be altered to show a different palette, fully colorize old monochrome sprites, or create entirely new graphics altogether.

# How do I dump/rip sprites? #

To dump a game's sprites, add the **--dump\_sprites** argument when running GBE from the command-line, or edit this line in the **gbe.ini** file: http://code.google.com/p/gb-enhanced/source/browse/src/gbe.ini#39

The sprites will automatically be dumped into Game Boy Enhanced's **Dump/Sprites** folder.

# How do I dump/rip background tiles #

Dumping background tiles must be done manually, unlike sprites which are done automatically. To dump a game's background tiles, add the **--dump\_sprites** argument when running GBE from the command-line, or edit this line in the **gbe.ini** file: http://code.google.com/p/gb-enhanced/source/browse/src/gbe.ini#39

When playing a game, move the mouse over the window. Game Boy Enhanced will now highlight a specific tile on-screen. Any other tiles that are the same as the one under the mouse cursor will also be highlighted. Once the user has chosen a tile to dump, left-clicking once will dump that tile to Game Boy Enhanced's **Dump/BG** folder.

This method provides a way to interactively select tiles. Dumping hundreds of background tiles at once can be confusing since Game Boy Enhanced does not order them in any way, and it may be hard for users to decide what each 8x8 tile represents in a game. Using this way, users can dump the exact tiles they're looking for while displaying which parts of the game the dumped tile is used in.

# How do I edit sprites/background tiles? #

After dumping the graphics, use an image editor to change the dumped sprites/background tiles to your desire. There are a few considerations to take into account, however.

**All** image files must have an alpha-channel. For example, if a sprite was dumped by Game Boy Enhanced, it will not load correctly unless an alpha-channel is manually added to that image file. Game Boy Enhanced does not currently dump sprites or background tiles with their own alpha-channel. Adding an alpha-channel in something like GIMP is as simple as going to Layer -> Transparency -> Add Alpha Channel.

**All** image files must be in BMP format. Saving to other formats in SDL is not a straightforward task, even though loading them is. However, given that storing hundreds or thousands of them won't take up much more space than a few measly megabytes, this isn't a pressing concern.

**All** images file must be the same size as the originals. Game Boy Enhanced does not yet support "HD" custom graphics that are larger than the game's native graphics. This is expected to change some time in the future however.

Sprites can have their own "transparency color" just like the original Game Boy hardware, that is to say the user can specify a 32-bit ARGB value that Game Boy Enhanced will ignore when drawing a sprite. By default, this color is pure green in ARGB (#FF00FF00 or just #00FF00 in HTML/CSS) because no one should ever use that color anyway. This "transparency color" can be changed in the **gbe.ini** file.

**It is highly recommended that users DO NOT edit background tiles that are solid colors.** If a tile is one solid color (e.g. all-black or all-white), these are often poor candidates for re-coloring or editing. Most games use these solid colors to fill in various areas of the game. If an all-white background tile is edited to something like all-blue (say to color the sky) it may have the unintended consequences of editing other areas (such as the ground or menus) that use that same all-white tile. Game Boy Enhanced treats any instance of the all-white background tile as the same tile; it cannot distinguish whether it belongs in the sky, on the ground, or somewhere else. Artistically, it is best to avoid editing these areas and work around
them.

# How do I load custom sprites/background tiles? #

After dumping the graphics and editing the images, the files must placed in their corresponding folders in Game Boy Enhanced's **Load** directory. Edited sprites go in **Load/Sprites**. Edited background tiles go in **Load/BG**.

Add the **--load\_sprites** argument when running GBE from the command-line, or edit this line in the **gbe.ini** file: http://code.google.com/p/gb-enhanced/source/browse/src/gbe.ini#44

# Can I dump and load sprites at the same time? #

No.


# Why does Game Boy Enhanced pause sometimes when loading custom graphics? #

Game Boy Enhanced may encounter a temporary bottleneck when loading a sudden large amount of custom pixel data. There are opportunities for optimization in the future, however.

# Why does Game Boy Enhanced seem to use more CPU resources when dumping or loading custom graphics? #

The main culprit here is the fact that all of the graphics need to be accurately hashed before dumping or loading them. While the hashing itself is fast enough, it actually has to be done very frequently based on when the Game Boy's emulated VRAM is updated.

# Will custom graphics work with Game Boy Enhanced's filters? #

Yes. The graphics are changed before the filter is applied.

# Can any game use custom graphics? #

Yes. Currently, however, only DMG and DMG-compatible GBC titles support them, simply because Game Boy Enhanced does not support the Game Boy Color yet. When GBC support is added, custom graphics support is expected to carry over.

# Why are some of the graphics completely blank when I dump them? #

Sometimes the graphics aren't ready to be drawn to the Game Boy's emulated LCD screen at the time Game Boy Enhanced dumps them. Most of the time this will result in image files that are blank or may not look complete. When editing custom graphics, feel free to completely ignore these and use only the graphics that are "finished" and recognizable.
NEXTSTEPS
=========

Each Map has a Tile Palette associated with it

Tile Palettes
- 16 bytes
- tells the tile loader what graphics to load
- the position of each tile matters:
  - each pair of tiles share a GBC color palette
  - the odd tiles use 0 and 1 for their color
  - the even tiles use 1 and 2 for their color

Tile Loader
- all tiles are stored in ROM as 1bpp where the values are always 0 and 1
- the tile loader takes a Tile Palette and copies the tiles to VRAM
  - when it copies an even tile it maps each bit so that 0 -> 2 and -> 1 -> 3

When updating the VRAM we use the 4 bit tile index to determine
which color palette to use

Tile Game Data
- another 16 bytes that stores information like collision data
  so that we can look up collision data with the same 4 bits
  we use for graphics
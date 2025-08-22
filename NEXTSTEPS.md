NEXTSTEPS
=========

OK so now the problem I want to solve is that I want to be able to have

a level with
- blocks you push

a level with
- blocks you can push two of at a time

a level with
- mobs that hunt you and can't push... or that can!

I want to be able to compose these behaviours on the fly... and this reminds me a lot of ECS but
I am unfamiliar and must spend some time imagining the code I wish I had.

Suppose we have a hard limit of 16 entities. The entities
are indexes and we spin up arrays to hold the properties that exist.
How do we do just a simple block pushing puzzle in this way?

tic
Process input, "record intent" (ie update player dx, dy)

movement system
- for each mover, look ahead in the direction of movement
  - if we see a pushable we mark it (and decrement push strength)
    - or if push strength is zero we abort the move
  - if we see an empty space we commit the move
  - if we see a wall we abort the move

What about logs. If we push them they fall over, or stand up. If we roll them they roll.
it's a class of objects that all behave the same when when pushed.

If we push a stone into the water it sinks and can no longer be pushed, but
now it is collidable

I feel like what I want is a kind of "consequences engine" with

materials and their properties?
and exceptions.

ie
floats / sinks (with on sink, on float callbacks)
so you push a big rock, it sinks, and on sink it transitions to be immovable but still collides
you push a log, it floats, and now it behaves differently
you push a log into a fire and it burns

maybe we just have to iterate over each object, doing an update cascade, 
with each of them being a state machine

player inputs a move
- iterate over every entity that can move and mark the intent 
- iterate over the entities with intents and update their neighbours
- cascade out until we have an iteration where nothing gets marked
- update everything one time step (ie a log rolls 1 tile)
- continue back and forth, consequence search, update, search, update...
  until we reach a stillness
- then get another input
- (allow the player to reset the room at any time)




---

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
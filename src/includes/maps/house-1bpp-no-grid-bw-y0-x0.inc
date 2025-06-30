IF !DEF(TEST_MAP_INC)
DEF TEST_MAP_INC EQU 1

Section "House1bppNoGridBwY0X0Map", ROM0

Start: ; marks the "initial map"
House1bppNoGridBwY0X0Map:
  db HIGH(House1bppNoGridBwY0X0MapTileset), LOW(House1bppNoGridBwY0X0MapTileset), 
  db HIGH(House1bppNoGridBwY0X0MapDMGBGPalettes), LOW(House1bppNoGridBwY0X0MapDMGBGPalettes), 
House1bppNoGridBwY0X0MapData:
; this file will be generated
INCLUDE "maps/house-1bpp-no-grid-bw-y0-x0-data.inc"

House1bppNoGridBwY0X0MapTileset:
db EMPTY_TILE, FLOWER_TILE, GRASS_TILE, ROAD_TILE, SIGN_TILE, TREE_ONE_TILE, TREE_TWO_TILE, WALL_TILE
db WATER_TILE, WATER_EDGE_TILE, BRIDGE_TILE, CLIFF_TILE, CLIFF_EDGE_TILE, CAVE_TILE, UNUSED_TILE , DOOR_TILE, END_OF_TILESET

; specifies one pair of colors per tile
; the low value, and the high value
; 2 pairs are read into each of 8 palettes
; so we have 16 distinct color pairs
; the "low" color and the "high" color
House1bppNoGridBwY0X0MapDMGBGPalettes:
dw GRASS_GREEN, GRASS_GREEN
dw GRASS_GREEN, YELLOW_FLOWER
dw GRASS_GREEN, TALL_GRASS
dw GRASS_GREEN, DIRT_ROAD
dw GRASS_GREEN, WOOD_SIGN
dw GRASS_GREEN, HEALTHY_TREE
dw GRASS_GREEN, SKINNY_TREE
dw STONE_WALL, STONE_WALL_BRICK

dw OCEAN_BLUE, OCEAN_BLUE
dw OCEAN_BLUE, DIRT_FLOOR
dw OCEAN_BLUE, BRIDGE_BROWN
dw DIRT_FLOOR, STONE_WALL_BRICK
dw GRASS_GREEN, DIRT_FLOOR
dw STONE_WALL_BRICK, BLACK
dw BLACK, BLACK
dw STONE_WALL, DIRT_FLOOR


ENDC

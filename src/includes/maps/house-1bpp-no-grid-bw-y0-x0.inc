IF !DEF(TEST_MAP_INC)
DEF TEST_MAP_INC EQU 1

Section "House1bppNoGridBwY0X0Map", ROM0

Start: ; marks the "initial map"
House1bppNoGridBwY0X0Map:
  db HIGH(House1bppNoGridBwY0X0MapTileset), LOW(House1bppNoGridBwY0X0MapTileset), 
House1bppNoGridBwY0X0MapData:
; this file will be generated
INCLUDE "maps/house-1bpp-no-grid-bw-y0-x0-data.inc"

House1bppNoGridBwY0X0MapTileset:
db EMPTY_TILE, FLOWER_TILE, GRASS_TILE, ROAD_TILE, SIGN_TILE, TREE_ONE_TILE, TREE_TWO_TILE, WALL_TILE
db WATER_TILE, WATER_EDGE_TILE, BRIDGE_TILE, CLIFF_TILE, CLIFF_EDGE_TILE, CAVE_TILE, UNUSED_TILE , DOOR_TILE, END_OF_TILESET

ENDC

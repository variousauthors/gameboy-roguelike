IF !DEF(GRAPHICS_INC)
DEF GRAPHICS_INC EQU 1

SECTION "TileDataUtils", ROM0

; @param bc - address of the tile set to load

; each tile set is 16 tiles long, each entry 1 byte
; each tile set must end with 0xFF to indicate end of tileset
LoadMapTileset:
  ; iterate along bc until we hit END_OF_TILESET
  ; at each step, copy 8 bytes from some hl to VRAM
  ; TileData is aligned to 8 bytes so e is 0x00

  ; destination
  ld hl, $9000
.loop
  ld de, TileData;
  ld a, [bc]
  cp a, END_OF_TILESET
  jr z, .done ; we hit end of tileset

  ; copy 1 tile
  ld e, a ; now e is the low portion of the address of a tile

  ; otherwise copy 8 bytes
  REPT 8
    ; Mem copy 1 tile, 8 bytes
    ld a, [de]
    ld [hli], a

    ; second byte same as the first
    ld a, [de]
    ld [hli], a
    inc de
  ENDR

  inc bc

  jr .loop
.done

  ret

/** 256 tiles, each 8x8 1bpp = 8 bytes */
SECTION "TileData", ROM0, ALIGN[8]

TileData:

DEF UNUSED_TILE EQU 0
DEF RESERVED EQU 0
DEF END_OF_TILESET EQU $FF

; 16 tiles
OverworldTiles:
; the start address of the tile
RSRESET
DEF EMPTY_TILE RB $8
DEF FLOWER_TILE RB $8
DEF GRASS_TILE RB $8
DEF ROAD_TILE RB $8
DEF SIGN_TILE RB $8
DEF TREE_ONE_TILE RB $8
DEF TREE_TWO_TILE RB $8
DEF WALL_TILE RB $8
DEF WATER_TILE RB $8
DEF WATER_EDGE_TILE RB $8
DEF BRIDGE_TILE RB $8
DEF CLIFF_TILE RB $8
DEF CLIFF_EDGE_TILE RB $8
DEF CAVE_TILE RB $8
DEF _UNUSED_1 RB $8
DEF DOOR_TILE RB $8

INCBIN "assets/prototype/house-1bpp-no-grid-bw.1bpp"
OverworldTilesEnd:

Tiles:
  ; floor
	dw `00000000
	dw `00000000
	dw `00000000
	dw `00000000
	dw `00000000
	dw `00000000
	dw `00000000
	dw `00000000

  ; wall
	dw `33333333
	dw `33333333
	dw `33333333
	dw `33333333
	dw `33333333
	dw `33333333
	dw `33333333
	dw `33333333

  ; tree
	dw `01111100
	dw `11111110
	dw `11111110
	dw `11111110
	dw `01111100
	dw `02222000
	dw `00220000
	dw `02222000

  ; wall
	dw `22222222
	dw `22222222
	dw `22222222
	dw `22222222
	dw `22222222
	dw `22222222
	dw `22222222
	dw `22222222
TilesEnd:

SECTION "Tilemap", ROM0

Tilemap:
  db $02, $02, $02, $02, $02, $00, $03, $00, $00, $00, $00, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $02, $02, $00, $03, $00, $00, $00, $02, $00, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $02, $00, $00, $03, $00, $00, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $00, $03, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $03, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
  db $02, $02, $02, $00, $03, $00, $00, $00, $02, $02, $02, $02, $02, $02, $00, $02, $02, $02, $02, $02, 0,0,0,0,0,0,0,0,0,0,0,0
TilemapEnd:

PlayerSprites:
	dw `22222222
	dw `21111112
	dw `21111112
	dw `21111112
	dw `21111112
	dw `21111112
	dw `21111112
	dw `22222222

	dw `11111111
	dw `12222221
	dw `12111121
	dw `12111121
	dw `12111121
	dw `12111121
	dw `12222221
	dw `11111111

	dw `11111111
	dw `11111111
	dw `11222211
	dw `11211211
	dw `11211211
	dw `11222211
	dw `11111111
	dw `11111111

	dw `11111111
	dw `11111111
	dw `11111111
	dw `11122111
	dw `11122111
	dw `11111111
	dw `11111111
	dw `11111111
PlayerSpritesEnd:

MonsterSprites:
	dw `11111111
	dw `12222221
	dw `12222221
	dw `12222221
	dw `12222221
	dw `12222221
	dw `12222221
	dw `11111111

	dw `22222222
	dw `21111112
	dw `21222212
	dw `21222212
	dw `21222212
	dw `21222212
	dw `21111112
	dw `22222222

	dw `22222222
	dw `22222222
	dw `22111122
	dw `22122122
	dw `22122122
	dw `22111122
	dw `22222222
	dw `22222222

	dw `22222222
	dw `22222222
	dw `22222222
	dw `22211222
	dw `22211222
	dw `22222222
	dw `22222222
	dw `22222222
MonsterSpritesEnd:

ENDC
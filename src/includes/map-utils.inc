IF !DEF(MAP_INC)
DEF MAP_INC EQU 1

SECTION "MapVariables", WRAM0

CURRENT_MAP_HIGH_BYTE: ds 1
CURRENT_MAP_LOW_BYTE: ds 1

; instead of seeking the map each time
; we will store the pointer when we change
; maps... this will also allow us to change
; just the attributes without changing the
; tiles for different "modes of travel" 
CURRENT_MAP_TILESET_HIGH_BYTE: ds 1
CURRENT_MAP_TILESET_LOW_BYTE: ds 1

SECTION "MapUtilities", ROM0

/*
OK so the plan is to cut the map data up into 20 x 18 rects
each rect will be a "map" and when we want to load a map
we just copy that rect into the VRAM
*/
LoadMapTileMap:
  call getCurrentMapTileMap
  ld hl, $9800

  ld c, 18
.loop
  call LoadMapTileMapOneRow
  dec c
  jr nz, .loop
.done

  ret

; @param de - source
; @param hl - destination
; @destroys b
LoadMapTileMapOneRow:
  ld b, 20
.copyOneRow
  ld a, [de]
  ld [hli], a
  inc de

  dec b
  jr nz, .copyOneRow
.copyOneRowDone

  ; get hl ready for the next row
  ld b, 12
.advance
  inc hl

  dec b
  jr nz, .advance
.done2

  ret

; @param de - address of the tile set to load
; each tile set is 16 tiles long, each entry 1 byte
; each tile set must end with 0xFF to indicate end of tileset
LoadMapTileset:
  ; iterate along bc until we hit END_OF_TILESET
  ; at each step, copy 8 bytes from some hl to VRAM
  ; TileData is aligned to 8 bytes so e is 0x00

  call getCurrentMapTileset

  ; destination
  ld hl, $9000
.loop
  ld bc, OverworldTiles;
  ld a, [de]
  cp a, END_OF_TILESET
  jr z, .done ; we hit end of tileset

  ; copy 1 tile
  ld c, a ; now e is the low portion of the address of a tile

  ; otherwise copy 8 bytes
  REPT 8
    ; Mem copy 1 tile, 8 bytes
    ld a, [bc]
    ld [hli], a

    ; second byte same as the first
    ld a, [bc]
    ld [hli], a
    inc bc
  ENDR

  inc de

  jr .loop
.done

  ret

/** load the tile palette for the map
 * copy the map data to VRAM */
; @pre - LCD is OFF
fullMapRedraw:
  call assertLCDOff

  call LoadMapTileset
  call LoadMapTileMap

  ret

; @return de - address of current map
getCurrentMap:
  ld hl, CURRENT_MAP_HIGH_BYTE
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ld d, h
  ld e, l

  ret

; @return de - address of tile map for current map
getCurrentMapTileMap:
  call getCurrentMap
  ret

; @return de - address of tileset for current map
getCurrentMapTileset:
  ld hl, CURRENT_MAP_TILESET_HIGH_BYTE
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ld d, h
  ld e, l

  ret

; @param de - address of current map
; @return void CURRENT_MAP pointer is set to de
setCurrentMap:
  push de

  ; set the current map
  ld hl, CURRENT_MAP_HIGH_BYTE
  ld a, d
  ld [hl], a

  ld hl, CURRENT_MAP_LOW_BYTE
  ld a, e
  ld [hl], a

  ; set the current tileset using the map
  call getCurrentMap
  call getMapTilesetFromMetadata
  call setCurrentMapTileset

  pop de

  ret

; @param de - address of map
; @return de - address of map tileset
getMapTilesetFromMetadata:
  ; advance to the tileset pointer
  ; @DEPENDS on MAP_METADATA_SIZE
  ; no need to advance, it's the first field

  ; dereference the pointer
  ld a, [de]
  ld h, a
  inc de
  ld a, [de]
  ld l, a

  ld d, h
  ld e, l

  ret

; @param hl - tileset 
setCurrentMapTileset:
  ld hl, CURRENT_MAP_TILESET_HIGH_BYTE
  ld a, d
  ld [hl], a

  ld hl, CURRENT_MAP_TILESET_LOW_BYTE
  ld a, e
  ld [hl], a

  ret

initCurrentMap:
  ld de, Start
  call setCurrentMap

  ret

ENDC

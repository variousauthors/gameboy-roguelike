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

CURRENT_MAP_TILEMAP_HIGH_BYTE: ds 1
CURRENT_MAP_TILEMAP_LOW_BYTE: ds 1

CURRENT_MAP_TILESET_METADATA_HIGH_BYTE: ds 1
CURRENT_MAP_TILESET_METADATA_LOW_BYTE: ds 1

CURRENT_MAP_PALETTE_HIGH_BYTE: ds 1
CURRENT_MAP_PALETTE_LOW_BYTE: ds 1

SECTION "MapUtilities", ROM0

/** loads the tile with BGP equal to
 * the tile index into the tileset / 2
 * so every 2 tiles have one palette */
LoadMapTileMapAttributes:
  call getCurrentMapTileMap
  ld de, $9800

  ; select GBC bank 1, tile attributes
  ld a, 1
  ld [rVBK], a

  ld c, 18
.loop
  call LoadMapTileMapAttributesOneRow
  dec c
  jr nz, .loop
.done

  ; reset bank
  ld a, 0
  ld [rVBK], a

  ret

; @param hl - source, the tilemap
; @param de - destination
; @destroys b
LoadMapTileMapAttributesOneRow:
  ld b, 20
.copyOneRow
  ld a, [hli]
  srl a ; now a is in 0 - 7
  ld [de], a
  inc de

  dec b
  jr nz, .copyOneRow
.copyOneRowDone

  ; get hl ready for the next row
  ld b, 12
.advance
  inc de

  dec b
  jr nz, .advance
.done2

  ret

/*
OK so the plan is to cut the map data up into 20 x 18 rects
each rect will be a "map" and when we want to load a map
we just copy that rect into the VRAM
*/
LoadMapTileMap:
  call getCurrentMapTileMap
  ld de, $9800

  ld c, 18
.loop
  call LoadMapTileMapOneRow
  dec c
  jr nz, .loop
.done

  ret

; @param hl - source
; @param de - destination
; @destroys b
LoadMapTileMapOneRow:
  ld b, 20
.copyOneRow
  ld a, [hli]
  ld [de], a
  inc de

  dec b
  jr nz, .copyOneRow
.copyOneRowDone

  ; get hl ready for the next row
  ld b, 12
.advance
  inc de

  dec b
  jr nz, .advance
.done2

  ret

LoadMapPalette:
  ; arrange for BCPD to point to the first palette
  ld de, rBCPS
  ld a, %10000000 ; auto increment
  ld [de], a

  ; load the source
  call getCurrentMapPalette

  ld de, rBCPD ; set the destination

  ld b, 8
.loop
  ; copy one palette, 8 bytes
  REPT 8
    ld a, [hli]
    ld [de], a
  ENDR
  
  ; BCPD is incremented internally
  dec b
  jr nz, .loop
.done

  ret

; @param hl - address of the tile set to load
; each tile set is 16 tiles long, each entry 1 byte
; each tile set must end with 0xFF to indicate end of tileset
; loads only the event tiles, so that odd and even tiles
; use different halves of the palette
LoadMapTilesetEvenTiles:
  ; iterate along bc until we hit END_OF_TILESET
  ; at each step, copy 8 bytes from some hl to VRAM
  ; TileData is aligned to 8 bytes so e is 0x00

  call getCurrentMapTileset ; source
  ld de, $9000 ; destination

  ; we copy all of the even tiles
.loop
  ld bc, OverworldTiles;
  ld a, [hl]

  ; copy one tile
  cp a, END_OF_TILESET
  jr z, .done ; we hit end of tileset

  ; copy 1 tile
  ld c, a ; now c is the low portion of the address of a tile

  ; otherwise copy 8 bytes
  REPT 8
    ; Mem copy 1 tile, 8 bytes
    ld a, [bc]
    ld [de], a
    inc de

    ; second byte always $00
    ld a, $00
    ld [de], a
    inc de
    inc bc
  ENDR

  ; skip a tile in the source
  inc hl
  inc hl

  ; skip a tile in the dest (add 16)
  ld a, $0F
  or e
  ld e, a
  inc de

  jr .loop
.done

  ret

; @param hl - address of the tile set to load
; each tile set is 16 tiles long, each entry 1 byte
; each tile set must end with 0xFF to indicate end of tileset
; loads only the odd tiles, so that odd and even tiles
; use different halves of the palette
LoadMapTilesetOddTiles:
  ; iterate along bc until we hit END_OF_TILESET
  ; at each step, copy 8 bytes from some hl to VRAM
  ; TileData is aligned to 8 bytes so e is 0x00

  call getCurrentMapTileset ; source
  inc hl ; start from odd tile
  ld de, $9000 ; destination

  ; skip a tile in the dest (add 16)
  ld a, $0F
  or e
  ld e, a
  inc de

  ; we copy all of the even tiles
.loop
  ld bc, OverworldTiles;
  ld a, [hl]

  ; copy one tile
  cp a, END_OF_TILESET
  jr z, .done ; we hit end of tileset

  ; copy 1 tile
  ld c, a ; now c is the low portion of the address of a tile

  ; otherwise copy 8 bytes
  REPT 8
    ; Mem copy 1 tile, 8 bytes
    ld a, [bc]
    ld [de], a
    inc de

    ; second byte always $FF
    ld a, $FF
    ld [de], a
    inc de
    inc bc
  ENDR

  ; skip a tile in the source
  inc hl
  inc hl

  ; skip a tile in the dest (add 16)
  ld a, $0F
  or e
  ld e, a
  inc de

  jr .loop
.done

  ret

/** load the tile palette for the map
 * copy the map data to VRAM */
; @pre - LCD is OFF
fullMapRedraw:
  call assertLCDOff

  call LoadMapPalette
  call LoadMapTilesetOddTiles
  call LoadMapTilesetEvenTiles
  call LoadMapTileMap
  call LoadMapTileMapAttributes

  ret

; @return hl - address of current map
getCurrentMap:
  ld hl, CURRENT_MAP_HIGH_BYTE
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

; @return hl - address of tile map for current map
getCurrentMapTileMap:
  ld hl, CURRENT_MAP_TILEMAP_HIGH_BYTE
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

; @return hl - address of tileset for current map
getCurrentMapTileset:
  ld hl, CURRENT_MAP_TILESET_HIGH_BYTE
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

; @return hl - address of tileset metadata
getCurrentMapTilesetMetadata:
  ld hl, CURRENT_MAP_TILESET_METADATA_HIGH_BYTE
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

; @return hl - address of palette for current map
getCurrentMapPalette:
  ld hl, CURRENT_MAP_PALETTE_HIGH_BYTE
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

; Convert a world position to a tilemap address
; hl = currentMapAddress + X + Y * 20
; @param b: Y
; @param c: X
; @return hl: tile address
getCurrentMapTileAddressByWorldPosition:
  call getTileAddressByWorldPosition
  call getCurrentMapTileMap
  add hl, de
  ret

; @param hl - address of current map
; @return void CURRENT_MAP pointer is set to hl
setCurrentMap:
  ; set the current map
  ld de, CURRENT_MAP_HIGH_BYTE
  ld a, h
  ld [de], a

  ld de, CURRENT_MAP_LOW_BYTE
  ld a, l
  ld [de], a

  ; set the current tileset using the map
  push hl
  call getTilesetFromMapMetadata
  call setCurrentMapTileset
  pop hl

  push hl
  call getTilemapFromMap
  call setCurrentMapTilemap
  pop hl

  push hl
  call getTilesetMetadataFromMapMetadata
  call setCurrentMapTilesetMetadata
  pop hl

  push hl
  call getPaletteFromMapMetadata
  call setCurrentMapPalette
  pop hl

  ret

; @param hl - address of map
; @return hl - address of map tileset
getTilemapFromMap:
  ; advance to the tileset pointer
  ; @DEPENDS on MAP_METADATA_SIZE
  ; no need to advance, it's the first field

  ; need to advance past metadata
  inc hl
  inc hl
  inc hl
  inc hl
  inc hl
  inc hl

  ; it's not a pointer so no dereference

  ret

; @param hl - address of map
; @return hl - address of map tileset
getTilesetFromMapMetadata:
  ; advance to the tileset pointer
  ; @DEPENDS on MAP_METADATA_SIZE
  ; no need to advance, it's the first field

  ; dereference the pointer
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

; @param hl - address of map
; @return hl - address of map tileset
getTilesetMetadataFromMapMetadata:
  ; advance to the tileset pointer
  ; @DEPENDS on MAP_METADATA_SIZE
  ; no need to advance, it's the first field

  ; need to advance passed the tileset
  inc hl
  inc hl

  ; dereference the pointer
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

; @param hl - address of map
; @return hl - address of map palette
getPaletteFromMapMetadata:
  ; advance to the palette pointer
  ; @DEPENDS on MAP_METADATA_SIZE

  ; need to advance passed the tileset
  ; need to advance passed the tileset metadata
  inc hl
  inc hl
  inc hl
  inc hl

  ; dereference the pointer
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

; @param hl - tileset 
setCurrentMapTilesetMetadata:
  ld de, CURRENT_MAP_TILESET_METADATA_HIGH_BYTE
  ld a, h
  ld [de], a

  ld de, CURRENT_MAP_TILESET_METADATA_LOW_BYTE
  ld a, l
  ld [de], a

  ret

; @param hl - tileset 
setCurrentMapTilemap:
  ld de, CURRENT_MAP_TILEMAP_HIGH_BYTE
  ld a, h
  ld [de], a

  ld de, CURRENT_MAP_TILEMAP_LOW_BYTE
  ld a, l
  ld [de], a

  ret

; @param hl - tileset 
setCurrentMapTileset:
  ld de, CURRENT_MAP_TILESET_HIGH_BYTE
  ld a, h
  ld [de], a

  ld de, CURRENT_MAP_TILESET_LOW_BYTE
  ld a, l
  ld [de], a

  ret

; @param hl - palette 
setCurrentMapPalette:
  ld de, CURRENT_MAP_PALETTE_HIGH_BYTE
  ld a, h
  ld [de], a

  ld de, CURRENT_MAP_PALETTE_LOW_BYTE
  ld a, l
  ld [de], a

  ret

; sets the current map to Start
initCurrentMap:
  ld hl, Start
  call setCurrentMap

  ret

ENDC

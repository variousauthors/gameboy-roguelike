IF !DEF(MAP_INC)
DEF MAP_INC EQU 1

SECTION "MapVariables", WRAM0

CURRENT_MAP_HIGH_BYTE: ds 1
CURRENT_MAP_LOW_BYTE: ds 1

SECTION "Map", ROM0

/** load the tile palette for the map
 * copy the map data to VRAM */
; @pre - we are in VBLANK
; @pre - LCD is OFF
fullMapRedraw:
  call assertLCDOff

  call getCurrentMap
  call loadTilePalette
  call copyMapData

  ret

; @param de - pointer to map (high byte)
setCurrentMap:
  ; set the current map
  ld hl, CURRENT_MAP_HIGH_BYTE
  ld a, [de]
  ld [hli], a

  inc de
  ld a, [de]
  ld [hl], a

  dec de ; reset the pointer

  ret


; @return hl - address of current map
getCurrentMap:
  ld hl, CURRENT_MAP_HIGH_BYTE
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

initCurrentMap:
  ld de, Start
  call setCurrentMap

  ret

ENDC

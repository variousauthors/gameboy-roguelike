IF !DEF(NAVIGATION_MAP_INC)
DEF NAVIGATION_MAP_INC EQU 1

SECTION "NavigationMapState", WRAM0

DEF NAVIGATION_MAP_WIDTH EQU 20
DEF NAVIGATION_MAP_HEIGHT EQU 18
DEF FLOOD_MAX EQU $6

NavigationMap: ds NAVIGATION_MAP_HEIGHT * NAVIGATION_MAP_WIDTH
NavigationMapEnd:

SECTION "NavigationMap", ROM0

clearNavigationMap:
  ld hl, NavigationMap

  REPT NAVIGATION_MAP_HEIGHT
    ld b, NAVIGATION_MAP_WIDTH 
    call clearMemory
  ENDR

  ret

/* smell is complicated maybe we should use
 * sound after all
 * when the player finishes a turn we send an
 * echo, with a strength based on the action
 * and it flood fills out and monsters use it
 * to plan their next action */

/* for now we will flood fill the entire map
 * and then only use values > some threshold
 * for navigation */
floodFillNavigationMap:
  call clearNavigationMap
  ; convert player world position to address
  call getNavigationMapAddressByWorldPosition

  ; run flood fill recursively
  ; terminating whenever a address is outside the map
  
  ld a, FLOOD_MAX
  call recursivelyFloodFillNavigationMap

  ret

/*
 * hl and de are preserved on the stack
 * @param hl - address in navigation map
 * @return z - corresponding map tile is collides */
framedMapCollisionCheck:
  push hl
  push de

  ; convert hl navigation map tile address
  ; into map data tile address by subtracting off NavigationMap

  ld de, NavigationMap
	; 6 bytes, 6 cycles calculates hl = hl - de
	ld a, l
	sub e
	ld l, a
	ld a, h
	sbc d
	ld h, a

  ; swap em
  ld d, h
  ld e, l
  ; now de has offset
  call getCurrentMapTileMap
  add hl, de
  ; now hl has tile in tilemap

  ld a, [hl]
  call isCollision

  pop de
  pop hl

  ret

/** 
 * @param a - current value to write
 * @param hl - address of map cell to try to flood 
 */
recursivelyFloodFillNavigationMap:
  ld b, a ; hide the value

  ; if the center cell already has a value, abort
  ld a, [hl]
  cp a, 0
  ret nz ; cell has been touched

  ; if a is already 0, abort
  ld a, b
  cp a, 0
  ret z ; we're out of steam

  ; if hl collides with geometry, abort
  call framedMapCollisionCheck
  jr nz, .noCollision

  ; we mark this collision so that we will skip future
  ; checks against this tile
.markCollision
  ld [hl], $FF
  ret
.noCollision

  ; if this address is out of bounds, abort
  ld de, NavigationMapEnd
  call addressIsGreaterEqual
  ret nc ; out of bounds

  ld de, NavigationMap
  call addressIsGreaterEqual
  ret c ; out of bounds

  ; otherwise, fill the current cell and recurse
  ld [hl], b
  ld a, b
  dec a

  push af
  push hl
  inc hl
  call recursivelyFloodFillNavigationMap
  pop hl
  pop af

  push af
  push hl
  dec hl
  call recursivelyFloodFillNavigationMap
  pop hl
  pop af

  push af
  push hl
  ; add 20 to hl
  ld de, 20
  add hl, de
  call recursivelyFloodFillNavigationMap
  pop hl
  pop af

  ret

/** 
 * @param hl - address 
 * @param de - upper bound 
 * @return nc - hl greater equal de */
addressIsGreaterEqual:
  ld a, h
  cp d
  ret c

  ld a, l
  cp e
  ret c

  ret

; Convert a world position to a tilemap address
; hl = navigationMap + X + Y * 20
; @param b: Y
; @param c: X
; @return hl: map address
getNavigationMapAddressByWorldPosition:
  call getPlayerWorldPosition
  call getTileAddressByWorldPosition
  ld hl, NavigationMap
  add hl, de
  ret

/* we can additionally have a "pheremone trail"
 * that is a set of arrows the player leaves behind
 * so that a randomly wandering monster can pick up their sent
 * if they passed recently */

ENDC
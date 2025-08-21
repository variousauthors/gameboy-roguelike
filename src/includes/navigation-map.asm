IF !DEF(NAVIGATION_MAP_INC)
DEF NAVIGATION_MAP_INC EQU 1

SECTION "NavigationMapState", WRAM0, ALIGN[8]

DEF NAVIGATION_MAP_WIDTH EQU 20
DEF NAVIGATION_MAP_HEIGHT EQU 18
DEF FLOOD_MAX EQU $5
DEF VISITED_MASK EQU %10000000
DEF VALUE_MASK EQU %00001111

NavigationMap: ds NAVIGATION_MAP_HEIGHT * NAVIGATION_MAP_WIDTH
NavigationMapEnd:

; the size of the navigation map will be 4 * (triangle of size n - 1) + 1
; so the queue only needs to hold the frontier which is (n - 1) * 4
; I'm keeping this constant just because I like it, we don't actually need it
DEF NAVIGATION_DIAMOND_SIZE EQU (4 * ((FLOOD_MAX * (FLOOD_MAX - 1)) / 2)) + 1

NavigationMapQueue: ds (FLOOD_MAX - 1) * 4
NavigationMapQueueTop: ds 2 ; pointer to the top

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
  call initSimpleQueue
  call getPlayerWorldPosition
  call getNavigationMapAddressByWorldPosition

  ; enqueue the address
  ld b, FLOOD_MAX
  call tryFillCell
  call markVisitedCell
  call enqueueSimpleQueue

.loop
  ; pop an address from the queue
  ; if the queue is empty, end
  call dequeueSimpleQueue
  jr z, .done

  call getValueCell
  dec a
  jr z, .loop ; no need to continue from this cell

  ; then we fill, mark, and enqueue the neighbours

  push af
  push hl
  inc hl
  ld b, a
  call tryFillCell
  jr z, .skip1
  call markVisitedCell
  call enqueueSimpleQueue
.skip1
  pop hl
  pop af

  push af
  push hl
  dec hl
  ld b, a
  call tryFillCell
  jr z, .skip2
  call markVisitedCell
  call enqueueSimpleQueue
.skip2
  pop hl
  pop af

  push af
  push hl
  ld de, 20
  add hl, de
  ld b, a
  call tryFillCell
  jr z, .skip3
  call markVisitedCell
  call enqueueSimpleQueue
.skip3
  pop hl
  pop af

  push af
  push hl
  ld de, 20
  call subHLDE
  ld b, a
  call tryFillCell
  jr z, .skip4
  call markVisitedCell
  call enqueueSimpleQueue
.skip4
  pop hl
  pop af

  jr .loop

.done

  ret

; @param hl - address
; @return a - value
; @return z - no value
getValueCell:
  ld a, [hl]
  cp a, $FF ; collision, return zero
  jr z, .collision

  and a, VALUE_MASK
  ret

.collision
  ld a, 0
  cp a, 0

  ret

; @param hl - address to mark visited
markVisitedCell: 
  ld a, [hl]
  ld b, VISITED_MASK
  or a, b
  ld [hl], a

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
 * @param b - value to use
 * @param hl - address of cell to fill 
 * @return z - failed to fill */
tryFillCell:
  ; if the cell already has a value, abort
  ld a, [hl]
  cp a, 0
  jr nz, .abort ; cell has been touched

  ; if a is already 0, abort
  ld a, b
  cp a, 0
  jr z, .abort ; we're out of steam

  ; if hl collides with geometry, abort
  call framedMapCollisionCheck
  jr nz, .noCollision

  ; we mark this collision so that we will skip future
  ; checks against this tile
.markCollision
  ld [hl], $FF
  jr .abort

.noCollision

  ; if this address is out of bounds, abort
  ld de, NavigationMapEnd
  call isGreaterEqualAddress
  ret nc ; out of bounds

  ld de, NavigationMap
  call isGreaterEqualAddress
  ret c ; out of bounds

  ld a, b ; recover value
  ld [hl], a ; fill cell

  ret

.abort
  ld a, 0
  cp a, 0 ; return z

  ret

; Convert a world position to a tilemap address
; hl = navigationMap + X + Y * 20
; @param b: Y
; @param c: X
; @return hl: map address
getNavigationMapAddressByWorldPosition:
  call getTileAddressByWorldPosition
  ld hl, NavigationMap
  add hl, de
  ret

/* we can additionally have a "pheremone trail"
 * that is a set of arrows the player leaves behind
 * so that a randomly wandering monster can pick up their sent
 * if they passed recently */

ENDC
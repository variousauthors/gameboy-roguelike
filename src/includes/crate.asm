IF !DEF(CRATE_INC)
DEF CRATE_INC EQU 1

SECTION "CrateState", WRAM0

nextSpriteIndex: ds 1

; crate position active, y, x, tile, palette
; 4 bytes
DEF CRATE_SIZE EQU 5
DEF CRATE_COUNT EQU 10
CrateState:
  ds CRATE_SIZE * CRATE_COUNT

SECTION "Crate", ROM0

resetNextSpriteIndex: 
  ld a, 0
  ld [nextSpriteIndex], a

  ret

; @return de - holds the address of the next empty sprite
getNextSpriteAddress: 
  ld a, [nextSpriteIndex]
  inc a
  ld [nextSpriteIndex], a
  dec a

  ; a * 4
  sla a
  sla a

  ld de, _RAM
  call addAToDE

  ret

; @TODO this will be done per map obviously
initCrates:
  ld hl, CrateState
  ; three crates

  ld a, 1 ; active
  ld [hl+], a
  ld a, 9 ; y
  ld [hl+], a
  ld a, 2 ; x
  ld [hl+], a
  ld a, 0 ; tile
  ld [hl+], a
  ld a, 0 ; palette
  ld [hl+], a

  ld a, 1 ; active
  ld [hl+], a
  ld a, 9 ; y
  ld [hl+], a
  ld a, 3 ; x
  ld [hl+], a
  ld a, 0 ; tile
  ld [hl+], a
  ld a, 0 ; palette
  ld [hl+], a

  ld a, 1 ; active
  ld [hl+], a
  ld a, 9 ; y
  ld [hl+], a
  ld a, 4 ; x
  ld [hl+], a
  ld a, 0 ; tile
  ld [hl+], a
  ld a, 0 ; palette
  ld [hl+], a

  ret

; @param hl - crate to draw
; @post hl is pointing to the next crate
drawCrate:
  ld a, [hli] ; active
  cp a, 0
  jp z, .skip

  ; it is active so draw
  call getNextSpriteAddress
  call memCopyC

  ret

.skip
  inc hl
  inc hl
  inc hl
  inc hl

  ret

drawCrates: 
  ld hl, CrateState

  ld b, CRATE_COUNT
.loop
  call drawCrate
  dec b
  jr nz, .loop

  ret

applyMoveIntentCrate:
  ret

isBlockedCrate:
  ret

recordMoveIntentCrate:
  ret

; @param bc - player world next y, x
; @return z - there is a collision
checkCollisionCrate:

  ret

ENDC
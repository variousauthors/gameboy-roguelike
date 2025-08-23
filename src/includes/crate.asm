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

; @return hl - holds the address of the next empty sprite
getNextSpriteAddressHL: 
  ld a, [nextSpriteIndex]
  inc a
  ld [nextSpriteIndex], a
  dec a

  ; a * 4
  sla a
  sla a

  ld hl, _RAM
  call addAToHL

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
  call drawSprite

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

checkCollisionCrate:
  push hl
  ld a, [hli] ; active
  cp a, 0
  jr z, .inactive

  ; check y, x
  ld a, [hli]
  cp a, b
  jp nz, .noCollision

  ld a, [hli]
  cp a, c
  jp nz, .noCollision

  ; collision!
  pop hl

  ret

.noCollision
  pop hl
  ret

.inactive
  ; skip forward and return nz
  pop hl

  ld a, 1 ; set nz
  or a

  ret

; @param bc - entity world next y, x
; @return z - there is a collision
; @return hl - address of colliding crate
checkCollisionCrates:
  ; iterate over crates and find a colliding crate
  ld hl, CrateState
  ld d, CRATE_COUNT

.loop
  call checkCollisionCrate
  jp z, .collision ; we have a collision

  ; otherwise advance past this crate
  ld a, CRATE_SIZE
  call addAToHL

  dec d
  jr nz, .loop

.noCollision
  ld a, 1
  or a
  ret

.collision

  ret

ENDC
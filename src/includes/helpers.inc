IF !DEF(HELPERS_INC)
DEF HELPERS_INC EQU 1

SECTION "Helpers", ROM0

; @param a: tile ID
; @return z: set if a is a wall.
IsWallTile:
  cp a, $01
  ret z
  ret


; @param de - source
; @param hl - destination
; @param bc - length
Memcopy:
  ld a, [de]
  ld [hli], a
  inc de
  dec bc
  ld a, b
  or a, c
  jp nz, Memcopy
  ret

; @param de - source
; @param hl - destination
; @param bc - length
Memcopy1bpp:
  ld a, [de]
  ld [hli], a

  ; second byte same as the first
  ld a, [de]
  ld [hli], a
  inc de

  dec bc
  ld a, b
  or a, c
  jp nz, Memcopy1bpp
  ret

/** crashes the program is LCD is on */
assertLCDOff:
  ld a, [rLCDC]
  cp a, 0
  call nz, crash

  ret

turnOffLCD:
.waitVBlank
  ld a, [rLY]
  cp 144
  jp c, .waitVBlank

  ; Turn the LCD off
  ld a, 0
  ld [rLCDC], a
  ret

turnOnLCD:
	; Turn the LCD on
	ld a, LCDCF_ON | LCDCF_BGON | LCDCF_OBJON
	ld [rLCDC], a

	; During the first (blank) frame, initialize display registers
	ld a, %11100100
	ld [rBGP], a
  ld a, %11100100
  ld [rOBP0], a
  ret

; Convert a pixel position to a tilemap address
; hl = $9800 + X + Y * 32
; @param b: Y
; @param c: X
; @return hl: tile address
getTileAddressByPixel:
  ; First, we need to divide by 8 to convert a pixel position to a tile position.
  ; After this we want to multiply the Y position by 32.
  ; These operations effectively cancel out so we only need to mask the Y value.
  ld a, b
  and a, %11111000
  ld l, a
  ld h, 0
  ; Now we have the position * 8 in hl
  add hl, hl ; position * 16
  add hl, hl ; position * 32
  ; Convert the X position to an offset.
  ld a, c
  srl a ; a / 2
  srl a ; a / 4
  srl a ; a / 8
  ; Add the two offsets together.
  add a, l
  ld l, a
  adc a, h
  sub a, l
  ld h, a
  ; Add the offset to the tilemap's base address, and we are done!
  ld de, $9800
  add hl, de
  ret

; Convert a world position to a tilemap offset
; the caller must then add hl, de
; with hl having some base address
; de = X + Y * 20
; @param b: Y
; @param c: X
; @return de: offset of the tile
getTileAddressByWorldPosition:
  ; we want to multiply the Y position by 20.
  ; 20 = (4 + 16)
  ; so first times 4 then times 16
  ; then add
  ; world position y is in 0 - 18 so 
  ; we can safely shift up to 8x
  ld d, b
  sla d ; times 2
  sla d ; times 4

  ld l, b
  ld h, 0
  ; Now times 16
  add hl, hl ; position * 2
  add hl, hl ; position * 4
  add hl, hl ; position * 8
  add hl, hl ; position * 16

  ; then sum
  ld a, d
  call addAToHL

  ; the X position is just an offset.
  ld a, c
  call addAToHL

  ; we need the offset in de
  ; so we can add the tilemap base address
  ld e, l
  ld d, h

  ret

/** hl = hl - de */
subHLDE:
  push af
  ld a, l
	sub e
	ld l, a
	ld a, h
	sbc d
	ld h, a
  pop af

  ret

; @param b, c - y, x world position to check
; @return z if collision
checkCollisionMap:
  ; convert the y, x into tile address
  ; look it up in the tileset
  ; check the collision mask in the tileset
  call getCurrentMapTileAddressByWorldPosition

  ld a, [hl]
  call isCollision

  ret

; a - tilemap index
; @return z if collision
isCollision:
  push af
  call getCurrentMapTilesetMetadata
  pop af
  call addAToHL
  ld a, [hl]

  and a, PASSABLE_TILE
  ; z if not passable

  ret

/** halts the program forever if the  */
crash:
.loop
  jr .loop

  ret

; @param a - a
; @param hl - hl
; @return hl - hl + a
addAToHL:
  add l ; a = a + l
	ld l, a ; l' = a'
	adc h ; a'' = a' + h + c ; what!?
	sub l ; l' here is a + l
	ld h, a ; so h is getting h + c yikes!

  ret

; @param hl - pointer high byte
; @return hl - address
dereferencePointer:
  ld a, [hl+]
  ld l, [hl]
  ld h, a

  ret

; @param hl - address
; @param de - pointer
updatePointer:
  ld a, h
  ld [de], a
  inc de
  ld a, l
  ld [de], a

  ret

; @param - hl the address of some subroutie to call
indirectCall:
  jp hl

/* @param hl - memory to clear 
 * @param b - amount to clear
 **/
clearMemory:
  ld a, 0
.loop
  ld [hli], a
  dec b
  jp nz, .loop

  ret

ENDC
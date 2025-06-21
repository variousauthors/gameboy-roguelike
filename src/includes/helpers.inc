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
; @param b: X
; @param c: Y
; @return hl: tile address
getTileAddressByPixel:
  ; First, we need to divide by 8 to convert a pixel position to a tile position.
  ; After this we want to multiply the Y position by 32.
  ; These operations effectively cancel out so we only need to mask the Y value.
  ld a, c
  and a, %11111000
  ld l, a
  ld h, 0
  ; Now we have the position * 8 in hl
  add hl, hl ; position * 16
  add hl, hl ; position * 32
  ; Convert the X position to an offset.
  ld a, b
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
  ld bc, $9800
  add hl, bc
  ret

; hl - tile address
; @return z if collision
isCollision:
  ld a, [hl]
  cp a, WALL_TILE
  ret

/** halts the program forever if the  */
crash:
.loop
  jr .loop

  ret

ENDC
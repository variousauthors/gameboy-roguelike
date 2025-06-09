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

ENDC
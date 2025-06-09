IF !DEF(UTILITIES_INC)
DEF UTILITIES_INC EQU 1

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

; a - tile
; @return z if collision
isCollision:
  cp a, WALL_TILE
  ret


ENDC
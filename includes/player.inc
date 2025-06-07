IF !DEF(PLAYER_INC)
DEF PLAYER_INC EQU 1

playerMove:
.checkLeft
  ld a, [wNewKeys]
  and a, PADF_LEFT
  jp z, .checkRight
.left
  ld a, [_OAMRAM + 1]
  ld b, 8
  sub b

  ; if we moved out of bounds, reset the position
  cp a, 8
  ret z
  ld [_OAMRAM + 1], a
  ret

.checkRight
  ld a, [wNewKeys]
  and a, PADF_RIGHT
  jp z, .checkUp

.right
  ld a, [_OAMRAM + 1]
  ld b, 8
  add b

  ; if we moved out of bounds, reset the position
  cp a, 160
  ret z
  ld [_OAMRAM + 1], a
  ret

.checkUp
  ld a, [wNewKeys]
  and a, PADF_UP
  jp z, .checkDown

.up
  ld a, [_OAMRAM]
  ld b, 8
  sub b

  ; if we moved out of bounds, reset the position
  cp a, 16
  ret z
  ld [_OAMRAM], a
  ret

.checkDown
  ld a, [wNewKeys]
  and a, PADF_DOWN
  jp z, .done

.down
  ld a, [_OAMRAM]
  ld b, 8
  add b

  ; if we moved out of bounds, reset the position
  cp a, 152
  ret z
  ld [_OAMRAM], a
  ret

.done
  ret

ENDC
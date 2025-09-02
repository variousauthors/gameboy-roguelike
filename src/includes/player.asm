IF !DEF(PLAYER_INC)
DEF PLAYER_INC EQU 1

updatePlayerSprite:
  ; set the sprite
  call getPlayerPosition
  call getAddressFromPosition
  ld a, [hl]

  cp a, BLACK_STONE
  jr z, .blackStone
  cp a, WHITE_STONE
  jr z, .whiteStone
  jr .noStone

.blackStone
  ; set sprite to cross-hair
  ld a, 1
  ld [_OAMRAM + 2], a

  ; set palette to white
  ld a, [_OAMRAM + 3]
  set 4, a
  ld [_OAMRAM + 3], a
  ret

.whiteStone
  ; set sprite to cross-hair
  ld a, 1
  ld [_OAMRAM + 2], a

  ; set palette to black
  ld a, [_OAMRAM + 3]
  res 4, a
  ld [_OAMRAM + 3], a
  ret

.noStone
  ; set sprite to stone
  ld a, 0
  ld [_OAMRAM + 2], a

  ld a, [currentTurn]
  cp a, BLACK_TURN
  jr z, .blackPalette

  ld a, [_OAMRAM + 3]
  set 4, a
  ld [_OAMRAM + 3], a
  ret

.blackPalette
  ld a, [_OAMRAM + 3]
  res 4, a
  ld [_OAMRAM + 3], a

  ret
  ; palette swap based on current turn


; @return bc - y, x
getPlayerPosition:
  ld a, [_OAMRAM]
  srl a
  srl a
  srl a
  dec a
  dec a
  ld b, a

  ld a, [_OAMRAM + 1]
  srl a
  srl a
  srl a
  dec a
  ld c, a

  ret

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
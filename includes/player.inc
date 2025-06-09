IF !DEF(PLAYER_INC)
DEF PLAYER_INC EQU 1

DEF PLAYER_MOVE_LEFT EQU PADF_LEFT
DEF PLAYER_MOVE_RIGHT EQU PADF_RIGHT
DEF PLAYER_MOVE_UP EQU PADF_UP
DEF PLAYER_MOVE_DOWN EQU PADF_DOWN

SECTION "PlayerState", WRAM0

wPlayerDirection: db
wPlayerX: db
wPlayerY: db
wPlayerNextX: db
wPlayerNextY: db

SECTION "PlayerLogic", ROM0

playerMove:
.checkLeft
  ld a, [wNewKeys]
  and a, PADF_LEFT
  jp z, .checkRight
.left
  ld a, [wPlayerX]
  ld b, 8
  sub b

  ; if we moved out of bounds, reset the position
  cp a, 0
  ret z
  ld [wPlayerNextX], a
  ret

.checkRight
  ld a, [wNewKeys]
  and a, PADF_RIGHT
  jp z, .checkUp

.right
  ld a, [wPlayerX]
  ld b, 8
  add b

  ; if we moved out of bounds, reset the position
  cp a, 152
  ret z
  ld [wPlayerNextX], a
  ret

.checkUp
  ld a, [wNewKeys]
  and a, PADF_UP
  jp z, .checkDown

.up
  ld a, [wPlayerY]
  ld b, 8
  sub b

  ; if we moved out of bounds, reset the position
  cp a, 0
  ret z
  ld [wPlayerNextY], a
  ret

.checkDown
  ld a, [wNewKeys]
  and a, PADF_DOWN
  jp z, .done

.down
  ld a, [wPlayerY]
  ld b, 8
  add b

  ; if we moved out of bounds, reset the position
  cp a, 136
  ret z
  ld [wPlayerNextY], a
  ret

.done
  ret

; @return z - is collision
playerBump:
  ; convert position to address
  ; then check for wall tile
  ld a, [wPlayerNextX]
  ld b, a
  ld a, [wPlayerNextY]
  ld c, a
  call getTileAddressByPixel
  ld a, [hl]
  call isCollision

  ret

playerUpdateSprite:
  ld hl, _OAMRAM
  ld a, [wPlayerY]
  ld b, 16
  add a, b
  ld [hli], a
  ld a, [wPlayerX]
  ld b, 8
  add a, b
  ld [hl], a
  ret

ENDC
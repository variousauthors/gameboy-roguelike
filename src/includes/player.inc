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
wPlayerWounds: db
wPlayerHP: db
wPlayerDead: db

SECTION "PlayerLogic", ROM0

playerRollbackMove:
  ld a, [wPlayerX]
  ld [wPlayerNextX], a
  ld a, [wPlayerY]
  ld [wPlayerNextY], a
  ret

; @return z - monster dead
playerCommitAttack:
  call playerRollbackMove

  ld a, [wMonsterWounds]
  inc a
  ld [wMonsterWounds], a
  ld b, a
  ld a, [wMonsterHP]
  cp a, b

  ret nz

  ; monster dead
  ld a, 1
  ld [wMonsterDead], a
  cp a, 1 ; ret z is monster dead

  ret

; @return z - no move
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
  call isCollision

  ret

; @return z - is attacking
playerAttack:
  ld a, [wPlayerNextX]
  ld b, a
  ld a, [wMonsterX]
  cp a, b
  ret nz ; no monster

  ld a, [wPlayerNextY]
  ld b, a
  ld a, [wMonsterY]
  cp a, b
  ret nz ; no monster

  ; TODO later just remove the monster
  ; from the list of monsters when it does
  ; and then this won't be necessary trigger
  ; we found a monster, but is it dead
  ld a, [wMonsterDead]
  cp a, 0

  ret

initPlayer:
  ld a, 16
  ld [wPlayerX], a
  ld [wPlayerNextX], a
  ld a, 128
  ld [wPlayerY], a
  ld [wPlayerNextY], a
  ld a, 4
  ld [wPlayerHP], a
  ld a, 0
  ld [wPlayerWounds], a
  ld [wPlayerDead], a
  ret

playerUpdateSprite:
  ; init player sprite
  ld hl, _OAMRAM
  ld a, [wPlayerY]
  ld b, 16
  add a, b
  ld [hli], a
  ld a, [wPlayerX]
  ld b, 8
  add a, b
  ld [hli], a

  ld a, 0 ; sprite
  ld b, a
  ld a, [wPlayerWounds]
  add a, b
  ld [hli], a
  ld a, 0 ; attributes
  ld [hl], a

  ld a, [wPlayerDead]
  cp a, 0
  ret z

  ; disable the sprite
  ld a, 0
  ld [_OAMRAM], a
  ld [_OAMRAM + 1], a

  ret

ENDC
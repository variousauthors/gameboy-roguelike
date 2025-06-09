IF !DEF(MONSTERS_INC)
DEF MONSTERS_INC EQU 1

SECTION "MonstersState", WRAM0

wMonsterDirection: db
wMonsterX: db
wMonsterY: db
wMonsterNextX: db
wMonsterNextY: db
wMonsterHP: db
wMonsterWounds: db
wMonsterDead: db

SECTION "MonstersLogic", ROM0

; @return z - is collision
monsterBump:
  ; convert position to address
  ; then check for wall tile
  ld a, [wMonsterNextX]
  ld b, a
  ld a, [wMonsterNextY]
  ld c, a
  call getTileAddressByPixel
  call isCollision

  ret

monsterRollbackMove: 
  ld a, [wMonsterX]
  ld [wMonsterNextX], a
  ld a, [wMonsterY]
  ld [wMonsterNextY], a

  ret

monsterCommitAttack:
  call monsterRollbackMove

  ld a, [wPlayerWounds]
  inc a
  ld [wPlayerWounds], a
  ld b, a
  ld a, [wPlayerHP]
  cp a, b

  ret nz

  ; player dead
  ld a, 1
  ld [wPlayerDead], a

  ret

; record intent to move toward the player
; avoid abstacles
monsterMove:
  ; choose shortest direction
  ld a, [wPlayerX]
  ld b, a
  ld a, [wMonsterX]
  sub a, b
  jr nc, .doneX
  cpl a
  inc a ; neg a
.doneX

  ld c, a ; stash it away

  ; choose shortest direction
  ld a, [wPlayerY]
  ld b, a
  ld a, [wMonsterY]
  sub a, b
  jr nc, .doneY
  cpl a
  inc a ; neg a
.doneY

  cp a, c ; which is bigger?

  ld d, 2

  jr c, .monsterMoveX
  jr .monsterMoveY

.monsterMoveY
  ld a, [wMonsterY]
  ld c, a
  ld a, [wPlayerY]

  cp a, c
  jr c, .up

.down
  ld a, c
  ld c, 8
  add a, c
  ld [wMonsterNextY], a
  jr .validateMoveY

.up
  ld a, c
  ld c, 8
  sub a, c
  ld [wMonsterNextY], a
  jr .validateMoveY

.validateMoveY
  ; check if this is a valid move
  ld c, a
  ld a, [wMonsterX]
  ld b, a
  call getTileAddressByPixel
  call isCollision
  jr z, .downFailY
  ret ; yes, valid move

.downFailY
  ; rollback
  ld a, [wMonsterY]
  ld [wMonsterNextY], a
  dec d ; tries
  ret z ; if we are stuck, return
  jr .monsterMoveX ; otherwise try x

.monsterMoveX
  ld a, [wMonsterX]
  ld b, a
  ld a, [wPlayerX]

  cp a, b
  jr c, .left

.right
  ld a, b
  ld b, 8
  add a, b
  ld [wMonsterNextX], a
  jr .validateMoveX

.left
  ld a, b
  ld b, 8
  sub a, b
  ld [wMonsterNextX], a
  jr .validateMoveX

.validateMoveX
  ; check if this is a valid move
  ld b, a
  ld a, [wMonsterY]
  ld c, a
  call getTileAddressByPixel
  call isCollision
  jr z, .downFailX
  ret ; yes, valid move

.downFailX
  ; rollback
  ld a, [wMonsterX]
  ld [wMonsterNextX], a
  dec d ; tries
  ret z ; if we are stuck, return
  jr .monsterMoveY ; otherwise try x
  
initMonster:
  ld a, 3 * 8
  ld [wMonsterX], a
  ld [wMonsterNextX], a
  ld a, 3 * 8
  ld [wMonsterY], a
  ld [wMonsterNextY], a
  ld a, 4
  ld [wMonsterHP], a

  ld a, 0
  ld [wMonsterWounds], a

  ld a, 0
  ld [wMonsterDead], a

  ret

monsterUpdateSprite:
  ; init Monster sprite
  ld hl, _OAMRAM + 4
  ld a, [wMonsterY]
  ld b, 16
  add a, b
  ld [hli], a
  ld a, [wMonsterX]
  ld b, 8
  add a, b
  ld [hli], a

  ld a, 4 ; sprite
  ld b, a
  ld a, [wMonsterWounds]
  add a, b
  ld [hli], a
  ld a, 0 ; attributes
  ld [hl], a

  ld a, [wMonsterDead]
  cp a, 0
  ret z

  ; disable the sprite
  ld a, 0
  ld [_OAMRAM + 4], a
  ld [_OAMRAM + 5], a

  ret

ENDC
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

  jr c, .monsterMoveX
  jr .monsterMoveY

.monsterMoveY
  ld a, [wPlayerY]
  ld b, a
  ld a, [wMonsterY]

  cp a, b
  jr c, .down

.up
  ld b, 8
  sub a, b
  ld [wMonsterNextY], a
  ret

.down
  ld b, 8
  add a, b
  ld [wMonsterNextY], a
  ret
.monsterMoveX
  ld a, [wPlayerX]
  ld b, a
  ld a, [wMonsterX]

  cp a, b
  jr c, .right

.left
  ld b, 8
  sub a, b
  ld [wMonsterNextX], a
  ret

.right
  ld b, 8
  add a, b
  ld [wMonsterNextX], a
  ret
  
initMonster:
  ld a, 16
  ld [wMonsterX], a
  ld [wMonsterNextX], a
  ld a, 64
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
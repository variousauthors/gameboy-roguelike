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
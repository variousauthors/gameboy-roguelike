IF !DEF(MONSTER_LIST_INC)
DEF MONSTER_LIST_INC EQU 1

SECTION "MonsterStateList", WRAM0, ALIGN[2]

MonsterList:
  wMonsterPositionYs: ds 4
  wMonsterPositionXs: ds 4

  wMonsterPositionNextYs: ds 4
  wMonsterPositionNextXs: ds 4

  wMonsterCount: ds 1

SECTION "MonsterList", ROM0

monsterListInit:
  ld a, 15 * 8
  ld [wMonsterPositionXs], a
  ld [wMonsterPositionNextXs], a
  ld a, 10 * 8
  ld [wMonsterPositionYs], a
  ld [wMonsterPositionNextYs], a

  ld a, 3 * 8
  ld [wMonsterPositionXs + 1], a
  ld [wMonsterPositionNextXs + 1], a
  ld a, 3 * 8
  ld [wMonsterPositionYs + 1], a
  ld [wMonsterPositionNextYs + 1], a

  ld a, 9 * 8
  ld [wMonsterPositionXs + 2], a
  ld [wMonsterPositionNextXs + 2], a
  ld a, 6 * 8
  ld [wMonsterPositionYs + 2], a
  ld [wMonsterPositionNextYs + 2], a

  ld a, 15 * 8
  ld [wMonsterPositionXs + 3], a
  ld [wMonsterPositionNextXs + 3], a
  ld a, 15 * 8
  ld [wMonsterPositionYs + 3], a
  ld [wMonsterPositionNextYs + 3], a

  ld a, 4
  ld [wMonsterCount], a

  ret

monsterListUpdateSprites:
  ld de, MonsterList
  ld hl, _OAMRAM + 4
  ld a, [wMonsterCount]
  ld c, a

.loopYs
  ld a, [de]
  add a, 16
  inc de
  ld [hl], a ; load y

  ; we know this will not overflow into h because OAMRAM is 8 bit aligned
  ; all the addresses are in FE00 - FEFF
  ld a, l
  or a, %00000011
  inc a
  ld l, a
  
  dec c
  jr z, .doneYs
  jr .loopYs
.doneYs

  ld de, MonsterList + 4
  ld hl, _OAMRAM + 4
  ld a, [wMonsterCount]
  ld c, a

.loopXs
  inc hl ; advance to X
  ld a, [de]
  add a, 8
  inc de
  ld [hl], a ; load x

  ; we know this will not overflow into h because OAMRAM is 8 bit aligned
  ; all the addresses are in FE00 - FEFF
  ld a, l
  or a, %00000011
  inc a
  ld l, a
  
  dec c
  jr z, .doneXs
  jr .loopXs
.doneXs

  ret

monsterListCommitMoves:
  ld de, wMonsterPositionNextXs
  ld hl, wMonsterPositionXs
  ld a, [wMonsterCount]
  ld c, a

.loopX
  ld a, [de]
  ld [hli], a
  inc de

  dec c
  jr z, .doneX
  jr .loopX
.doneX

  ld de, wMonsterPositionNextYs
  ld hl, wMonsterPositionYs
  ld a, [wMonsterCount]
  ld c, a

.loopY
  ld a, [de]
  ld [hli], a
  inc de

  dec c
  jr z, .doneY
  jr .loopY
.doneY

  ret

monsterListPlanMoves:
  call monsterListPlanMovesX
  call monsterListPlanMovesY

  ret

monsterListPlanMovesX:
  ld hl, wMonsterPositionXs
  ld de, wMonsterPositionNextXs
  ld a, [wMonsterCount]
  ld c, a
.loop
  ld a, [hl]
  ld b, a
  ld a, [wPlayerX]

  cp a, b
  jr z, .none
  jr c, .left

.right
  ld a, b
  ld b, 8
  add a, b
  jr .loopEnd

.left
  ld a, b
  ld b, 8
  sub a, b
  jr .loopEnd

.none
  ld a, [hl]

.loopEnd
  ld [de], a
  inc de
  inc hl
  dec c
  jr z, .done
  jr .loop
.done
  ret

monsterListPlanMovesY:
  ld hl, wMonsterPositionYs
  ld de, wMonsterPositionNextYs
  ld a, [wMonsterCount]
  ld c, a
.loop
  ld a, [hl]
  ld b, a
  ld a, [wPlayerY]

  cp a, b
  jr z, .none
  jr c, .up

.down
  ld a, b
  ld b, 8
  add a, b
  jr .loopEnd

.up
  ld a, b
  ld b, 8
  sub a, b
  jr .loopEnd

.none
  ld a, [hl]

.loopEnd
  ld [de], a
  inc de
  inc hl
  dec c
  jr z, .done
  jr .loop
.done

  ret

; at the start of the planning we assume
; @param hl - list of positions
; @param de - list of next positions
; @param c - count of monsters
monsterListPlanMoveAxis:
  ret

ENDC
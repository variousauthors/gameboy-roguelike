IF !DEF(MONSTER_INC)
DEF MONSTER_INC EQU 1

SECTION "MonsterState", WRAM0

MonsterPositions: ds 2
MonsterPositionsEnd:
MonsterNextPositions: ds 2
MonsterHPs: ds 2

SECTION "Monster", ROM0

initMonsters: 
  ld a, 9
  ld [MonsterPositions], a
  ld [MonsterNextPositions], a
  ld a, 4
  ld [MonsterPositions + 1], a
  ld [MonsterNextPositions + 1], a

  ld a, 2
  ld [MonsterHPs], a

  ret

drawMonsters:
  ; if monster is dead zero it out
  ld a, [MonsterHPs]
  cp 0
  jr nz, .draw

  ld hl, _RAM + 4
  ld [hli], a
  ld [hli], a
  ld [hli], a
  ld [hli], a

  ret

.draw
  ; draw monster sprite
  ld a, [MonsterPositions]
  inc a
  inc a ; add 16, sprites start off-screen
  sla a
  sla a
  sla a

  ld hl, _RAM + 4
  ld [hli], a

  ld a, [MonsterPositions + 1]
  inc a ; add 8 sprites start off-screen
  sla a
  sla a
  sla a

  ld hl, _RAM + 5
  ld [hli], a

  ld a, 0
  ld [hli], a
  ld [hli], a

  ret

recordMoveIntentMonster:
  ld a, [MonsterPositions]
  ld b, a
  ld a, [MonsterPositions + 1]
  ld c, a

  call getNavigationMapAddressByWorldPosition
  
  call getValueCell
  ld b, a

  ; now move to the first square with a higher value

.checkRight
  inc hl
  call getValueCell

  cp a, b
  jr c, .checkLeft
  jr z, .checkLeft

  ; update next position
  ld a, [MonsterNextPositions + 1]
  inc a
  ld [MonsterNextPositions + 1], a

  jr .done

.checkLeft
  dec hl ; undo
  dec hl ; check left
  call getValueCell

  cp a, b
  jr c, .checkDown
  jr z, .checkDown

  ; update next position
  ld a, [MonsterNextPositions + 1]
  dec a
  ld [MonsterNextPositions + 1], a

  jr .done

.checkDown
  inc hl ; undo
  push hl
  ld de, 20
  add hl, de ; check up
  call getValueCell
  pop hl

  cp a, b
  jr c, .checkUp
  jr z, .checkUp

  ; update next position
  ld a, [MonsterNextPositions]
  inc a
  ld [MonsterNextPositions], a

  jr .done

.checkUp
  ld de, 20
  call subHLDE
  call getValueCell

  cp a, b
  jr c, .done
  jr z, .done

  ; update next position
  ld a, [MonsterNextPositions]
  dec a
  ld [MonsterNextPositions], a

.done

  ret

getMonsterWorldNextPosition:
  ld a, [MonsterNextPositions]
  ld b, a
  ld a, [MonsterNextPositions + 1]
  ld c, a
  ret

; @return z if we aborted the turn
doTurnMonster:
  call recordMoveIntentMonster

  ; check for collision
  call getMonsterWorldNextPosition
  call checkCollisionMap
  jr z, .abortMonsterTurn

  ; check for player

  ; update player pos
  call applyMoveIntentMonster

.abortMonsterTurn
  call resetMoveIntentMonster

  ret

resetMoveIntentMonster:
  ld a, [MonsterPositions]
  ld [MonsterNextPositions], a

  ld a, [MonsterPositions + 1]
  ld [MonsterNextPositions + 1], a
  ret


applyMoveIntentMonster:
  ld a, [MonsterNextPositions]
  ld [MonsterPositions], a

  ld a, [MonsterNextPositions + 1]
  ld [MonsterPositions + 1], a
  ret

ENDC
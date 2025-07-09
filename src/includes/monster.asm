IF !DEF(MONSTER_INC)
DEF MONSTER_INC EQU 1

SECTION "MonsterState", WRAM0

SECTION "Monster", ROM0

; @return z if we aborted the turn
doTurnMonster:
  ; do monster ai
  ; flood the field with player
  ; distance numbers out to some distance
  ; update monster intents

  ret

ENDC
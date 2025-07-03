IF !DEF(MONSTER_INC)
DEF MONSTER_INC EQU 1

SECTION "MonsterState", WRAM0

SECTION "Monster", ROM0

; @return z if we aborted the turn
doTurnMonster:
  ; do monster ai
  ; update monster intents

  ret

ENDC
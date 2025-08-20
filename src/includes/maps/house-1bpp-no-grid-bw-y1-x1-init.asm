IF !DEF(HOUSE_1BPP_NO_GRID_BW_Y1_X1_INIT_INC)
DEF HOUSE_1BPP_NO_GRID_BW_Y1_X1_INIT_INC EQU 1

  ; this is the body of a subroutine, the name of
  ; which is defined in a separate file
  ld b, 7
  ld c, 12
  ld a, $04
  call initMapObject

  ret

ENDC
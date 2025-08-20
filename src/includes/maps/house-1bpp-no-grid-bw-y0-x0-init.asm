IF !DEF(HOUSE_1BPP_NO_GRID_BW_Y0_X0_INIT_INC)
DEF HOUSE_1BPP_NO_GRID_BW_Y0_X0_INIT_INC EQU 1

  ; this is the body of a subroutine, the name of
  ; which is defined in a separate file
  ld b, 3
  ld c, 10
  ld a, $0D
  call initMapObject

  ret

ENDC
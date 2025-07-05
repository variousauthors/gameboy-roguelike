IF !DEF(NAVIGATION_MAP_INC)
DEF NAVIGATION_MAP_INC EQU 1

SECTION "NavigationMapState", WRAM0

DEF NAVIGATION_MAP_WIDTH EQU 20
DEF NAVIGATION_MAP_HEIGHT EQU 18

NavigationMap: ds NAVIGATION_MAP_HEIGHT * NAVIGATION_MAP_WIDTH

SECTION "NavigationMap", ROM0

/* smell is complicated maybe we should use
 * sound after all
 * when the player finishes a turn we send an
 * echo, with a strength based on the action
 * and it flood fills out and monsters use it
 * to plan their next action */

/* for now we will flood fill the entire map
 * and then only use values > some threshold
 * for navigation */
floodFillNavigationMap:
  ; convert player world position to address
  call getNavigationMapAddressByWorldPosition
  ret

; Convert a world position to a tilemap address
; hl = navigationMap + X + Y * 20
; @param b: Y
; @param c: X
; @return hl: map address
getNavigationMapAddressByWorldPosition:
  call getTileAddressByWorldPosition
  call getCurrentMapTileMap
  add hl, de
  ret

/* we can additionally have a "pheremone trail"
 * that is a set of arrows the player leaves behind
 * so that a randomly wandering monster can pick up their sent
 * if they passed recently */

ENDC
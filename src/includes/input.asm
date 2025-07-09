IF !DEF(INPUT_INC)
DEF INPUT_INC EQU 1

SECTION "Input", ROM0
UpdateKeys:
  ; poll half the controller
  ld a, P1F_GET_BTN
  call .onenibble
  ld b, a

  ; poll the other half
  ld a, P1F_GET_DPAD
  call .onenibble
  swap a
  xor a, b
  ld b, a

  ld a, P1F_GET_NONE
  ldh [rP1], a

  ld a, [wCurKeys]
  xor a, b ; a gets keys that changed state
  and a, b ; a gets keys that changed to pressed
  ld [wNewKeys], a
  ld a, b
  ld [wCurKeys], a
  ret

.onenibble
  ldh [rP1], a
  call .knownret ; burn 10 cycles calling a known ret
  ldh a, [rP1] ; waiting for the key matrix to settle
  ldh a, [rP1]
  ldh a, [rP1] ; this read counts
  or a, $F0

.knownret
  ret

DEF BUTTON_RIGHT EQU $10
DEF BUTTON_LEFT EQU $20
DEF BUTTON_UP EQU $40
DEF BUTTON_DOWN EQU $80

DEF BUTTON_RIGHT_BIT EQU $4
DEF BUTTON_LEFT_BIT EQU $5
DEF BUTTON_UP_BIT EQU $6
DEF BUTTON_DOWN_BIT EQU $7

SECTION "Input Variables", WRAM0
wCurKeys: db
wNewKeys: db

ENDC
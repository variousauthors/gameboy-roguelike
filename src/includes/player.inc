IF !DEF(PLAYER_INC)
DEF PLAYER_INC EQU 1

DEF PLAYER_INITIAL_Y EQU 13
DEF PLAYER_INITIAL_X EQU 7

SECTION "PlayerState", WRAM0

playerWorldX: db
playerWorldY: db

playerWorldNextX: db
playerWorldNextY: db

SECTION "Player", ROM0

DEF BLOCKING_TILE EQU $00
DEF PASSABLE_TILE EQU $10

; @return z if collision
checkCollisionPlayer:
  ; convert the y, x into tile address
  ; look it up in the tileset
  ; check the collision mask in the tileset
  ld a, [playerWorldNextY]
  ld b, a
  ld a, [playerWorldNextX]
  ld c, a
  call getCurrentMapTileAddressByWorldPosition

  ld a, [hl]
  call isCollision

  ret

resetMoveIntentPlayer:
  ld a, [playerWorldX]
  ld [playerWorldNextX], a

  ld a, [playerWorldY]
  ld [playerWorldNextY], a

  ret

; @return z if we aborted the turn
doTurnPlayer:
  ; read input
  call UpdateKeys

  ld a, [wNewKeys]
  cp a, 0
  jr z, .noInput

  call recordMoveIntentPlayer

  ; check for collision
  call checkCollisionPlayer
  jr z, .abortPlayerTurn

  ; check for monster

  ; update player pos
  call applyMoveIntentPlayer

  ; detect whether the player actually _did_
  ; anything... and pass the turn

  call passTurnPlayer

.noInput
  ret

.abortPlayerTurn
  call resetMoveIntentPlayer

  ret

passTurnPlayer:
  ret

applyMoveIntentPlayer:
  ld a, [playerWorldNextX]
  ld [playerWorldX], a

  ld a, [playerWorldNextY]
  ld [playerWorldY], a

  ret

/** record intents */
recordMoveIntentPlayer:
  ld a, [wNewKeys]

.checkLeft
  bit BUTTON_LEFT_BIT, a
  jr z, .checkRight

  ld hl, playerWorldNextX
  ld b, [hl]
  dec b
  ld [hl], b

.checkRight
  bit BUTTON_RIGHT_BIT, a
  jr z, .checkUp

  ld hl, playerWorldNextX
  ld b, [hl]
  inc b
  ld [hl], b

.checkUp
  bit BUTTON_UP_BIT, a
  jr z, .checkDown

  ld hl, playerWorldNextY
  ld b, [hl]
  dec b
  ld [hl], b

.checkDown
  bit BUTTON_DOWN_BIT, a
  jr z, .doneCheck

  ld hl, playerWorldNextY
  ld b, [hl]
  inc b
  ld [hl], b

.doneCheck

  ret

drawPlayer:
  ; draw player sprite
  ld hl, _OAMRAM

  ld a, [playerWorldY]
  inc a
  inc a ; add 16, sprites start off-screen
  sla a
  sla a
  sla a
  ld [hli], a

  ld a, [playerWorldX]
  inc a ; add 8 sprites start off-screen
  sla a
  sla a
  sla a
  ld [hli], a

  ld a, 0
  ld [hli], a
  ld [hli], a

  ret

initPlayer:
	; Copy the player sprite
	ld de, PlayerSprite
	ld hl, $8000
	ld bc, PlayerSpriteEnd - PlayerSprite
  call Memcopy

  ; initialize player state
  ld a, PLAYER_INITIAL_Y
  ld [playerWorldY], a
  ld [playerWorldNextY], a

  ld a, PLAYER_INITIAL_X
  ld [playerWorldX], a
  ld [playerWorldNextX], a

  ret

ENDC
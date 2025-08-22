IF !DEF(PLAYER_INC)
DEF PLAYER_INC EQU 1

DEF PLAYER_INITIAL_Y EQU 12
DEF PLAYER_INITIAL_X EQU 6
DEF PLAYER_INITIAL_HP EQU 3

SECTION "PlayerState", WRAM0

playerWorldX: db
playerWorldY: db

playerWorldNextX: db
playerWorldNextY: db

playerHP: db

SECTION "Player", ROM0

DEF BLOCKING_TILE EQU $00
DEF PASSABLE_TILE EQU $10

getPlayerWorldPosition:
  ld a, [playerWorldY]
  ld b, a
  ld a, [playerWorldX]
  ld c, a

  ret

getPlayerWorldNextPosition:
  ld a, [playerWorldNextY]
  ld b, a
  ld a, [playerWorldNextX]
  ld c, a

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
  call getPlayerWorldNextPosition
  call checkCollisionMap
  jr z, .abortPlayerTurn

  ; check for monster
  call getPlayerWorldNextPosition
  call tryHitMonster
  jr z, .didHitMonster

  ; update player pos
  call applyMoveIntentPlayer

  ; update the monster navigation map
  call floodFillNavigationMap

  ld a, 0
  cp a, 1 ; return nz
  
  ret

.noInput
  ret

.didHitMonster
  call resetMoveIntentPlayer

  ld a, 0
  cp a, 1 ; return nz, we did not "abort"

  ret

.abortPlayerTurn
  call resetMoveIntentPlayer

  ret

; @param b,c - y,x world position
; @return z - hit monster
tryHitMonster:
  ld hl, MonsterPositions
  ld a, MonsterPositionsEnd - MonsterPositions
  ld d, a
  ld e, 0 ; index of monster

.loop
  ; compare y
  ld a, [hli]
  cp b
  jr nz, .next

  ; compare x
  ld a, [hli]
  cp c
  jr z, .found

.next
  inc e ; index of next monster
  dec d
  dec d ; increment past 2 positions
  jr z, .notFound
  jr .loop

.found
  ; hit that monster
  ; seek to monster hp
  ld hl, MonsterHPs
  ld a, e
  call addAToHL

  ld a, [hl]
  dec a
  ld [hl], a

  ld a, 1
  dec a

  ret

.notFound
  ld a, 2
  dec a ; set nz

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
  call getNextSpriteAddress

  ld a, [playerWorldY]
  inc a
  inc a ; add 16, sprites start off-screen
  sla a
  sla a
  sla a
  ld [de], a
  inc de

  ld a, [playerWorldX]
  inc a ; add 8 sprites start off-screen
  sla a
  sla a
  sla a
  ld [de], a
  inc de

  ld a, 0
  ld [de], a
  inc de
  ld [de], a

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

  ld a, PLAYER_INITIAL_HP
  ld [playerHP], a

  ret

ENDC
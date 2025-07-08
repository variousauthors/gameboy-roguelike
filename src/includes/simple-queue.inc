IF !DEF(SIMPLE_QUEUE_INC)
DEF SIMPLE_QUEUE_INC EQU 1

Section "SimpleQueueState", WRAM0

readPointerSimpleQueue: ds 2
writePointerSimpleQueue: ds 2

; is a queue of addresses
SimpleQueue: ds 10
SimpleQueueEnd:

Section "SimpleQueue", ROM0

initSimpleQueue:
  ld hl, SimpleQueue
  ld de, readPointerSimpleQueue
  call updatePointer

  ld hl, SimpleQueue
  ld de, writePointerSimpleQueue
  call updatePointer

  ret

; @param z - queue is empty
isEmptySimpleQueue:
  call addressIsEqual
  ret z

  ret

; @param a - element to insert
enqueueSimpleQueue:
  ; write it
  ld hl, writePointerSimpleQueue
  call dereferencePointer

  ld [hli], a

  ; advance the write pointer
  ; assumes we never reach the end before re-init
  ld de, writePointerSimpleQueue
  call updatePointer

  ret

; @return a - element in the queue
; @return z - queue empty
dequeueSimpleQueue:
  call isEmptySimpleQueue
  ret z

  ld hl, readPointerSimpleQueue
  call dereferencePointer

  ld a, [hli]

  ld de, readPointerSimpleQueue
  call updatePointer

  ret

ENDC
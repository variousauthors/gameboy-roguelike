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

; @param hl - element to insert (2 bytes)
enqueueSimpleQueue:
  push hl
  push hl

  ; get the address in the queue to write to
  ld hl, writePointerSimpleQueue
  call dereferencePointer

  pop de

  ; write two bytes
  ld a, d
  ld [hli], a
  ld a, e
  ld [hli], a

  ld de, writePointerSimpleQueue
  call updatePointer

  pop hl

  ret

; @return hl - next element (2 bytes)
; @return z - queue empty
dequeueSimpleQueue:
  call isEmptySimpleQueue
  ret z

  ld hl, readPointerSimpleQueue
  call dereferencePointer

  ; we have to adjust the readpointer
  ; before we get the value to return
  ; because of register pressure
  push hl

  inc hl
  inc hl ; advance the read pointer
  ld de, readPointerSimpleQueue
  call updatePointer

  ; recover the address of the return value in the queue
  pop hl

  ; we want to return the actual address
  call dereferencePointer

  ret

ENDC
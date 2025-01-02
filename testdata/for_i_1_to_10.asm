; =================================================
; for(int i=1; i<10; i++)
; =================================================
  	LD R0, 1 		; R0 = 1;
start_loop:		; for(; R0<=10; R0++)
  	CMP R0, 10		; done if Z or: (R0-10>0)
  	BRP $end_loop	; if Pos, jump to end_loop
  	; do something
  	ADD R0, 1		; R0++
  	BRA $start_loop	; jump to beginning of loop
end_loop:
  	HALT

.entry r1
.entry LENGTH
.extern L3
.extern W
MAIN: 	mov #5,LOOP
	add r2,STR
LOOP: 	jmp &STR
	prn #-5
	
	sub r1, r4
	inc K
	mov L3, r7
	
	bne L3
END: 	stop
STR: 	.string "abcdef
LENGTH:	.data 6,-9,15
K: 	.data 2,3.23 

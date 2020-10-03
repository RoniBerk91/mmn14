; This is a testing file for mmn14.
; By Lotem Ben Yaakov

.entry r8
.entry str1

main:	add **, r3
	cmp X, #-1234
data:	.data 6, 75
label1:	stop
	red vAr
	bne str3
	.data -17, -847, 123, 45
	prn #124
	bne r4
.entry Rst
str1:	.string "mmn14"

; "extern" is a legal label name.
.extern extern

label2:	dec X
	sub ***, str1
r8:	.data -32498

.entry label2
	jsr r6
uselessLabel: .entry label1
	lea ***, extern
	inc main
.extern X

; This is one string.
vAr:	.string "hello" "world" 
Rst:	rst
	.data 8876, 0
str2: 	.string ".data"
.extern str3
.entry str2




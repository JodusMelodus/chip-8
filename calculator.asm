LDVN 1 5		; Store 5 in V1
LDP 0			; Wait for key press and store in V0
CLS				; Clear screen
LDIF 0			; Load I with V0th font
DRW 1 1 5		; Draw character at (V1, V1)
JP 0x202		; Jump to line 2
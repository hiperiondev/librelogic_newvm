LD %i0
ST %m0
EQ %m3
JMP? end

LD !%i1/3
ST %m1
EQ %m3
JMP? end

while:LD %m0    ; A
EQ %m1          ; A == B
JMP? endwhile    ; while(A != B)
    LD %m0
    LT %m1      ; A < B
    JMP? reverse
    LD %m0
    SUB %m1     ; A - B
    ST %m0      ; A = A - B
JMP while
reverse:LD %m1  ; B
    SUB %m0     ; B - A
    ST %m1      ; B = B - A
JMP while
endwhile:LD %m0 ; 
ST %q0 ; output gcd 
end: LD %m3;

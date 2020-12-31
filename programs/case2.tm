; Input: an expression of 1's e.g. 11111x11=1111111111

; the finite set of states
#Q = {0,mul0,mul,cp,cmp,mh,mh2,clone_mh,clone,accept,accept2,accept3,accept4,accept_mh,halt_accept,reject_mh,reject,reject2,reject3,reject4,reject5,halt_reject,clear,clear_reject}

; the finite set of input symbols
#S = {1,x,=}

; the complete set of tape symbols
#G = {1,x,=,_,t,r,u,e,f,a,l,s}

; the start state
#q0 = 0

; the blank symbol
#B = _

; the set of final states
#F = {halt_accept}

; the number of tapes
#N = 2

; the transition functions

; State 0: start state
0 1_ 1_ ** cp
0 x_ x_ ** reject_mh
0 =_ =_ ** reject_mh
0 __ __ ** reject ; empty input

; State cp: copy the string "1^m" to the 2nd tape 
cp 1_ 11 rr cp
cp x_ x_ r* mul0
cp =_ =_ ** reject_mh
cp __ __ l* reject_mh

; State mul0: change the first bit of "1^n" to 'x'
mul0 1_ x_ r* mul
mul0 x_ x_ ** reject_mh
mul0 =_ =_ ** reject_mh
mul0 __ __ l* reject_mh

; State mul: start multiply
mul 1_ x_ l* clone_mh
mul =_ =_ rl cmp
mul x_ x_ ** reject_mh
mul __ l_ ** reject_mh

; clone_mh: move the head to the end of "1^m"
clone_mh x_ x_ l* clone_mh
clone_mh 1_ 1_ ** clone

; clone: start make copies of "1^m"
clone 1_ 11 lr clone
clone __ __ r* mh

; State mh: move to the first bit of "x..x"
mh 1_ 1_ r* mh
mh x_ x_ ** mh2

; State mh2: move to the last bit of "x..x"
mh2 x_ x_ r* mh2
mh2 1_ 1_ ** mul 
mh2 =_ =_ rl cmp

; State cmp: compare two strings
cmp 11 11 rl cmp
cmp 1_ 1_ ** reject_mh
cmp x1 x1 ** reject_mh
cmp x_ x_ ** reject_mh
cmp =1 =1 ** reject_mh
cmp =_ =_ ** reject_mh
cmp _1 _1 l* reject_mh
cmp __ __ l* accept_mh

; State reject_mh: move 1st head to the leftmost
reject_mh 1_ 1_ l* reject_mh
reject_mh 11 11 l* reject_mh
reject_mh =_ =_ l* reject_mh
reject_mh =1 =1 l* reject_mh
reject_mh x_ x_ l* reject_mh
reject_mh x1 x1 l* reject_mh
reject_mh __ __ ** reject
reject_mh _1 _1 ** reject

; State accept_mh: move 1st head to the leftmost
accept_mh 1_ 1_ l* accept_mh
accept_mh =_ =_ l* accept_mh
accept_mh x_ x_ l* accept_mh
accept_mh __ __ ** accept

; State accept*: write 'true' on 1st tape
accept __ t_ r* accept2
accept2 1_ r_ r* accept3
accept3 1_ u_ r* accept4
accept3 x_ u_ r* accept4
accept4 1_ e_ r* clear
accept4 x_ e_ r* clear

; State clear: clear the rest of 1st tape
clear 1_ __ r* clear
clear x_ __ r* clear
clear =_ __ r* clear
clear __ __ ** halt_accept

; State reject*: write 'false' on 1st tape
reject _1 f1 r* reject2
reject __ f_ r* reject2
reject2 _1 a1 r* reject3
reject2 __ a_ r* reject3
reject2 11 a1 r* reject3
reject2 1_ a_ r* reject3
reject2 =1 a1 r* reject3
reject2 =_ a_ r* reject3
reject2 x1 a1 r* reject3
reject2 x_ a_ r* reject3
reject3 _1 l1 r* reject4
reject3 __ l_ r* reject4
reject3 11 l1 r* reject4
reject3 1_ l_ r* reject4
reject3 =1 l1 r* reject4
reject3 =_ l_ r* reject4
reject3 x1 l1 r* reject4
reject3 x_ l_ r* reject4
reject4 _1 s1 r* reject5
reject4 __ s_ r* reject5
reject4 11 s1 r* reject5
reject4 1_ s_ r* reject5
reject4 =1 s1 r* reject5
reject4 =_ s_ r* reject5
reject4 x1 s1 r* reject5
reject4 x_ s_ r* reject5
reject5 _1 e1 r* clear_reject
reject5 __ e_ r* clear_reject
reject5 11 e1 r* clear_reject
reject5 1_ e_ r* clear_reject
reject5 =1 e1 r* clear_reject
reject5 =_ e_ r* clear_reject
reject5 x1 e1 r* clear_reject
reject5 x_ e_ r* clear_reject

; State clear_reject: clear the rest of 1st tape
clear_reject 1_ __ r* clear_reject
clear_reject 11 _1 r* clear_reject
clear_reject x_ __ r* clear_reject
clear_reject x1 _1 r* clear_reject
clear_reject =_ __ r* clear_reject
clear_reject =1 __ r* clear_reject
clear_reject __ __ ** halt_reject
clear_reject _1 _1 ** halt_reject
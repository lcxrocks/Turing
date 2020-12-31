; Input: a string of a's and b's, e.g. 'aaabbaaabb'

; the finite set of states
#Q = {0,look_a,look_b,cp,cmp,mh,mh2,accept,accept2,accept3,accept4,halt_accept,reject_mh,reject,reject2,reject3,reject4,reject5,halt_reject,clear,clear_reject}

; the finite set of input symbols
#S = {a,b}

; the complete set of tape symbols
#G = {a,b,_,t,r,u,e,f,l,s}

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
0 a_ a_ ** look_a
0 b_ b_ ** reject_mh
0 __ __ ** reject_mh ; empty input

; look_a: look out for 'a'
look_a a_ a_ r* look_a
look_a b_ b_ r* look_b
look_a __ __ l* reject_mh ; no input 'b'

; look_b: look out for 'b'
look_b b_ b_ r* look_b
look_b a_ a_ ** cp
look_b __ __ l* reject_mh ; no 2nd input 'a'

; State cp: copy the string "a^ib^j" to the 2nd tape 
cp a_ aa rr cp
cp b_ bb rr cp
cp __ __ ll mh

; State mh: move 1st head to the end of "a^ib^ja^i"
mh bb bb l* mh
mh ab ab l* mh2
mh ba ba ** reject_mh
mh aa aa ** reject_mh

; State mh2: move 1st head to the end of "a^ib^j"
mh2 ab ab l* mh2
mh2 bb bb ** cmp

; State cmp: compare two strings
cmp bb bb ll cmp
cmp aa aa ll cmp
cmp __ __ ** accept
cmp ab ab ll reject_mh
cmp ba ba ll reject_mh
cmp a_ a_ ll reject_mh
cmp b_ b_ ll reject_mh
cmp _a _a ** reject 
cmp _b _b ** reject 

; State reject_mh: move 1st head to the leftmost
reject_mh _a _a ** reject
reject_mh _b _b ** reject
reject_mh __ __ ** reject
reject_mh aa aa l* reject_mh
reject_mh ab ab l* reject_mh 
reject_mh a_ a_ l* reject_mh
reject_mh ba ba l* reject_mh
reject_mh bb bb l* reject_mh
reject_mh b_ b_ l* reject_mh

; State accept*: write 'true' on 1st tape
accept __ t_ r* accept2
accept _a ta r* accept2
accept _b tb r* accept2
accept2 a_ r_ r* accept3
accept2 aa ra r* accept3
accept2 ab rb r* accept3
accept3 a_ u_ r* accept4
accept3 aa ua r* accept4
accept3 ab ub r* accept4
accept3 b_ u_ r* accept4
accept3 ba ua r* accept4
accept3 bb ub r* accept4
accept4 a_ e_ r* clear
accept4 aa ea r* clear
accept4 ab eb r* clear
accept4 b_ e_ r* clear
accept4 ba ea r* clear
accept4 bb eb r* clear

; State clear: clear the rest of 1st tape
clear a_ __ r* clear
clear aa _a r* clear
clear ab _b r* clear
clear b_ __ r* clear
clear ba _a r* clear
clear bb _b r* clear
clear _a _a ** halt_accept
clear _b _b ** halt_accept
clear __ __ ** halt_accept

; State reject*: write 'false' on 1st tape
reject __ f_ r* reject2
reject _a fa r* reject2
reject _b fb r* reject2
reject2 a_ a_ r* reject3
reject2 aa aa r* reject3
reject2 ab ab r* reject3
reject2 __ a_ r* reject3
reject2 _a aa r* reject3
reject2 _b ab r* reject3
reject2 b_ a_ r* reject3
reject2 ba aa r* reject3
reject2 bb ab r* reject3
reject3 a_ l_ r* reject4
reject3 aa la r* reject4
reject3 ab lb r* reject4
reject3 b_ l_ r* reject4
reject3 ba la r* reject4
reject3 bb lb r* reject4
reject3 __ l_ r* reject4
reject3 _a la r* reject4
reject3 _b lb r* reject4
reject4 a_ s_ r* reject5
reject4 aa sa r* reject5
reject4 ab sb r* reject5
reject4 b_ s_ r* reject5
reject4 ba sa r* reject5
reject4 bb sb r* reject5
reject4 __ s_ r* reject5
reject4 _a sa r* reject5
reject4 _b sb r* reject5
reject5 a_ e_ r* clear_reject
reject5 aa ea r* clear_reject
reject5 ab eb r* clear_reject
reject5 b_ e_ r* clear_reject
reject5 ba ea r* clear_reject
reject5 bb eb r* clear_reject
reject5 __ e_ r* clear_reject
reject5 _a ea r* clear_reject
reject5 _b eb r* clear_reject

; State clear_reject: clear the rest of 1st tape
clear_reject a_ __ r* clear_reject
clear_reject aa _a r* clear_reject
clear_reject ab _b r* clear_reject
clear_reject b_ __ r* clear_reject
clear_reject ba _a r* clear_reject
clear_reject bb _b r* clear_reject
clear_reject _a _a ** halt_reject
clear_reject _b _b ** halt_reject
clear_reject __ __ ** halt_reject
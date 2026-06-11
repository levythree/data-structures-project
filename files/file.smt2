; Integer arithmetic
(set-logic QF_LIA)
(declare-const x Int)
(declare-const y Int)
(assert (<= (- x (* 2 y)) 20))
(assert (>= (- x (* 2 y)) 21))
(check-sat)
; unsat
(exit)
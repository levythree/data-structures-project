; Integer arithmetic
(set-logic QF_LIA)
(declare-const x Int)
(declare-const y Int)
(assert (= (- x y) (+ x (- y) 1)))
(assert (= (+ x (* 2 y)) 20))
(check-sat)
; unsat
(exit)
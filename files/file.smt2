; Integer arithmetic
(set-logic QF_LIA)
(declare-const x Int)
(declare-const y Int)
(assert (or (= x 5) (= x 10)))
(assert (<= (- x) 0))

(true, false, true) -> Simplex

(check-sat)
; unsat
(exit)
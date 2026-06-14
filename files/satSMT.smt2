(declare-const w1 Int)
(declare-const w2 Int)
(declare-const w3 Int)
(declare-const w4 Int)

; w1 >= 0
; w2 >= 0
; w3 >= 0
; w4 >= 0
; w1 + w2 + w3 + w4 = 35
; 3w3 + 2w4 = 27
; w2 >= w1 -> w2 - w1 >= 0
; (w4 >= 10) OR (w4 <= 2)
; w1 + w2 >= 25
; (w1 >= 15) OR (w1 <= 5)

(assert (>= w1 0))
(assert (>= w2 0))
(assert (>= w3 0))
(assert (>= w4 0))

(assert (= (+ w1 w2 w3 w4) 35))

(assert (= (+ (* 3 w3) (* 2 w4)) 27))

(assert (>= w2 w1))

(assert (or
    (>= w4 10)
    (<= w4 2)
))

(assert (>= (+ w1 w2) 25))

(assert (or
    (>= w1 15)
    (<= w1 5)
))

(check-sat)
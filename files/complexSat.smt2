(declare-const p1 Int)
(declare-const p2 Int)
(declare-const p3 Int)
(declare-const p4 Int)
(declare-const p5 Int)

(assert (>= p1 0))
(assert (>= p2 0))
(assert (>= p3 0))
(assert (>= p4 0))
(assert (>= p5 0))

; p1 + p2 + p3 + p4 + p5 = 25
; (p1 >= 10) OR (p1 <= 0)
; p2 - p1 >= 0
; 3p3 + 2p4 = 17
; (p5 <= 2) OR (p5 >= 20)
; (p4 = 7) OR (p4 = 4) OR (p4 = 1)

(assert (= (+ p1 p2 p3 p4 p5) 25))

(assert (or
    (>= p1 10)
    (<= p1 0)
))

(assert (>= (- p2 p1) 0))

(assert (= (+ (* 3 p3) (* 2 p4)) 17))

(assert (or
    (<= p5 2)
    (>= p5 20)
))

(assert (or
    (= p4 7)
    (or
        (= p4 4)
        (= p4 1)
    )
))

(check-sat)
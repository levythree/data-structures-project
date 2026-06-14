(declare-const p1 Int)
(declare-const p2 Int)
(declare-const p3 Int)
(declare-const p4 Int)

; (p1 <= 0) OR (p2 >= 10)
; (p1 >= 1) OR (2p2 >= 3)
; (p1 >= 1) OR (2p3 >= 3)
; (p1 >= 1) OR (2p2 + 2p3 <= 7)
; (p1 <= 9) OR (2p4 + 4p2 = 15)

(assert (or (<= p1 0) (>= p1 10)))

(assert (or (>= p1 1) (>= (* 2 p2) 3)))
(assert (or (>= p1 1) (>= (* 2 p3) 3)))
(assert (or (>= p1 1) (<= (+ (* 2 p2) (* 2 p3)) 7)))

(assert (or (<= p1 9) (= (+ (* 2 p4) (* 4 p2)) 15)))

(check-sat)
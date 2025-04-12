(set-logic AUFLIA)

(assert (forall ((x Int)) (=> (> x 0) (not (= x 1)))))
(assert (not (forall ((x Int)) (not (= x 1)))))

(check-sat)

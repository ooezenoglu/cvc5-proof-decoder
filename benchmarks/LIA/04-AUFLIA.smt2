(set-logic LIA)
(assert (forall ((x Int)) (not (= x 0))))
(assert (exists ((x Int)) (= x 0)))
(check-sat)

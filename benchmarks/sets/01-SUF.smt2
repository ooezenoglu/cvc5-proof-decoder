(declare-sort $$unsorted 0)
(declare-fun tptp.subset ($$unsorted $$unsorted) Bool)
(assert (not (forall ((A $$unsorted)) (let ((_let_1 (tptp.subset A A))) 
        (=> _let_1 _let_1)))))
(assert (forall ((A $$unsorted)) (tptp.subset A A)))
(check-sat)
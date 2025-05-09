(set-info :smt-lib-version 2.6)
(set-logic NIA)
(set-info
  :source |
 Generated by PSyCO 0.1
 More info in N. P. Lopes and J. Monteiro. Weakest Precondition Synthesis for
 Compiler Optimizations, VMCAI'14.
|)
(set-info :category "industrial")
(set-info :status unsat)
(declare-fun W_S1_V6 () Bool)
(declare-fun W_S1_V2 () Bool)
(declare-fun W_S1_V3 () Bool)
(declare-fun W_S1_V1 () Bool)
(declare-fun R_S1_V3 () Bool)
(declare-fun R_S1_V1 () Bool)
(declare-fun R_S1_V6 () Bool)
(declare-fun R_S1_V5 () Bool)
(declare-fun R_S1_V2 () Bool)
(declare-fun DISJ_W_S1_R_S1 () Bool)
(declare-fun W_S1_V5 () Bool)
(assert
 (let
 (($x21121
   (forall
    ((V2_0 Int) (V5_0 Int) 
     (V6_0 Int) (MW_S1_V1 Bool) 
     (MW_S1_V3 Bool) (MW_S1_V2 Bool) 
     (MW_S1_V5 Bool) (MW_S1_V6 Bool) 
     (S1_V3_!1741 Int) (S1_V3_!1746 Int) 
     (S1_V1_!1740 Int) (S1_V1_!1745 Int) 
     (S1_V2_!1742 Int) (S1_V2_!1747 Int) 
     (S1_V5_!1743 Int) (S1_V5_!1748 Int) 
     (S1_V6_!1744 Int) (S1_V6_!1749 Int))
    (let ((?x21214 (ite MW_S1_V6 S1_V6_!1749 V6_0)))
    (let ((?x21212 (ite MW_S1_V6 S1_V6_!1744 V6_0)))
    (let (($x21216 (= ?x21212 ?x21214)))
    (let ((?x21208 (ite MW_S1_V5 S1_V5_!1748 V5_0)))
    (let ((?x21206 (ite MW_S1_V5 S1_V5_!1743 V5_0)))
    (let (($x21210 (= ?x21206 ?x21208)))
    (let ((?x21188 (ite MW_S1_V2 S1_V2_!1747 V2_0)))
    (let ((?x21174 (ite MW_S1_V2 S1_V2_!1742 V2_0)))
    (let (($x21204 (= ?x21174 ?x21188)))
    (let ((?x21198 (+ (- 1) ?x21188)))
    (let ((?x21052 (ite MW_S1_V3 S1_V3_!1741 0)))
    (let (($x21202 (= ?x21052 ?x21198)))
    (let ((?x21060 (ite MW_S1_V1 S1_V1_!1740 0)))
    (let (($x21200 (= ?x21060 ?x21198)))
    (let (($x21220 (and $x21200 $x21202 $x21204 $x21210 $x21216)))
    (let ((?x21190 (* ?x21188 ?x21188)))
    (let (($x21192 (>= 1 ?x21190)))
    (let ((?x21182 (* V2_0 V2_0)))
    (let (($x21184 (<= ?x21182 0)))
    (let (($x21186 (not $x21184)))
    (let ((?x21176 (+ (- 1) ?x21174)))
    (let (($x21180 (>= ?x21060 ?x21176)))
    (let (($x21178 (>= ?x21052 ?x21176)))
    (let (($x21170 (<= V2_0 0)))
    (let (($x21172 (not $x21170)))
    (let (($x21194 (and $x21172 $x21178 $x21180 $x21186 $x21192)))
    (let (($x21196 (not $x21194)))
    (let (($x21078 (not MW_S1_V6)))
    (let (($x21079 (or $x21078 W_S1_V6)))
    (let (($x21082 (not MW_S1_V2)))
    (let (($x21083 (or $x21082 W_S1_V2)))
    (let (($x21084 (not MW_S1_V3)))
    (let (($x21085 (or $x21084 W_S1_V3)))
    (let (($x21086 (not MW_S1_V1)))
    (let (($x21087 (or $x21086 W_S1_V1)))
    (let (($x21089 (= S1_V6_!1749 S1_V6_!1744)))
    (let (($x94 (not R_S1_V3)))
    (let (($x21141 (or $x94 (= (* (div 0 V2_0) V2_0) 0))))
    (let ((?x21136 (div 0 V2_0)))
    (let (($x21137 (= ?x21136 0)))
    (let (($x92 (not R_S1_V1)))
    (let (($x21138 (or $x92 $x21137)))
    (let (($x21144 (not (and $x21138 $x21141))))
    (let
    (($x20975
      (and (or $x21144 (= S1_V3_!1746 S1_V3_!1741))
      (or $x21144 (= S1_V1_!1745 S1_V1_!1740))
      (or (not (and (or $x92 (= 0 ?x21136)) $x21141))
      (= S1_V2_!1742 S1_V2_!1747))
      (or (not (and (or $x92 (= 0 ?x21136)) $x21141))
      (= S1_V5_!1743 S1_V5_!1748)) 
      (or $x21144 $x21089) $x21087 $x21085 $x21083 $x21079)))
    (or (not $x20975) $x21196 $x21220))))))))))))))))))))))))))))))))))))))))))))))))
 (let (($x21 (and W_S1_V6 R_S1_V6)))
 (let (($x16 (and W_S1_V2 R_S1_V2)))
 (let (($x13 (and W_S1_V3 R_S1_V3)))
 (let (($x10 (and W_S1_V1 R_S1_V1)))
 (let (($x28 (or $x10 $x13 $x16 R_S1_V5 $x21)))
 (let (($x29 (not $x28)))
 (let (($x30 (= DISJ_W_S1_R_S1 $x29))) (and W_S1_V5 $x30 $x21121))))))))))
(assert
 (let (($x20284 (not W_S1_V2)))
 (let (($x20278 (not W_S1_V3)))
 (let (($x20266 (not W_S1_V1)))
 (let (($x22302 (and $x20266 $x20278 $x20284))) (not $x22302))))))
(check-sat)
(exit)


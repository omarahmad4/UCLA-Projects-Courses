let my_subset_test0 = subset [] []
let my_subset_test1 = subset [] [1]
let my_subset_test2 = subset [1] [1]
let my_subset_test3 = subset [1;1] [1]
let my_subset_test4 = not (subset [1] [])
let my_subset_test5 = not (subset [1;2;3] [2;3])
let my_subset_test6 = not (subset [1;2;3] [1;3])

let my_equal_sets_test0 = equal_sets [] []
let my_equal_sets_test1 = equal_sets [1] [1]
let my_equal_sets_test2 = equal_sets [1;2;3] [1;1;1;2;2;2;3;2;2;]
let my_equal_sets_test3 = not (equal_sets [1;2;3] [])
let my_equal_sets_test4 = not (equal_sets [] [1;2;3])
let my_equal_sets_test5 = not (equal_sets [1;2;3] [1;2;5;3])
				  
let my_set_union_test0 = equal_sets( set_union [] [] ) []
let my_set_union_test1 = equal_sets( set_union [1] [2] ) [1;2]
let my_set_union_test2 = equal_sets( set_union [1;2;2;1] [] ) [1;2]
let my_set_union_test3 = equal_sets( set_union [] [1] ) [1]
let my_set_union_test4 = not (equal_sets( set_union [1] [2] ) [1] )
let my_set_union_test5 = not (equal_sets( set_union [] [5] ) [] )

let my_set_intersection_test0 = equal_sets( set_intersection [] [] ) []
let my_set_intersection_test1 = equal_sets( set_intersection [1] [2;1;1] ) [1]
let my_set_intersection_test2 = equal_sets( set_intersection [2;3;4;5;5] [3;3] ) [3]
let my_set_intersection_test3 = equal_sets( set_intersection [] [1;2] ) []
let my_set_intersection_test4 = equal_sets( set_intersection [1;2] [] ) []
let my_set_intersection_test5 = not (equal_sets( set_intersection [1;2;3;4] [2;4;6;8] ) [4] )

let my_set_diff_test0 = equal_sets( set_diff [] [1;2;3] ) []
let my_set_diff_test1 = equal_sets( set_diff [1;1;1;1] [1;2;3] ) []
let my_set_diff_test2 = equal_sets( set_diff [1;2;3;4] [1;2;3] ) [4]
let my_set_diff_test3 = equal_sets( set_diff [2;4;6;7] [1;2;3] ) [4;6;7]
let my_set_diff_test4 = equal_sets( set_diff [] [] ) []
let my_set_diff_test5 = equal_sets( set_diff [1;2;2;3] [] ) [1;2;3]

let my_computed_fixed_point_test0 = computed_fixed_point (=) (fun x-> x/4) 10000 = 0

type test_nonterminals =
 | Phrase | Noun | Verb | Adj

let my_filter_reachable_test0 = filter_reachable ( Phrase,
						   [ Phrase, [N Noun; N Verb];
						     Noun, [T"mary"];
						     Verb, [T"eats"];
						     Adj,  [T"fast"] ] )
                                                  =
						 (Phrase,
						  [ Phrase, [N Noun; N Verb];
						    Noun, [T"mary"];
						    Verb, [T"eats"] ] )

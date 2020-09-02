type my_NTs =
  Sent | Phrase | Noun | Verb 

let my_rules = 
   [
     Sent, [N Phrase; T"and"; N Phrase];
     Sent, [N Phrase];
     Phrase, [N Noun; N Verb];
     Noun, [T"mark"];
     Noun, [T"mary"];
     Verb, [T"eats"];
     Verb, [T"drinks"] ]

let my_grammar = Sent, my_rules 

let hw2_gram = convert_grammar my_grammar

let acc_empty = function
|[] -> Some []
| _ -> None

let testFrag1 = ["mark"; "eats"; "and"; "mary"; "jumps"] 
let testFrag2 = ["mark"; "eats"; "and"; "mary"; "drinks"] 

let make_matcher_test = ( make_matcher hw2_gram acc_empty testFrag1 = None )

let make_parser_test = match (make_parser hw2_gram testFrag2) with
| Some retVal -> parse_tree_leaves retVal = testFrag2
| _ -> false 


(* for testing other functions and debugging 

let test_start_symbol =  fst my_grammar  = fst my_grammar_hw2

let test_Phrase = (snd my_grammar_hw2) Phrase = [  [N Noun; N Verb]  ]

let test_Noun = (snd my_grammar_hw2) Noun = [ [T"mark"]; [T"mary"] ]

let test_Verb = (snd my_grammar_hw2) Verb = [ [T"eats"]; [T"drinks"] ]

let my_parse_tree_leaves_test0 = (parse_tree_leaves 
(Node("alpha",[Leaf 1;Leaf 2;Node("beta",[Leaf 3]);Leaf 4])) = [1;2;3;4])

let test_output = make_parser  my_grammar_hw2 ["mark"; "eats"] ;;
let test_output2 = make_parser  my_grammar_hw2 
let test_output3 = make_parser  my_grammar_hw2 ["mark"; "mary"] ;;

*)

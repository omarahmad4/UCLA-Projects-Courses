type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal

type ('nonterminal, 'terminal) parse_tree =
  | Node of 'nonterminal * ('nonterminal, 'terminal) parse_tree list
  | Leaf of 'terminal

(* ------------ HW2 #1 -------------*)
(* find_all_NT_rules takes in old gram rules, [], and a lhs(NT) and returns
 * a list of all rhs lists in old gram w lhs = lhs 
 * ex: call w rules []  Expr returns [ [N Term; N Binop; N Expr]; [N Term] ]
 * if rules = [... (Expr, [N Term;N Binop;N Term); (Expr, [N Term]); ...] *)
let rec find_all_NT_rules rules newGrammer lhs = match rules with
   |[] -> newGrammer
   | hd::tl -> match hd with  (* hd is a rule: (NT, [list] *)
      | (nonterminal, rhs) ->  (* nonterminal is NT; rhs is its rule list*)
         if nonterminal = lhs  (* ret rhs if nt = lhs else cont thru rules*)
         then
	    let currGram = List.append newGrammer [rhs]
	    in
	    find_all_NT_rules tl currGram lhs
         else find_all_NT_rules tl newGrammer lhs

(* convert hw1 style grammer to hw2 style
* utilizes currying with find_all_NT_rules: doesn't pass in the last arg
* so that it returns a function as in hw2 style grammer *)
let convert_grammar gram1 = match gram1 with 
  | (start, rules) -> (start, find_all_NT_rules rules [])


(* ----------- #2 ---------- *)
(* traverses parse tree left to right and rets list of leaves encountered *)
(* leaves_helper takes list so that can send in t2 as is *)
let rec leaves_helper tree = match tree with
|[] -> []
|hd::tl -> match hd with
   |Leaf l -> l::(leaves_helper tl)
   |Node (t1, t2) -> (leaves_helper t2) @ (leaves_helper tl)
   
let parse_tree_leaves tree = leaves_helper [tree] 


(* ------------ #3 ----------- *)
(* make_matcher returns a matcher for the grammar gram. When applied to an acceptor accept and a fragment frag,
the matcher must try the grammar rules in order and return the result of calling accept on the suffix corresponding
to the first acceptable matching prefix of frag; this is not necessarily the shortest or longest acceptable match.
A match is considered to be acceptable if accept succeeds when given the suffix fragment that immediately follows
the matching prefix. When this happens, the matcher returns whatever the acceptor returned.
If no acceptable match is found, the matcher returns None.
matcher : a curried function with two arguments: an acceptor accept and a fragment frag.
A matcher matches a prefix p of frag such that accept (when passed the corresponding suffix) accepts the
corresponding suffix (i.e., the suffix of frag that remains after p is removed). If there is such a match, the
matcher returns whatever accept returns; otherwise it returns None. 
acceptor : a function whose argument is a fragment frag. If the fragment is not acceptable, it returns None; otherwise it returns Some x for some value x.
fragment : a list of terminal symbols, e.g., ["3"; "+"; "4"; "xyzzy"]. *)

(* matcher_helper (w/ ruleFunc and currRule filled) is the matcher that is returned by make_matcher *)
let rec matcher_helper ruleFunc currRule accept frag = match currRule with  
| [] -> None  (* no acceptable match found *)
| ruleHD :: ruleTL ->
(*match prefix of frag with ruleHD and call acceptor on suffix; if yes ret what acceptor returned
  else call again on rest of rules (ruleTL)
  if go through all rules and find no matches, return None (empty list case at beginning of matcher_helper) *)
    match (find_matches ruleHD ruleFunc accept frag) with
    | None -> matcher_helper ruleFunc ruleTL accept frag
    | retVal -> retVal 

and

(* takes one rule (ex: [T"("; N Expr; T")"] ) and uses accept to match with frag *)
find_matches rule ruleFunc accept frag = match rule with (* rule is empty, or starts with N/T *)
| [] -> accept frag
| N nonterm :: restRules ->
     let     (* replace newRule with new set of rules from ruleFunc corresponding to nonterm *)
     newRule = ruleFunc nonterm
     and     (* replace accept in matcher_helper call with a recursive find_matches call on restRules *)
     newAccept = find_matches restRules ruleFunc accept
     in
     matcher_helper ruleFunc newRule newAccept frag
| T term  :: restRules -> match frag with
     | [] -> None (* cant match with empty frag *)
     | fragHD :: fragTL  -> 
          if fragHD = term
	    then find_matches restRules ruleFunc accept fragTL
	  else None

let make_matcher gram accept frag = match gram with  (*accept and frag left empty to ret matcher func*)
|(start, ruleFunc) ->
   let rules = ruleFunc start
   in
   matcher_helper ruleFunc rules accept frag 


(* ----------- #4 --------------- *)
(* make_parser gram returns a parser for the grammar gram. When applied to a fragment frag, the parser returns an
optional parse tree. If frag cannot be parsed entirely (that is, from beginning to end), the parser returns None.
Otherwise, it returns Some tree where tree is the parse tree corresponding to the input fragment.
   Using very similar mechanisms to #3 but have to keep track of what rules are applied and use that to build a
parse tree from the bottom up (leaves -> root) *)

let accept_empty = function (* acceptor that only accepts empty suffixes *) 
| [] -> Some []
| _  -> None 

(* very similar to matcher_helper except return each rule that was applied in list with what applied to *)
let rec parser_helper ruleFunc currRule accept frag = match currRule with  
| [] -> None  (* no acceptable match found *)
| ruleHD :: ruleTL ->
(*match prefix of frag with ruleHD and call acceptor on suffix;
  if yes ret ruleHD with what acceptor returned in a list wrapped in a Some 
  else call again on rest of rules (ruleTL)
  if go through all rules and find no matches, return None (empty list case at beginning of parser_helper) *)
    match (find_parse_matches ruleHD ruleFunc accept frag) with
    | None -> parser_helper ruleFunc ruleTL accept frag
    | Some retVal -> Some (ruleHD :: retVal) (* storing what rule is used *)

and

(* takes one rule (ex: [T"("; N Expr; T")"] ) and uses accept to match with frag - no diff from find_matches *)
find_parse_matches rule ruleFunc accept frag = match rule with (* rule is empty, or starts with N/T *)
| [] -> accept frag
| N nonterm :: restRules ->
     let     (* replace newRule with new set of rules from ruleFunc corresponding to nonterm *)
     newRule = ruleFunc nonterm
     and     (* replace accept in matcher_helper call with a recursive find_parse_matches call on restRules *)
     newAccept = find_parse_matches restRules ruleFunc accept
     in
     parser_helper ruleFunc newRule newAccept frag 
| T term  :: restRules -> match frag with
     | [] -> None (* cant match with empty frag *)
     | fragHD :: fragTL  -> 
          if fragHD = term
	    then find_parse_matches restRules ruleFunc accept fragTL
	  else None


(* return Some(parse tree) - only called if frag is parsable *)
let build_parse_tree parseData start =
    let rec builder_helper parseData = function
    | [] -> parseData, []
    | dataHead :: dataTail ->
      	    match find_children parseData dataHead with (* 1 - works w/2(below) to get to end of parseData *)
	    | pathRem, currSib ->
	         match builder_helper pathRem dataTail with (* attaches siblings of the applied rule *)
		 | rule, applied ->
		      (rule, currSib::applied)
    and
    find_children data = function
    | N nonterm -> (
      	 match data with
         | [] -> ([], Node (nonterm, [])) (* throwing away first elem at end *)
    	 | hd :: tl ->
	     match builder_helper tl hd with (* 2 - works w/1 to get to end of parseData to build tree bottom up *)
	     | rule, remRule ->
	         (rule, Node (nonterm, remRule)) )
    | T term -> (data, Leaf term)  (* create inner tuple of a Node (ex: (Noun, Leaf "mark") *)
in
match builder_helper parseData [N start] with
| junk, [retVal] -> Some(retVal)
| _ -> None

let make_parser gram frag = match gram with
|(start, ruleFunc) ->
   let rules = ruleFunc start
   in
   match parser_helper ruleFunc rules accept_empty frag with
   | None -> None
   | Some parseData -> build_parse_tree parseData start 




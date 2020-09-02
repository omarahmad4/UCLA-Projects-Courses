(* finding if the set represented by the list a is a subset of that of b *)
let rec subset a b = match (a, b) with  
    | ( [] ,  _ )  -> true
    | ( _  , [] )  -> false
    | (hd::tl, _ ) ->
       	if List.exists(fun x -> x = hd) b 
        then subset tl b
        else false;;

(* finding if the sets represented by a and b are equivalent *) 
let rec equal_sets a b = subset a b && subset b a;;

(* returning list representing the union of the sets that a and b represent *) 
let set_union a b = a @ b;;       

(* returning the interesection of the the sets a & b represent *)
let rec set_intersection a b = match (a, b) with  
    | ( [] , _ ) -> []
    | ( _ , [] ) -> []
    | (hd::tl,_) ->
	 if List.exists(fun x -> x = hd) b
	 then hd::set_intersection tl b
	 else set_intersection tl b;;

(* reutrning the diff a - b *) 
let rec set_diff a b = match (a, b) with      
    | ( [] , _ ) -> []
    | ( _ , [] ) -> a
    | (hd::tl,_) ->
         if List.exists(fun x -> x = hd) b
		 then set_diff tl b
		 else hd::set_diff tl b;;

(* if f(x) = x, that is a fixed point (can take any 'equality fucntion') *) 
let rec computed_fixed_point eq f x =   
    let comp_val = f x in 
    if eq comp_val x
    then x
    else computed_fixed_point eq f comp_val;;

type ('nonterminal, 'terminal) symbol =
     | N of 'nonterminal
     | T of 'terminal;;

(*extract non-terminals - call with RHS's of rules *)
let rec extractN = function   
    | [] -> []
    | N hd::tl -> hd :: (extractN tl)
    | T hd::tl -> extractN tl;;

(* get lhs and rhs of 2 element tuples *) 
let getLHS ( n , _ ) = n;;   
let getRHS ( _ , n ) = n;;

let rec findChildNs parents rules = match rules with   (* find direct children of parents *)
    | [] -> parents        	    	  	       (* if rules empty ret parents *)
    | hd :: tl ->				       (* else seperate out first rule (hd) *)
      let left = getLHS hd in			       (* left is NT lhs of hd rule *)
      if List.mem left parents				(* if curr lhs is in parents (ie a reachable NT) *)
      then
		let right = extractN (getRHS hd) in		(* right is all NTs on rhs of lhs *)	
		findChildNs (set_union parents right) tl	(* add right to parents and rec call with rest of rules *) 
	  else findChildNs parents tl;;			(* else if not in parents call again on rest of rules *)

let rec recursiveFindChildren parents rules =	(* helper to rec. call findChildNs to see when it stops changing *)
    let newNs = findChildNs parents rules in   	(* newNs is new set of reachable NTs from parents *)
    if equal_sets parents newNs		(* if newNs is same as parents it is the set of all reachable NTs *)
    then newNs
    else recursiveFindChildren newNs rules ;;	(* else call findChildNs until newNs is same as parents *)


let filter_reachable g = match g with		(* remove unreachable grammar rules *)
    | ( start , rules ) ->
    let reachableN = recursiveFindChildren [start] rules in (* find reachable non terminals from start *)
    let filteredRules = List.filter  (* filter out rules to remove those whose lhs does not appear in reahcableN *)
    (	fun n -> match n with
			|( x , _ ) -> List.mem x reachableN  
    )rules in
    (start, filteredRules) 	(* return in same format as g (input) *) 
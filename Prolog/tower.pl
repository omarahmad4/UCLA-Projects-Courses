/*
    tower/3 takes 3 args:
N, a nonnegative integer specifying the size of the square grid.
T, a list of N lists, each representing a row of the square grid. Each row is represented by a list of N distinct integers from 1 through N. The corresponding columns also contain all the integers from 1 through N.
C, a structure with function symbol counts and arity 4. Its arguments are all lists of N integers, and represent the tower counts for the top, bottom, left, and right edges, respectively.
Precondition. No solution will involve an integer that exceeds the vector_max of the GNU Prolog finite domain solver. Also, all arguments must be finite terms of the proper types. Your code need not check these preconditions; it can assume that the preconditions hold. Arguments can contain logical variables, except that the first argument N must be bound to a nonnegative integer.


    are a tower if:
  Valid:
    - each row has N elmts
    - each col has N elmts
    - all elmts b/w 1 and N
    - Sum of any row/col is SumN
    - SumN is N + N-1 + N-2 + ... + 1

    OR

    - is permutation of [1,2,...,N]

  Correct:
    - CanSee of any row/col is corresponding to that count
    - CanSee goes along row/col and counts times max changes
    
*/  

ambiguous(N, C, T1, T2):-
    tower(N, T1, C),
    tower(N, T2, C), 
    T1 \= T2.






speedup(SpeedupRatio) :-
    getTime(Time),
    getPlainTime(PlainTime),
    SpeedupRatio is PlainTime/Time.

getTime(TotalTime) :-
    statistics(cpu_time, [Start|_]),
    tower(5,[[2,3,4,5,1],[5,4,1,3,2],[4,1,5,2,3],[1,2,3,4,5],[3,5,2,1,4]],counts([2,3,2,1,4],[3,1,3,3,2],[4,1,2,5,2],[2,4,2,1,2])),
    tower(5,[[2,3,4,5,1],[5,4,1,3,2],[4,1,5,2,3],[1,2,3,4,5],[3,5,2,1,4]],_),
    tower(5,_,counts([2,3,2,1,4],[3,1,3,3,2],[4,1,2,5,2],[2,4,2,1,2])),
    statistics(cpu_time, [End|_]),
    TotalTime is End - Start + 1. %add one so that speedup ratio doesnt divide by 0 

getPlainTime(TotalTime) :-
    statistics(cpu_time, [Start|_]),
    plain_tower(5,[[2,3,4,5,1],[5,4,1,3,2],[4,1,5,2,3],[1,2,3,4,5],[3,5,2,1,4]],counts([2,3,2,1,4],[3,1,3,3,2],[4,1,2,5,2],[2,4,2,1,2])),
    plain_tower(5,[[2,3,4,5,1],[5,4,1,3,2],[4,1,5,2,3],[1,2,3,4,5],[3,5,2,1,4]],_),
    plain_tower(5,_,counts([2,3,2,1,4],[3,1,3,3,2],[4,1,2,5,2],[2,4,2,1,2])),
    statistics(cpu_time, [End|_]),
    TotalTime is End - Start.


plain_tower(N,T,C) :-
    C = counts(Top, Bot, Left, Right), 
    length(T, N), 
    maplist(revLen(N), T),
    length(Top, N),
    length(Bot, N),
    length(Left, N),
    length(Right, N),
    !,
    plain_checkValid(N, T, Left, Right),
    transpose(T, T_transpose),
    plain_checkValid(N, T_transpose, Top, Bot).


plain_checkValid(_, [], _, _).
plain_checkValid(N, [T_H|TRest], [F_H|ForwardRest],[B_H|BackwardRest]) :-
    prod_StandardList(N, Standard), 
    !,
    permutation(Standard, T_H),
    member(F_H, Standard),
    member(B_H, Standard),
    plain_checkCount(T_H, F_H, 0), 
    reverse(T_H, T_H_rev), 
    plain_checkCount(T_H_rev, B_H, 0),
    plain_checkValid(N, TRest, ForwardRest, BackwardRest).

plain_checkCount([], 0, _).
plain_checkCount([Curr|Rest], Count, CurrMax) :-
    Curr =< CurrMax,
    plain_checkCount(Rest, Count, CurrMax)
    ;
    Curr > CurrMax,
    NewCount is Count - 1,
    plain_checkCount(Rest, NewCount, Curr).


prod_StandardList(0, []).
prod_StandardList(N, [Head|Tail]) :-   %standardList is [N->1]; ie [3,2,1] for N = 3
    Head is N,
    NewN is N - 1,
    prod_StandardList(NewN, Tail).



tower(N,T,C) :-
    C = counts(Top, Bot, Left, Right),
    length(T, N), 
    maplist(revLen(N), T),
    maplist(revDomain(N), T),
    maplist(fd_all_different, T), 
    transpose(T, T_transpose),
    maplist(fd_all_different, T_transpose), 
    maplist(reverse, T, T_reverse),
    maplist(reverse, T_transpose, T_tran_rev),

    check_C(N,Top),
    check_C(N,Bot),
    check_C(N,Left),
    check_C(N,Right),

    checkVis(T, Left),
    checkVis(T_reverse, Right),
    checkVis(T_transpose, Top),
    checkVis(T_tran_rev, Bot),

    maplist(fd_labeling, T).

revLen(Len, List) :- %helper to use length in maplist
    length(List, Len).

revDomain(N, List) :- %helper to use fd_domain in maplist
    fd_domain(List, 1, N).

check_C(N,Cnt) :-     %check if C valid         
    length(Cnt, N),
    fd_domain(Cnt, 1, N).
    
unique_list(N, List) :-       % from Kimmo's disc slides - switched order of N and List to use with maplist
    length(List,N),           %create a list of len N w no bound vars
    fd_domain(List, 1, N),    %define all vals in list to be bw 1 & N
    fd_all_different(List),   %define all vals in list to be diff
    fd_labeling(List).        %find a sol - backgracking will gen a new sol

checkVis([], _).
checkVis([Row|Rest], [RowCount|RestCounts]) :-
    checkCount(Row, RowCount, 0),
    checkVis(Rest, RestCounts).

checkCount([], 0, _).
checkCount([Curr|Rest], Count, CurrMax) :-
    Curr #=< CurrMax,
    checkCount(Rest, Count, CurrMax)
    ;
    Curr #> CurrMax,
    NewCount #= Count - 1,
    checkCount(Rest, NewCount, Curr).

% This is SWI-prolog's old implementation
% from stackoverflow: https://stackoverflow.com/questions/4280986/how-to-transpose-a-matrix-in-prolog
transpose([], []).
transpose([F|Fs], Ts) :-
    transpose(F, [F|Fs], Ts).

transpose([], _, []).
transpose([_|Rs], Ms, [Ts|Tss]) :-
        lists_firsts_rests(Ms, Ts, Ms1),
        transpose(Rs, Ms1, Tss).

lists_firsts_rests([], [], []).
lists_firsts_rests([[F|Os]|Rest], [F|Fs], [Os|Oss]) :-
        lists_firsts_rests(Rest, Fs, Oss).



















    


/*

tower(N,T,C) :- 
    length(T,N),
    %C = counts(Top, Bot, Left, Right),
    valid(N,T,C),
    write('Is valid\n'),
    correct(N,T,C).

valid(N,T,C) :- 
    valid_T(N,T),
    transpose(T, T_transpose),
    valid_T(N,T_transpose),
    valid_C(N,C).

valid_T(N,T) :-
    length(T,0) 
    ;
    T = [Head|Tail],
    unique_list(Head, N),
    valid_T(N, Tail).

valid_C(N,C) :-
    C = counts(Top, Bot, Left, Right),
    check_C(N,Top),
    check_C(N,Bot),
    check_C(N,Left),
    check_C(N,Right).

correct(N,T,C) :-
    maplist(fd_labeling, T),
    C = counts(Top, Bot, Left, Right),
    transpose(T, T_transpose),
    maplist(reverse, T, T_reverse),
    maplist(reverse, T_transpose, T_tran_rev),
    checkVis(T, Left),
    checkVis(T_reverse, Right),
    checkVis(T_transpose, Top),
    checkVis(T_tran_rev, Bot).

correct(N,T,C) :-
    T = [BoardHead|BoardTail],
    countVisible(N, BoardHead, COUNT, 0),
    write('count = '),
    write(COUNT),
%    write('b'),
    Head = [CurrCount|RestCount], 
    write('c'),
    CurrCount =:= COUNT.

countVis(Row, Count) :-
    length(Row, N),
    %Count #=< N,
    countVisible(N, Row, Count, 0)
    .

countVisible(N, [], Count, _).
countVisible(N, [Head|Tail], Count, CurrMax) :-    
    Head #> CurrMax,
    NewCount #= Count + 1,
    countVisible(N, Tail, NewCount, Head)
    ;
    Head #=< CurrMax,
    countVisible(N, Tail, Count, CurrMax)
    %,Count #=< Nxs
    .

countV(List, Count, CurrMax) :-
    length(List, 0)
    ;
    List = [Head|Tail],
    (
    Head #> CurrMax, %new largest val
    NewCount is Count + 1,
    countV(Tail, NewCount, Head)
    ;
    Head #=< CurrMax,
    countV(Tail, Count, CurrMax)
    ).

valid_C(N, C) :-
    C = counts(Cnt),
    length(Cnt, 0)
    ;
    C = counts(Head,Tail), %maybe move unifying into func params
    length(Head, N),
    fd_domain(Head, 1, N),
    valid_C(N, counts(Tail)).
*/


    

/*    
ownDiff(N, List):-
    length(StandardList, N), %standardList is [1-N]; ie [1,2,3] for N = 3
    member(


ownDomain(_, []).
ownDomain(N, [Head|Tail]) :-
    Head >= 1, 
    Head =< N, 
    ownDomain(N, Tail).
 
	
ownDomain(N, T) :-         %all elemnts of T are >= 1 and <= N
    maplist(>=(N), T),
    maplist(=<(1), T). 

ownDiff([]).
ownDiff([Head|Tail]) :- member(Head, Tail), !, fail.
ownDiff([_|T]) :- ownDiff(T).

ownDomain(N,T) :-      %gurantees fd_domain and fd_all_different
    prod_StandardList(N, Standard),
    permutation(Standard, T).
    N = 1,
    onePerm(T);
    N = 2,
    twoPerm(T);
    N = 3, 
    threePerm(T);
    N = 4,
    fourPerm(T);
    N = 5, 
    fivePerm(T).

onePerm(T) :-
    permutation([1], T).
twoPerm(T) :-
    permutation([1,2], T).
threePerm(T) :- 
    permutation([1,2,3], T).
fourPerm(T) :-
    permutation([1,2,3,4], T).
fivePerm(T) :- 
    permutation([1,2,3,4,5], T).


maplist(pcC_helper(N), C).
 
oneMember(C) :-
    member(C, [1]).
twoMember(C) :-
    member(C, [1,2]).
threeMember(C) :- 
    member(C, [1,2,3]).
fourMember(C) :-
    member(C, [1,2,3,4]).
fiveMember(C) :- 
    member(C, [1,2,3,4,5]).

pcC_helper(N, C) :-
    N = 1,
    oneMember(C);
    N = 2,
    twoMember(C);
    N = 3, 
    threeMember(C);
    N = 4,
    fourMember(C);
    N = 5, 
    fiveMember(C).


ownDomain(N,T) :-      %gurantees fd_domain and fd_all_different
    prod_StandardList(N, Standard),
    permutation(Standard, T).

	

plain_tower: second half    
    maplist(permutation(Standard), T),
    transpose(T, T_transpose),
    maplist(permutation(Standard), T_transpose),
    maplist(reverse, T, T_reverse),
    maplist(reverse, T_transpose, T_tran_rev),

    maplist(revMem(Standard), Left),     
    plain_checkVis(T, Left),

    maplist(revMem(Standard), Top),
    plain_checkVis(T_transpose, Top),

    maplist(revMem(Standard), Bot),    
    plain_checkVis(T_tran_rev, Bot),

    
    maplist(revMem(Standard), Right),
    plain_checkVis(T_reverse, Right)
    .

plain_check_C(N, C, Standard) :-  %all members of C 
    length(C, N),
    maplist(revMem(Standard), C).

revMem(List, Element) :-
    member(Element, List).

plain_checkVis([], _).
plain_checkVis([Row|Rest], [RowCount|RestCounts]) :-
    plain_checkCount(Row, RowCount, 0),
    !,
    plain_checkVis(Rest, RestCounts).


plain_checkCount([Last|[]],1,CurrMax) :-
    Last > CurrMax.
plain_checkCount([Last|[]],0,CurrMax) :-
    Last < CurrMax.



*/


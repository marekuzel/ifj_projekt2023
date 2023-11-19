[prog] → [stmt]*

[stmt] →
   | let [id] [stmt_assign]
   | var [id] [stmt_assign]
   | if [expr] { [stmt_seq] } [stmt_else]
   | if let [id] = [expr] [stmt_else]
   | while [expr] { [stmt_seq] }
   | [def_func]
   | [callFunction]
   | return [expr]

[stmt_assign]-> 
   | = [expr]
   | : [type] = [expr]
   | : [type]
   
[stmt_else] →
   | else { [stmt_seq] }
   | // empty

[stmt_seq] → [stmt]*

[callFunction] → [functId] ([parameters])

[def_func] →
   | func [funcId] ([parameters]) [func_ret] { [stmt_seq] }
   

[parameters] →
   | [id] : [type] [parameters_seq]*
   | // empty

[parameters_seq] →
   | , [id] : [type] [parameters_seq]
   | // empty

[func_ret] →
   | -> [type]
   | // empty

[expr_seq] → [expr]*

[expr] →
   | ( [expr] )
   | [expr] + [expr]
   | [expr] * [expr]
   | [expr] == [expr]
   | [expr] > [expr]
   | [expr] !
   | [expr] ?? [expr]
   | [id]
   | [literal]

[id] → *id*

[type] →
   | Int
   | String
   | Double
   | nil
   | [type]?
   | [type]!

[literal] → *literal*
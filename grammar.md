[prog] → [stmt_seq]

[stmt] →
   | let [id] [stmt_assign]
   | var [id] [stmt_assign]
   | if [expr] { [stmt_seqFunc] } [stmt_else]
   | if let [id] = [expr] [stmt_else]
   | while [expr] { [stmt_seqFunc] }
   | [def_func]
   | [callFunction]
   | [id] [stmt_assign]

[stmt_assign]-> 
   | = [expr]
   | : [type] = [expr]
   | : [type]
   | = [callFunction]
   
[stmt_else] →
   | else { [stmt_seqFunc] }
   | // empty

[stmt_seq] → [stmt]*

[callFunction] → [functId] ([parameters])

[def_func] →
   | func [funcId] ([parameters]) [func_ret]
   

[parameters] →
   | [name] [id] : [type] ( [parameters_seq]* )
   | // empty

[parameters_seq] →
   | , [name] [id] : [type] [parameters_seq]
   | // empty

[name] → *name*

[func_ret] →
   | -> [type] { [stmt_seqFunc] }
   | { [stmt_seqFunc] }

[stmt_seqFunc] → 
   | [stmt]*
   | return [expr]
   | return

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
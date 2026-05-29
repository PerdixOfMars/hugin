# C++ testing DSL is the Hugin MVP product

Hugin's MVP product is a high-level C++ testing DSL for executable specifications of Munin UI behavior, not a thin wrapper around Munin primitives. The DSL must provide readable sessions, selectors, nodes, actions, assertions, waits, and diagnostic failures, while an underlying engine and Munin adapter handle snapshot traversal, predicate evaluation, action dispatch, and diagnostic assembly.

This deliberately keeps textual DSLs, standalone script runners, and remote-control sessions out of the MVP. The important trade-off is that Hugin will prioritize expressive test vocabulary and failure clarity before broad transport or scripting ambitions, because the primary user experience of a testing framework is the quality of its test code and failures.

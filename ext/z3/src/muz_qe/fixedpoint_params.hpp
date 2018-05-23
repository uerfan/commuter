// Automatically generated file
#ifndef __FIXEDPOINT_PARAMS_HPP_
#define __FIXEDPOINT_PARAMS_HPP_
#include"params.h"
#include"gparams.h"
struct fixedpoint_params {
  params_ref const & p;
  params_ref g;
  fixedpoint_params(params_ref const & _p = params_ref::get_empty()):
     p(_p), g(gparams::get_module("fixedpoint")) {}
  static void collect_param_descrs(param_descrs & d) {
    d.insert("timeout", CPK_UINT, "set timeout", "4294967295");
    d.insert("engine", CPK_SYMBOL, "Select: auto-config, datalog, pdr, bmc", "auto-config");
    d.insert("default_table", CPK_SYMBOL, "default table implementation: sparse, hashtable, bitvector, interval", "sparse");
    d.insert("default_relation", CPK_SYMBOL, "default relation implementation: external_relation, pentagon", "pentagon");
    d.insert("generate_explanations", CPK_BOOL, "(DATALOG) produce explanations for produced facts when using the datalog engine", "false");
    d.insert("use_map_names", CPK_BOOL, "(DATALOG) use names from map files when displaying tuples", "true");
    d.insert("bit_blast", CPK_BOOL, "(PDR) bit-blast bit-vectors", "false");
    d.insert("explanations_on_relation_level", CPK_BOOL, "(DATALOG) if true, explanations are generated as history of each relation, rather than per fact (generate_explanations must be set to true for this option to have any effect)", "false");
    d.insert("magic_sets_for_queries", CPK_BOOL, "(DATALOG) magic set transformation will be used for queries", "false");
    d.insert("unbound_compressor", CPK_BOOL, "auxiliary relations will be introduced to avoid unbound variables in rule heads", "true");
    d.insert("similarity_compressor", CPK_BOOL, "(DATALOG) rules that differ only in values of constants will be merged into a single rule", "true");
    d.insert("similarity_compressor_threshold", CPK_UINT, "(DATALOG) if similarity_compressor is on, this value determines how many similar rules there must be in order for them to be merged", "11");
    d.insert("all_or_nothing_deltas", CPK_BOOL, "(DATALOG) compile rules so that it is enough for the delta relation in union and widening operations to determine only whether the updated relation was modified or not", "false");
    d.insert("compile_with_widening", CPK_BOOL, "(DATALOG) widening will be used to compile recursive rules", "false");
    d.insert("eager_emptiness_checking", CPK_BOOL, "(DATALOG) emptiness of affected relations will be checked after each instruction, so that we may ommit unnecessary instructions", "true");
    d.insert("default_table_checked", CPK_BOOL, "if true, the detault table will be default_table inside a wrapper that checks that its results are the same as of default_table_checker table", "false");
    d.insert("default_table_checker", CPK_SYMBOL, "see default_table_checked", "null");
    d.insert("initial_restart_timeout", CPK_UINT, "length of saturation run before the first restart (in ms), zero means no restarts", "0");
    d.insert("output_profile", CPK_BOOL, "determines whether profile informations should be output when outputting Datalog rules or instructions", "false");
    d.insert("inline_linear", CPK_BOOL, "try linear inlining method", "true");
    d.insert("inline_eager", CPK_BOOL, "try eager inlining of rules", "true");
    d.insert("inline_linear_branch", CPK_BOOL, "try linear inlining method with potential expansion", "false");
    d.insert("fix_unbound_vars", CPK_BOOL, "fix unbound variables in tail", "false");
    d.insert("output_tuples", CPK_BOOL, "determines whether tuples for output predicates should be output", "true");
    d.insert("print_with_fixedpoint_extensions", CPK_BOOL, "use SMT-LIB2 fixedpoint extensions, instead of pure SMT2, when printing rules", "true");
    d.insert("print_low_level_smt2", CPK_BOOL, "use (faster) low-level SMT2 printer (the printer is scalable but the result may not be as readable)", "false");
    d.insert("print_with_variable_declarations", CPK_BOOL, "use variable declarations when displaying rules (instead of attempting to use original names)", "true");
    d.insert("bfs_model_search", CPK_BOOL, "PDR: use BFS strategy for expanding model search", "true");
    d.insert("use_farkas", CPK_BOOL, "PDR: use lemma generator based on Farkas (for linear real arithmetic)", "true");
    d.insert("generate_proof_trace", CPK_BOOL, "PDR: trace for 'sat' answer as proof object", "false");
    d.insert("flexible_trace", CPK_BOOL, "PDR: allow PDR generate long counter-examples by extending candidate trace within search area", "false");
    d.insert("unfold_rules", CPK_UINT, "PDR: unfold rules statically using iterative squarring", "0");
    d.insert("use_model_generalizer", CPK_BOOL, "PDR: use model for backwards propagation (instead of symbolic simulation)", "false");
    d.insert("validate_result", CPK_BOOL, "PDR validate result (by proof checking or model checking)", "false");
    d.insert("simplify_formulas_pre", CPK_BOOL, "PDR: simplify derived formulas before inductive propagation", "false");
    d.insert("simplify_formulas_post", CPK_BOOL, "PDR: simplify derived formulas after inductive propagation", "false");
    d.insert("slice", CPK_BOOL, "PDR: simplify clause set using slicing", "true");
    d.insert("karr", CPK_BOOL, "Add linear invariants to clauses using Karr's method", "false");
    d.insert("quantify_arrays", CPK_BOOL, "create quantified Horn clauses from clauses with arrays", "false");
    d.insert("instantiate_quantifiers", CPK_BOOL, "instantiate quantified Horn clauses using E-matching heuristic", "false");
    d.insert("coalesce_rules", CPK_BOOL, "BMC: coalesce rules", "false");
    d.insert("use_multicore_generalizer", CPK_BOOL, "PDR: extract multiple cores for blocking states", "false");
    d.insert("use_inductive_generalizer", CPK_BOOL, "PDR: generalize lemmas using induction strengthening", "true");
    d.insert("use_arith_inductive_generalizer", CPK_BOOL, "PDR: generalize lemmas using arithmetic heuristics for induction strengthening", "false");
    d.insert("use_convex_hull_generalizer", CPK_BOOL, "PDR: generalize using convex hulls of lemmas", "false");
    d.insert("cache_mode", CPK_UINT, "PDR: use no (0), symbolic (1) or explicit cache (2) for model search", "0");
    d.insert("inductive_reachability_check", CPK_BOOL, "PDR: assume negation of the cube on the previous level when checking for reachability (not only during cube weakening)", "false");
    d.insert("max_num_contexts", CPK_UINT, "PDR: maximal number of contexts to create", "500");
    d.insert("try_minimize_core", CPK_BOOL, "PDR: try to reduce core size (before inductive minimization)", "false");
    d.insert("profile_timeout_milliseconds", CPK_UINT, "instructions and rules that took less than the threshold will not be printed when printed the instruction/rule list", "0");
    d.insert("dbg_fpr_nonempty_relation_signature", CPK_BOOL, "if true, finite_product_relation will attempt to avoid creating inner relation with empty signature by putting in half of the table columns, if it would have been empty otherwise", "false");
    d.insert("print_answer", CPK_BOOL, "print answer instance(s) to query", "false");
    d.insert("print_certificate", CPK_BOOL, "print certificate for reachability or non-reachability", "false");
    d.insert("print_statistics", CPK_BOOL, "print statistics", "false");
    d.insert("use_utvpi", CPK_BOOL, "PDR: Enable UTVPI strategy", "true");
    d.insert("tab_selection", CPK_SYMBOL, "selection method for tabular strategy: weight (default), first, var-use", "weight");
    d.insert("dump_aig", CPK_SYMBOL, "Dump clauses in AIG text format (AAG) to the given file name", "");
  }
  /*
     REG_MODULE_PARAMS('fixedpoint', 'fixedpoint_params::collect_param_descrs')
     REG_MODULE_DESCRIPTION('fixedpoint', 'fixedpoint parameters')
  */
  unsigned timeout() const { return p.get_uint("timeout", g, 4294967295u); }
  symbol engine() const { return p.get_sym("engine", g, symbol("auto-config")); }
  symbol default_table() const { return p.get_sym("default_table", g, symbol("sparse")); }
  symbol default_relation() const { return p.get_sym("default_relation", g, symbol("pentagon")); }
  bool generate_explanations() const { return p.get_bool("generate_explanations", g, false); }
  bool use_map_names() const { return p.get_bool("use_map_names", g, true); }
  bool bit_blast() const { return p.get_bool("bit_blast", g, false); }
  bool explanations_on_relation_level() const { return p.get_bool("explanations_on_relation_level", g, false); }
  bool magic_sets_for_queries() const { return p.get_bool("magic_sets_for_queries", g, false); }
  bool unbound_compressor() const { return p.get_bool("unbound_compressor", g, true); }
  bool similarity_compressor() const { return p.get_bool("similarity_compressor", g, true); }
  unsigned similarity_compressor_threshold() const { return p.get_uint("similarity_compressor_threshold", g, 11u); }
  bool all_or_nothing_deltas() const { return p.get_bool("all_or_nothing_deltas", g, false); }
  bool compile_with_widening() const { return p.get_bool("compile_with_widening", g, false); }
  bool eager_emptiness_checking() const { return p.get_bool("eager_emptiness_checking", g, true); }
  bool default_table_checked() const { return p.get_bool("default_table_checked", g, false); }
  symbol default_table_checker() const { return p.get_sym("default_table_checker", g, symbol("null")); }
  unsigned initial_restart_timeout() const { return p.get_uint("initial_restart_timeout", g, 0u); }
  bool output_profile() const { return p.get_bool("output_profile", g, false); }
  bool inline_linear() const { return p.get_bool("inline_linear", g, true); }
  bool inline_eager() const { return p.get_bool("inline_eager", g, true); }
  bool inline_linear_branch() const { return p.get_bool("inline_linear_branch", g, false); }
  bool fix_unbound_vars() const { return p.get_bool("fix_unbound_vars", g, false); }
  bool output_tuples() const { return p.get_bool("output_tuples", g, true); }
  bool print_with_fixedpoint_extensions() const { return p.get_bool("print_with_fixedpoint_extensions", g, true); }
  bool print_low_level_smt2() const { return p.get_bool("print_low_level_smt2", g, false); }
  bool print_with_variable_declarations() const { return p.get_bool("print_with_variable_declarations", g, true); }
  bool bfs_model_search() const { return p.get_bool("bfs_model_search", g, true); }
  bool use_farkas() const { return p.get_bool("use_farkas", g, true); }
  bool generate_proof_trace() const { return p.get_bool("generate_proof_trace", g, false); }
  bool flexible_trace() const { return p.get_bool("flexible_trace", g, false); }
  unsigned unfold_rules() const { return p.get_uint("unfold_rules", g, 0u); }
  bool use_model_generalizer() const { return p.get_bool("use_model_generalizer", g, false); }
  bool validate_result() const { return p.get_bool("validate_result", g, false); }
  bool simplify_formulas_pre() const { return p.get_bool("simplify_formulas_pre", g, false); }
  bool simplify_formulas_post() const { return p.get_bool("simplify_formulas_post", g, false); }
  bool slice() const { return p.get_bool("slice", g, true); }
  bool karr() const { return p.get_bool("karr", g, false); }
  bool quantify_arrays() const { return p.get_bool("quantify_arrays", g, false); }
  bool instantiate_quantifiers() const { return p.get_bool("instantiate_quantifiers", g, false); }
  bool coalesce_rules() const { return p.get_bool("coalesce_rules", g, false); }
  bool use_multicore_generalizer() const { return p.get_bool("use_multicore_generalizer", g, false); }
  bool use_inductive_generalizer() const { return p.get_bool("use_inductive_generalizer", g, true); }
  bool use_arith_inductive_generalizer() const { return p.get_bool("use_arith_inductive_generalizer", g, false); }
  bool use_convex_hull_generalizer() const { return p.get_bool("use_convex_hull_generalizer", g, false); }
  unsigned cache_mode() const { return p.get_uint("cache_mode", g, 0u); }
  bool inductive_reachability_check() const { return p.get_bool("inductive_reachability_check", g, false); }
  unsigned max_num_contexts() const { return p.get_uint("max_num_contexts", g, 500u); }
  bool try_minimize_core() const { return p.get_bool("try_minimize_core", g, false); }
  unsigned profile_timeout_milliseconds() const { return p.get_uint("profile_timeout_milliseconds", g, 0u); }
  bool dbg_fpr_nonempty_relation_signature() const { return p.get_bool("dbg_fpr_nonempty_relation_signature", g, false); }
  bool print_answer() const { return p.get_bool("print_answer", g, false); }
  bool print_certificate() const { return p.get_bool("print_certificate", g, false); }
  bool print_statistics() const { return p.get_bool("print_statistics", g, false); }
  bool use_utvpi() const { return p.get_bool("use_utvpi", g, true); }
  symbol tab_selection() const { return p.get_sym("tab_selection", g, symbol("weight")); }
  symbol dump_aig() const { return p.get_sym("dump_aig", g, symbol("")); }
};
#endif

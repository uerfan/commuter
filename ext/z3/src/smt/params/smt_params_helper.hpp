// Automatically generated file
#ifndef __SMT_PARAMS_HELPER_HPP_
#define __SMT_PARAMS_HELPER_HPP_
#include"params.h"
#include"gparams.h"
struct smt_params_helper {
  params_ref const & p;
  params_ref g;
  smt_params_helper(params_ref const & _p = params_ref::get_empty()):
     p(_p), g(gparams::get_module("smt")) {}
  static void collect_param_descrs(param_descrs & d) {
    d.insert("auto_config", CPK_BOOL, "automatically configure solver", "true");
    d.insert("random_seed", CPK_UINT, "random seed for the smt solver", "0");
    d.insert("relevancy", CPK_UINT, "relevancy propagation heuristic: 0 - disabled, 1 - relevancy is tracked by only affects quantifier instantiation, 2 - relevancy is tracked, and an atom is only asserted if it is relevant", "2");
    d.insert("macro_finder", CPK_BOOL, "try to find universally quantified formulas that can be viewed as macros", "false");
    d.insert("ematching", CPK_BOOL, "E-Matching based quantifier instantiation", "true");
    d.insert("phase_selection", CPK_UINT, "phase selection heuristic: 0 - always false, 1 - always true, 2 - phase caching, 3 - phase caching conservative, 4 - phase caching conservative 2, 5 - random, 6 - number of occurrences", "3");
    d.insert("restart_strategy", CPK_UINT, "0 - geometric, 1 - inner-outer-geometric, 2 - luby, 3 - fixed, 4 - arithmetic", "1");
    d.insert("restart_factor", CPK_DOUBLE, "when using geometric (or inner-outer-geometric) progression of restarts, it specifies the constant used to multiply the currect restart threshold", "1.1");
    d.insert("case_split", CPK_UINT, "0 - case split based on variable activity, 1 - similar to 0, but delay case splits created during the search, 2 - similar to 0, but cache the relevancy, 3 - case split based on relevancy (structural splitting), 4 - case split on relevancy and activity, 5 - case split on relevancy and current goal", "1");
    d.insert("delay_units", CPK_BOOL, "if true then z3 will not restart when a unit clause is learned", "false");
    d.insert("delay_units_threshold", CPK_UINT, "maximum number of learned unit clauses before restarting, ingored if delay_units is false", "32");
    d.insert("pull_nested_quantifiers", CPK_BOOL, "pull nested quantifiers", "false");
    d.insert("soft_timeout", CPK_UINT, "soft timeout (0 means no timeout)", "0");
    d.insert("mbqi", CPK_BOOL, "model based quantifier instantiation (MBQI)", "true");
    d.insert("mbqi.max_cexs", CPK_UINT, "initial maximal number of counterexamples used in MBQI, each counterexample generates a quantifier instantiation", "1");
    d.insert("mbqi.max_cexs_incr", CPK_UINT, "increment for MBQI_MAX_CEXS, the increment is performed after each round of MBQI", "0");
    d.insert("mbqi.max_iterations", CPK_UINT, "maximum number of rounds of MBQI", "1000");
    d.insert("mbqi.trace", CPK_BOOL, "generate tracing messages for Model Based Quantifier Instantiation (MBQI). It will display a message before every round of MBQI, and the quantifiers that were not satisfied", "false");
    d.insert("mbqi.force_template", CPK_UINT, "some quantifiers can be used as templates for building interpretations for functions. Z3 uses heuristics to decide whether a quantifier will be used as a template or not. Quantifiers with weight >= mbqi.force_template are forced to be used as a template", "10");
    d.insert("qi.profile", CPK_BOOL, "profile quantifier instantiation", "false");
    d.insert("qi.profile_freq", CPK_UINT, "how frequent results are reported by qi.profile", "4294967295");
    d.insert("qi.max_instances", CPK_UINT, "maximum number of quantifier instantiations", "4294967295");
    d.insert("qi.eager_threshold", CPK_DOUBLE, "threshold for eager quantifier instantiation", "10.0");
    d.insert("qi.lazy_threshold", CPK_DOUBLE, "threshold for lazy quantifier instantiation", "20.0");
    d.insert("qi.cost", CPK_STRING, "expression specifying what is the cost of a given quantifier instantiation", "(+ weight generation)");
    d.insert("qi.max_multi_patterns", CPK_UINT, "specify the number of extra multi patterns", "0");
    d.insert("bv.reflect", CPK_BOOL, "create enode for every bit-vector term", "true");
    d.insert("bv.enable_int2bv", CPK_BOOL, "enable support for int2bv and bv2int operators", "false");
    d.insert("arith.random_initial_value", CPK_BOOL, "use random initial values in the simplex-based procedure for linear arithmetic", "false");
    d.insert("arith.solver", CPK_UINT, "arithmetic solver: 0 - no solver, 1 - bellman-ford based solver (diff. logic only), 2 - simplex based solver, 3 - floyd-warshall based solver (diff. logic only) and no theory combination", "2");
    d.insert("arith.nl", CPK_BOOL, "(incomplete) nonlinear arithmetic support based on Groebner basis and interval propagation", "true");
    d.insert("arith.nl.gb", CPK_BOOL, "groebner Basis computation, this option is ignored when arith.nl=false", "true");
    d.insert("arith.nl.branching", CPK_BOOL, "branching on integer variables in non linear clusters", "true");
    d.insert("arith.nl.rounds", CPK_UINT, "threshold for number of (nested) final checks for non linear arithmetic", "1024");
    d.insert("arith.euclidean_solver", CPK_BOOL, "eucliean solver for linear integer arithmetic", "false");
    d.insert("arith.propagate_eqs", CPK_BOOL, "propagate (cheap) equalities", "true");
    d.insert("arith.propagation_mode", CPK_UINT, "0 - no propagation, 1 - propagate existing literals, 2 - refine bounds", "2");
    d.insert("arith.branch_cut_ratio", CPK_UINT, "branch/cut ratio for linear integer arithmetic", "2");
    d.insert("arith.int_eq_branch", CPK_BOOL, "branching using derived integer equations", "false");
    d.insert("arith.ignore_int", CPK_BOOL, "treat integer variables as real", "false");
  }
  /*
     REG_MODULE_PARAMS('smt', 'smt_params_helper::collect_param_descrs')
     REG_MODULE_DESCRIPTION('smt', 'smt solver based on lazy smt')
  */
  bool auto_config() const { return p.get_bool("auto_config", g, true); }
  unsigned random_seed() const { return p.get_uint("random_seed", g, 0u); }
  unsigned relevancy() const { return p.get_uint("relevancy", g, 2u); }
  bool macro_finder() const { return p.get_bool("macro_finder", g, false); }
  bool ematching() const { return p.get_bool("ematching", g, true); }
  unsigned phase_selection() const { return p.get_uint("phase_selection", g, 3u); }
  unsigned restart_strategy() const { return p.get_uint("restart_strategy", g, 1u); }
  double restart_factor() const { return p.get_double("restart_factor", g, 1.1); }
  unsigned case_split() const { return p.get_uint("case_split", g, 1u); }
  bool delay_units() const { return p.get_bool("delay_units", g, false); }
  unsigned delay_units_threshold() const { return p.get_uint("delay_units_threshold", g, 32u); }
  bool pull_nested_quantifiers() const { return p.get_bool("pull_nested_quantifiers", g, false); }
  unsigned soft_timeout() const { return p.get_uint("soft_timeout", g, 0u); }
  bool mbqi() const { return p.get_bool("mbqi", g, true); }
  unsigned mbqi_max_cexs() const { return p.get_uint("mbqi.max_cexs", g, 1u); }
  unsigned mbqi_max_cexs_incr() const { return p.get_uint("mbqi.max_cexs_incr", g, 0u); }
  unsigned mbqi_max_iterations() const { return p.get_uint("mbqi.max_iterations", g, 1000u); }
  bool mbqi_trace() const { return p.get_bool("mbqi.trace", g, false); }
  unsigned mbqi_force_template() const { return p.get_uint("mbqi.force_template", g, 10u); }
  bool qi_profile() const { return p.get_bool("qi.profile", g, false); }
  unsigned qi_profile_freq() const { return p.get_uint("qi.profile_freq", g, 4294967295u); }
  unsigned qi_max_instances() const { return p.get_uint("qi.max_instances", g, 4294967295u); }
  double qi_eager_threshold() const { return p.get_double("qi.eager_threshold", g, 10.0); }
  double qi_lazy_threshold() const { return p.get_double("qi.lazy_threshold", g, 20.0); }
  char const * qi_cost() const { return p.get_str("qi.cost", g, "(+ weight generation)"); }
  unsigned qi_max_multi_patterns() const { return p.get_uint("qi.max_multi_patterns", g, 0u); }
  bool bv_reflect() const { return p.get_bool("bv.reflect", g, true); }
  bool bv_enable_int2bv() const { return p.get_bool("bv.enable_int2bv", g, false); }
  bool arith_random_initial_value() const { return p.get_bool("arith.random_initial_value", g, false); }
  unsigned arith_solver() const { return p.get_uint("arith.solver", g, 2u); }
  bool arith_nl() const { return p.get_bool("arith.nl", g, true); }
  bool arith_nl_gb() const { return p.get_bool("arith.nl.gb", g, true); }
  bool arith_nl_branching() const { return p.get_bool("arith.nl.branching", g, true); }
  unsigned arith_nl_rounds() const { return p.get_uint("arith.nl.rounds", g, 1024u); }
  bool arith_euclidean_solver() const { return p.get_bool("arith.euclidean_solver", g, false); }
  bool arith_propagate_eqs() const { return p.get_bool("arith.propagate_eqs", g, true); }
  unsigned arith_propagation_mode() const { return p.get_uint("arith.propagation_mode", g, 2u); }
  unsigned arith_branch_cut_ratio() const { return p.get_uint("arith.branch_cut_ratio", g, 2u); }
  bool arith_int_eq_branch() const { return p.get_bool("arith.int_eq_branch", g, false); }
  bool arith_ignore_int() const { return p.get_bool("arith.ignore_int", g, false); }
};
#endif

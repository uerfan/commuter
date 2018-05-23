// Automatically generated file
#ifndef __BV_REWRITER_PARAMS_HPP_
#define __BV_REWRITER_PARAMS_HPP_
#include"params.h"
#include"gparams.h"
struct bv_rewriter_params {
  params_ref const & p;
  params_ref g;
  bv_rewriter_params(params_ref const & _p = params_ref::get_empty()):
     p(_p), g(gparams::get_module("rewriter")) {}
  static void collect_param_descrs(param_descrs & d) {
    d.insert("udiv2mul", CPK_BOOL, "convert constant udiv to mul", "false");
    d.insert("split_concat_eq", CPK_BOOL, "split equalities of the form (= (concat t1 t2) t3)", "false");
    d.insert("bit2bool", CPK_BOOL, "try to convert bit-vector terms of size 1 into Boolean terms", "true");
    d.insert("blast_eq_value", CPK_BOOL, "blast (some) Bit-vector equalities into bits", "false");
    d.insert("elim_sign_ext", CPK_BOOL, "expand sign-ext operator using concat and extract", "true");
    d.insert("hi_div0", CPK_BOOL, "use the 'hardware interpretation' for division by zero (for bit-vector terms)", "true");
    d.insert("mul2concat", CPK_BOOL, "replace multiplication by a power of two into a concatenation", "false");
    d.insert("bvnot2arith", CPK_BOOL, "replace (bvnot x) with (bvsub -1 x)", "false");
    d.insert("bv_sort_ac", CPK_BOOL, "sort the arguments of all AC operators", "false");
  }
  /*
     REG_MODULE_PARAMS('rewriter', 'bv_rewriter_params::collect_param_descrs')
  */
  bool udiv2mul() const { return p.get_bool("udiv2mul", g, false); }
  bool split_concat_eq() const { return p.get_bool("split_concat_eq", g, false); }
  bool bit2bool() const { return p.get_bool("bit2bool", g, true); }
  bool blast_eq_value() const { return p.get_bool("blast_eq_value", g, false); }
  bool elim_sign_ext() const { return p.get_bool("elim_sign_ext", g, true); }
  bool hi_div0() const { return p.get_bool("hi_div0", g, true); }
  bool mul2concat() const { return p.get_bool("mul2concat", g, false); }
  bool bvnot2arith() const { return p.get_bool("bvnot2arith", g, false); }
  bool bv_sort_ac() const { return p.get_bool("bv_sort_ac", g, false); }
};
#endif

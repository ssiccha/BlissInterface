/*
 * BlissInterface: Low level interface to the bliss graph automorphism tool
 */

#include "bliss-0.73/graph.hh" // for bliss_digraphs_release, . . .
#include "src/compiled.h"      /* GAP headers */

/*
 * copied from GAP package Digraphs 0.15.2 "digraphs_hook_function()"
 */

void blissinterface_hook_function(void *user_param_v, unsigned int N,
                                  const unsigned int *aut) {
  UInt4 *ptr;
  Obj p, gens, user_param;
  UInt i, n;

  // we need this in C++ to avoid "invalid conversion" error
  user_param = static_cast<Obj>(user_param_v);
  n = INT_INTOBJ(ELM_PLIST(user_param, 2)); // the degree
  p = NEW_PERM4(n);
  ptr = ADDR_PERM4(p);

  for (i = 0; i < n; i++) {
    ptr[i] = aut[i];
  }

  gens = ELM_PLIST(user_param, 1);
  AssPlist(gens, LEN_PLIST(gens) + 1, p);
}

/*
 * Returns: a GAP object list [gens,cl], where <gens> are generators
 * of the aut group of the bipartite digraph, associated to the Steiner
 * system, and <cl> is a canonical labeling of the digraph.
 *
 * Based on "FuncDIGRAPH_AUTOMORPHISMS" of the GAP package Digraphs 0.15.2
 */

static Obj blissinterface_autgr_canlab(bliss::AbstractGraph *graph) {
  Obj autos, p, n;
  UInt4 *ptr;
  const unsigned int *canon;
  Int i;
  bliss::Stats stats;

  autos = NEW_PLIST(T_PLIST, 2);
  n = INTOBJ_INT(graph->get_nof_vertices());

  SET_ELM_PLIST(autos, 1, NEW_PLIST(T_PLIST, 0)); // perms of the vertices
  CHANGED_BAG(autos);
  SET_ELM_PLIST(autos, 2, n);
  SET_LEN_PLIST(autos, 2);

  canon = graph->canonical_form(stats, blissinterface_hook_function, autos);

  p = NEW_PERM4(INT_INTOBJ(n));
  ptr = ADDR_PERM4(p);

  for (i = 0; i < INT_INTOBJ(n); i++) {
    ptr[i] = canon[i];
  }
  SET_ELM_PLIST(autos, 2, p);
  CHANGED_BAG(autos);

  if (LEN_PLIST(ELM_PLIST(autos, 1)) != 0) {
    SortDensePlist(ELM_PLIST(autos, 1));
    RemoveDupsDensePlist(ELM_PLIST(autos, 1));
  }

  return autos;
}

Obj FuncBlissSteinerCanonicalLabeling(Obj self, Obj mat, Obj m, Obj n) {
  bliss::AbstractGraph *g;
  UInt k, mm, nn;
  UInt i, j;
  mm = INT_INTOBJ(m);
  nn = INT_INTOBJ(n);
  g = new bliss::Digraph(nn + mm);
  k = 0;
  for (i = 1; i <= mm; i++) {
    for (j = 1; j <= nn; j++) {
      if (TEST_BIT_BLIST(ELM_LIST(mat, i), j)) {
        g->add_edge(i - 1, mm + j - 1);
      }
    }
  }
  return blissinterface_autgr_canlab(g);
}

/***************************************************************************/

// Table of functions to export

/*
 * "GVarFuncs" is nnt workin in C++, due to
 * "invalid conversion from ‘void*’ to ‘Obj‘" error
/*

// static StructGVarFunc GVarFuncs [] = {
//    GVAR_FUNC(TestCommand, 0, ""),
//    GVAR_FUNC(TestCommandWithParams, 2, "param, param2"),
//
//    { 0 } /* Finish with an empty entry */
//};

typedef Obj (*GVarFuncTypeDef)(/*arguments*/);

#define GVAR_FUNC_TABLE_ENTRY(name, nparam, params)                            \
  { #name, nparam, params, (GVarFuncTypeDef)Func##name, __FILE__ ":" #name }

// Table of functions to export
static StructGVarFunc GVarFuncs[] = {
    GVAR_FUNC_TABLE_ENTRY(BlissSteinerCanonicalLabeling, 3, "mat, m, n"),

    {0} /* Finish with an empty entry */

};

/****************************************************************************
**
*F  InitKernel( <module> ) . . . . . . . .  initialise kernel data structures
*/
static Int InitKernel(StructInitInfo *module) {
  /* init filters and functions */
  InitHdlrFuncsFromTable(GVarFuncs);

  /* return success */
  return 0;
}

/****************************************************************************
**
*F  InitLibrary( <module> ) . . . . . . .  initialise library data structures
*/
static Int InitLibrary(StructInitInfo *module) {
  /* init filters and functions */
  InitGVarFuncsFromTable(GVarFuncs);

  /* return success */
  return 0;
}

/****************************************************************************
**
*F  Init__Dynamic() . . . . . . . . . . . . . . . . . table of init functions
*/
static StructInitInfo module = {
    /* type        = */ MODULE_DYNAMIC,
    /* name        = */ "BlissInterface",
    /* revision_c  = */ 0,
    /* revision_h  = */ 0,
    /* version     = */ 0,
    /* crc         = */ 0,
    /* initKernel  = */ InitKernel,
    /* initLibrary = */ InitLibrary,
    /* checkInit   = */ 0,
    /* preSave     = */ 0,
    /* postSave    = */ 0,
    /* postRestore = */ 0,
    /* moduleStateSize      = */ 0,
    /* moduleStateOffsetPtr = */ 0,
    /* initModuleState      = */ 0,
    /* destroyModuleState   = */ 0,
};

extern "C" StructInitInfo *Init__Dynamic(void) { return &module; }

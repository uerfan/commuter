import simsym
import simtest
import z3
import z3util
import collections

class TestGenerator(simtest.ExecutionMonitorBase):
    """Base class for test case generators.

    This extends the basic execution monitor with methods that trace
    the generation of test models for each code path.

    The methods of this class will be invoked as symbolic execution
    and model enumeration proceed in the following order:
      (begin_call_set (begin_path on_model+ end_path)* end_call_set)* finish
    Subclasses may override any of these methods, but should always
    call the superclass method.
    """

    def __init__(self, test_file_name):
        """Initialize the test generator.

        test_file_name is the file name for test output.  The subclass
        may derive other file names from this.
        """
        super(TestGenerator, self).__init__()
        self.__result = self.__model = None

    def get_result(self, callno):
        """Return the result for the callno'th call.

        The result must be a dictionary.  Any values that are symbolic
        will be automatically bound to the model.
        """
        res = self.__result.value.results[callno]
        return {k: v.bind(self.__model) if isinstance(v, simsym.Symbolic) else v
                for k, v in res.iteritems()}

    def get_call_args(self, callno):
        """Return the arguments struct for the callno'th call."""
        var_name = (chr(ord('a') + callno) + "." + self.callset_names[callno])
        return self.__model[var_name]

    def begin_path(self, result):
        """Handle the beginning of a code path.

        result is the simsym.SymbolicApplyResult representing this
        code path.

        The code path will have one or more models that satisfy its
        path condition.
        """
        self.__result = result

    def on_model(self, testid, model, constraint):
        """Handle a concrete assignment for the current code path.

        testid is the string uniquely identifying this test.  It is a
        valid identifier.

        model is the simsym.Model object giving the concrete
        assignment.

        constraint is the simsym.SBool giving the path condition and
        enumeration constraint that let to this model.

        This method may be called more than once for a single code
        path.  Each call's model will differ in the value of at least
        one expression that was evaluated by past calls to on_model.
        """
        self.__model = model

    def end_path(self):
        """Handle the end of a code path."""
        self.__result = None

class CodeWriter(object):
    def __init__(self, fp=None):
        self.__fp = fp
        if fp is None:
            self.__blocks = []

    def __call__(self, *blocks):
        if self.__fp:
            self.__fp.write('\n'.join(map(str, blocks)) + '\n')
            self.__fp.flush()
        else:
            self.__blocks.extend(blocks)
        return self

    def __repr__(self):
        return 'CodeWriter(%r)' % self.__fp

    def __str__(self):
        if self.__fp:
            return repr(self)
        return '\n'.join(map(str, self.__blocks))

    def indent(self, by="  "):
        indented = by + str(self).replace('\n', '\n' + by)
        return CodeWriter()(indented)

def _is_literal(z3ast):
    if z3.is_int(z3ast):
        return z3.is_int_value(z3ast)
    if z3.is_bool(z3ast):
        return z3.is_true(z3ast) or z3.is_false(z3ast)
    if z3ast.sort_kind() == z3.Z3_UNINTERPRETED_SORT:
        return z3.is_const(z3ast) and '!' in str(z3ast)
    raise NotImplementedError('Don\'t know how to literal-check %s' % z3ast)

class DynamicDict(object):
    """A dynamically-filled dictionary.

    Values for dictionary mappings are drawn from an iterable as they
    are needed.  This has special support for Z3 literals, making it
    useful for assigning concrete values to uninterpreted constants.

    A DynamicDict can be iterated over just like a regular dict, but
    doing so freezes its contents to prevent further additions.
    """

    def __init__(self, iterable_or_fn):
        """Initialize an empty DynamicDict fed by iterable_or_fn.

        iterable_or_fn must either be an iterable object over physical
        values or a function that will be called with the key and must
        return a value or raise StopIteration.
        """

        if isinstance(iterable_or_fn, collections.Iterable):
            it = iter(iterable_or_fn)
            self.__fn = lambda x: it.next()
        else:
            self.__fn = iterable_or_fn

        self.__map = {}

    def __getitem__(self, key):
       """Return the value associated with key.

       If the dictionary does not currently contain a value for key,
       one will be drawn from self's iterable.  key may be a Z3
       literal.
       """

       hkey = z3util.HashableAst(key)

       if hkey not in self.__map:
           if isinstance(key, simsym.Symbolic) and \
              not _is_literal(simsym.unwrap(key)):
               # There's nothing wrong with supporting arbitrary ASTs,
               # but in every place we use DynamicDict, this indicates
               # an easy-to-miss bug.
               raise ValueError("Key is not a literal: %r" % key)

           if self.__fn is None:
               raise ValueError("DynamicDict has been read; cannot be extended")
           try:
               self.__map[hkey] = self.__fn(key)
           except StopIteration:
               raise ValueError("Ran out of values for %r" % key)
       return self.__map[hkey]

    def keys(self):
        self.__fn = None
        return (hkey.ast for hkey in self.__map.iterkeys())
    __iter__ = keys

    def values(self):
        self.__fn = None
        return self.__map.itervalues()

    def items(self):
        self.__fn = None
        return ((hkey.ast, val) for hkey, val in self.__map.iteritems())

class Interpreter(object):
    """An interpreter for uninterpreted values.

    This is like a DynamicDict, but expects keys to be unevaluated
    expressions with uninterpreted sorts (or sorts we wish to treat as
    uninterpreted).  This interprets these expressions into physical
    values.  Keys are considered equal if they evaluate to the same
    value in the model.  This also type-checks all keys.
    """

    def __init__(self, key_type, iterable_or_fn, enumerate=True):
        """Initialize an empty Interpreter.

        key_type must be a Symbolic subclass.  iterable_or_fn must be
        as for DynamicDict.  If iterable_or_fn is a function, it will
        be applied to the literal key, not the expression.  If
        enumerate is False, keys in this Interpreter will not be
        considered for path enumeration.
        """

        if not issubclass(key_type, simsym.Symbolic):
            raise TypeError("key_type must be subclass of Symbolic, not %r" %
                            simsym.strtype(key_type))
        self.__key_type = key_type

        if isinstance(iterable_or_fn, collections.Iterable):
            it = iter(iterable_or_fn)
            self.__fn = lambda x: it.next()
        else:
            self.__fn = iterable_or_fn

        if enumerate:
            self.__realm = self
        else:
            self.__realm = simsym.REALM_IGNORE

        self.__map = {}

    def __getitem__(self, key):
        if not isinstance(key, self.__key_type):
            raise TypeError("key must be %r instance, not %r" %
                            (self.__key_type.__name__, simsym.strtype(key)))
        if _is_literal(simsym.unwrap(key)):
            raise ValueError("key must be non-literal, not %r" % key)

        lit = key.eval(self.__realm)
        hlit = z3util.HashableAst(lit)

        if hlit not in self.__map:
            if self.__fn is None:
                raise ValueError("Interpreter has been read; cannot be extended")
            try:
                self.__map[hlit] = (key, self.__fn(lit))
            except StopIteration:
                raise ValueError("Ran out of values for %r" % key)
        return self.__map[hlit][1]

    def keys(self):
        self.__fn = None
        return (p[0] for p in self.__map.itervalues())
    __iter__ = keys

    def values(self):
        self.__fn = None
        return (p[1] for p in self.__map.itervalues())

    def items(self):
        self.__fn = None
        return self.__map.itervalues()

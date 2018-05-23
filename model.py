import simsym

def methodwrap(**arg_types):
    """Transform a method into a testable model method.

    This returns a decorator for model class methods to make them
    testable.  The keys of arg_types must correspond to the wrapped
    method's arguments (except 'self'), and its values must give the
    symbolic types of those arguments.  The decorator returns the
    function given to it, but augmented with an arg_struct_type
    attribute that gives the symbolic struct for its arguments.
    """

    # Create symbolic struct type for the arguments
    arg_struct_type = simsym.tstruct(**arg_types)

    def decorator(m):
        m.arg_struct_type = arg_struct_type
        m.is_model_function = True
        return m
    return decorator

cur_thread_idx = None

def cur_thread():
    if cur_thread_idx is None:
        raise ValueError('cur_thread called outside spec.test')
    return cur_thread_idx

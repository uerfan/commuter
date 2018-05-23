// Automatically generated file.
#include"gparams.h"
#include"trace.h"
#include"symbol.h"
#include"prime_generator.h"
#include"debug.h"
#include"rational.h"
void mem_initialize() {
initialize_symbols();
rational::initialize();
gparams::init();
}
void mem_finalize() {
gparams::finalize();
finalize_trace();
finalize_symbols();
prime_iterator::finalize();
finalize_debug();
rational::finalize();
}

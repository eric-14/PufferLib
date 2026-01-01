#include "template.h"

#define Env Template 
#include "../env_binding.h"

static int my_init(Env* env, PyObject* args, PyObject* kwargs) {
    printf("[C][binding]  my_init function \r\n"); 
    env->size = unpack(kwargs, "size");
    return 0;
}

static int my_log(PyObject* dict, Log* log) {
    printf("[C][binding]  my_log function \r\n"); 
    assign_to_dict(dict, "score", log->score);
    return 0;
}

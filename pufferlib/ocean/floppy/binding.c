#include "floppy.h"
 
typedef GameEnv Env; 
#include "../env_binding.h"

// typedef GameEnv Log; 

static int my_init(Env* env, PyObject* args, PyObject* kwargs) {
    env->log[0].size = unpack(kwargs, "size");
    env->log[0].num_envs = unpack(kwargs, "num_envs");
    return 0;
}

static int my_log(PyObject* dict, Env* env) {
    assign_to_dict(dict, "score", env->log[0].score);
    assign_to_dict(dict, "ep_length", env->log[0].ep_length);
    assign_to_dict(dict, "ep_return", env->log[0].ep_return);
    assign_to_dict(dict, "tick", env->log[0].tick);
    assign_to_dict(dict, "n", env->log[0].n);
    assign_to_dict(dict, "hiScore", env->log[0].hiScore);
    assign_to_dict(dict, "number_of_ups", env->log[0].number_of_ups);
    return 0;
}

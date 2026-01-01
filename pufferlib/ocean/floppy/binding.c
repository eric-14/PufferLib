#include "floppy.h"
 
typedef GameEnv Env; 
#include "../env_binding.h"

// typedef GameEnv Log; 

static int my_init(Env* env, PyObject* args, PyObject* kwargs) {
    env->log[0].size = unpack(kwargs, "size");
    env->log[0].num_envs = unpack(kwargs, "num_envs");
    return 0;
}

static int my_log(PyObject* dict, Log* log) {
    assign_to_dict(dict, "score", log.score);
    assign_to_dict(dict, "ep_length", log.ep_length);
    assign_to_dict(dict, "ep_return", log.ep_return);
    assign_to_dict(dict, "tick", log.tick);
    assign_to_dict(dict, "n", log.n);
    assign_to_dict(dict, "hiScore", log.hiScore);
    assign_to_dict(dict, "number_of_ups", log.number_of_ups);
    return 0;
}

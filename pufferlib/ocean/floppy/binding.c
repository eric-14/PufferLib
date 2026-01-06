#include "floppy.h"
 
typedef GameEnv Env; 
#include "../env_binding.h"


static int my_log(PyObject* dict, Log* log) {
    // Populate Python dictionary from C Log struct
    // raid uses: assign_to_dict(dict, "perf", log->perf); etc.
    // You need to implement assign_to_dict or use PyDict_SetItemString
    
    PyObject* py_perf = PyFloat_FromDouble(log->perf);
    PyDict_SetItemString(dict, "perf", py_perf);
    Py_DECREF(py_perf);
    
    PyObject* py_episode_return = PyFloat_FromDouble(log->episode_return);
    PyDict_SetItemString(dict, "episode_return", py_episode_return);
    Py_DECREF(py_episode_return);
    
    PyObject* py_episode_length = PyFloat_FromDouble(log->episode_length);
    PyDict_SetItemString(dict, "episode_length", py_episode_length);
    Py_DECREF(py_episode_length);
    
    PyObject* py_score = PyFloat_FromDouble(log->score);
    PyDict_SetItemString(dict, "score", py_score);
    Py_DECREF(py_score);
    
    // n is already added by vec_log, don't add it here
    return 0;
}

// typedef GameEnv Log; 

static int my_init(Env* env, PyObject* args, PyObject* kwargs) {
    env->log.size = unpack(kwargs, "size");
    // env->log.ep_length = unpack(kwargs, "episode_length");
    // env->log.ep_return = unpack(kwargs, "episode_return");
    _game_init(env, 1);
    // env->log.n = unpack(kwargs, "n");
    // env->log.score = unpack(kwargs, "score");
    // env->log.hiScore = unpack(kwargs, "hiScore");
    

   // env->num_envs = unpack(kwargs, "num_envs");
    return 0;
}

// static int my_log(PyObject* dict, Log* log) {
//     assign_to_dict(dict, "score", log->score);
//     assign_to_dict(dict, "episode_length", log->episode_length);
//     assign_to_dict(dict, "episode_return", log->episode_return);
//     assign_to_dict(dict, "tick", log->tick);
//     assign_to_dict(dict, "n", log->n);
//     assign_to_dict(dict, "perf", log->perf);
//     // assign_to_dict(dict, "hiScore", log->hiScore);
//     // assign_to_dict(dict, "number_of_ups", log->number_of_ups);
//     return 0;
// }

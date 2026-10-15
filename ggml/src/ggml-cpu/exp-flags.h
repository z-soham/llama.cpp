#pragma once

// Runtime switches for experimental CPU paths: GGML_CPU_EXP=<comma list>, read once.
#include <stdlib.h>
#include <string.h>

static inline int ggml_cpu_exp_enabled(const char * name) {
    const char * env = getenv("GGML_CPU_EXP");
    if (!env) {
        return 0;
    }
    const size_t n = strlen(name);
    for (const char * p = env; (p = strstr(p, name)) != NULL; p += n) {
        if ((p == env || p[-1] == ',') && (p[n] == ',' || p[n] == '\0')) {
            return 1;
        }
    }
    return 0;
}

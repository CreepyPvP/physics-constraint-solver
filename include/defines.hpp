#include <assert.h>
#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#define GL(line) do {                      \
        line;                                 \
        assert(glGetError() == GL_NO_ERROR);  \
    } while(0)
#else
#  define GL(line) line
#endif

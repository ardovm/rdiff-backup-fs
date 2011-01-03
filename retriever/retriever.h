#ifndef _RETRIEVER_H_
#define _RETRIEVER_H_

#include <pthread.h>

#include "../headers.h"
#include "../externs.h"
#include "../support/gstring.h"
#include "../support/gpath.h"
#include "../structure/core.h"

#include "simple.h"
#include "limit.h"

extern int cache_limit;

int retriever_init_simple(struct file_system_info *);
int retriever_init_limit(struct file_system_info *);

extern int (*retrieve)(struct stats *stats);
extern int (*release)(struct stats *stats);

#endif

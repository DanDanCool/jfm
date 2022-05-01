#pragma once

#include "jfm.h"

enum jfm_ftflags
{
	JFM_FILE = 1 << 0,
	JFM_FOLDER = 1 << 1,
    JFM_SELECTED = 1 << 2,
};

typedef struct
{
	const char* name;
    int flags;
} jfm_fd;

JFM_DEFINE_VEC(jfm_fd);

void jfm_directory(JFM_VEC(jfm_fd)* vec, const char* name);

#include "file.h"

#include <sys/types.h>
#include <dirent.h>

#include <stdlib.h>
#include <string.h>

JFM_VEC_ALL_DEFINE(jfm_fd);

static void clear(JFM_VEC(jfm_fd)* vec)
{
    for (int i = 0; i < vec->sz; i++)
        free(vec->data[i].name);

    JFM_VEC_CLEAR(jfm_fd, vec);
}

void jfm_directory(JFM_VEC(jfm_fd)* vec, const char* name)
{
    clear(vec);

	DIR* dir = opendir(name);
	if (!dir)
		return;

	struct dirent* entry;
	while (entry = readdir(dir))
    {
        if (!(entry->d_type & DT_DIR))
            continue;

        jfm_fd fd = {};
        fd.name = jfm_strcpy(entry->d_name);
        fd.flags = entry->d_type & DT_DIR ? JFM_FOLDER : JFM_FILE;
        JFM_VEC_ADD(jfm_fd, vec, &fd);
    }

    rewinddir(dir);
	while (entry = readdir(dir))
    {
        if (entry->d_type & DT_DIR)
            continue;

        jfm_fd fd = {};
        fd.name = jfm_strcpy(entry->d_name);
        fd.flags = entry->d_type & DT_DIR ? JFM_FOLDER : JFM_FILE;
        JFM_VEC_ADD(jfm_fd, vec, &fd);
    }
}

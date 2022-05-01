#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <curses.h>

#include "file.h"

typedef struct
{
    WINDOW* win;
    int col;
    int row;

    const char* parent;
    const char* dir;

    JFM_VEC(jfm_fd) data;
    JFM_VEC(jfm_fd) selected;

    int flags;
} jfm_state;

void jfm_state_init(jfm_state* state)
{
    state->win = stdscr;

    state->dir = getcwd(NULL, 0);
    state->parent = jfm_strcpy(state->dir);
    char* root = strrchr(state->parent, '/');
    *root = '\0';

    JFM_VEC_CREATE(jfm_fd, &state->data, 16);
    JFM_VEC_CREATE(jfm_fd, &state->selected, 16);

    state->flags = 1;
}

void jfm_state_cd(jfm_state* state, int index)
{
    if (index < 0)
    {
        free(state->dir);
        state->dir = state->parent;
        state->parent = jfm_strcpy(state->dir);
        char* root = strrchr(state->parent, '/');
        *root = '\0';

        state->flags = 1;
        return;
    }

    if (!(state->data.data[index].flags & JFM_FOLDER))
        return;

    const char* dir = state->data.data[index].name;
    int l1 = strlen(state->dir);
    int l2 = strlen(dir);
    char* buf = (char*)malloc(l1 + l2 + 2);
    strcpy(buf, state->dir);
    strcpy(buf + l1 + 1, dir);
    buf[l1] = '/';

    free(state->parent);
    state->parent = state->dir;
    state->dir = buf;

    state->flags = 1;
}

void jfm_select(jfm_state* state)
{
    for (int i = 0; i < state->data.sz; i++)
    {
        jfm_fd* fd = &state->data.data[i];

        int l1 = strlen(state->dir);
        int l2 = strlen(fd->name);
        char* buf = (char*)malloc(l1 + l2 + 2);
        strcpy(buf, state->dir);
        strcpy(buf + l1 + 1, fd->name);
        buf[l1] = '/';

        jfm_fd temp = {};
        temp.name = buf;
        temp.flags = fd->flags;

        if (fd->flags & JFM_SELECTED)
            JFM_VEC_ADD(jfm_fd, &state->selected, &temp);
    }
}

void jfm_unselect(jfm_state* state)
{
    for (int i = 0; i < state->data.sz; i++)
    {
        jfm_fd* fd = &state->data.data[i];
        int clear = ~JFM_SELECTED;
        fd->flags = fd->flags & clear;
    }

    for (int i = 0; i < state->selected.sz; i++)
    {
        jfm_fd* fd = &state->selected.data[i];
        free(fd->name);
    }

    JFM_VEC_CLEAR(jfm_fd, &state->selected);
}

void jfm_delete(jfm_state* state)
{
    for (int i = 0; i < state->selected.sz; i++)
    {
        jfm_fd* fd = &state->selected.data[i];
        remove(fd->name);
    }

    jfm_unselect(state);
    state->flags = 1;
}

void jfm_copy(jfm_state* state)
{
    for (int i = 0; i < state->selected.sz; i++)
    {
        jfm_fd* fd = &state->selected.data[i];
        if (fd->flags & JFM_FOLDER)
            continue;

        int len = strlen(fd->name);
        char* buf = (char*)malloc(len + 6);
        strcpy(buf, fd->name);
        strcpy(buf + len, "_copy");

        int in = open(fd->name, O_RDONLY);
        struct stat info;
        fstat(in, &info);

        int out = open(buf, O_WRONLY | O_CREAT, info.st_mode);
        off_t offset = 0;

        sendfile(out, in, &offset, info.st_size);
        close(in);
        close(out);
    }

    jfm_unselect(state);
    state->flags = 1;
}

void jfm_draw(jfm_state* state)
{
    if (state->flags)
    {
        jfm_directory(&state->data, state->dir);
        state->flags = 0;
    }

    attron(COLOR_PAIR(1));
    int i = 0;
	for(; i < state->data.sz; i++)
	{
		jfm_fd* fd = &state->data.data[i];
        if (!(fd->flags & JFM_FOLDER))
            break;

        const char* sel = fd->flags & JFM_SELECTED ? "*" : "";
        wprintw(state->win, "%s%s\n", fd->name, sel);
	}
    attroff(COLOR_PAIR(1));

	for(; i < state->data.sz; i++)
	{
		jfm_fd* fd = &state->data.data[i];
        const char* sel = fd->flags & JFM_SELECTED ? "*" : "";
        wprintw(state->win, "%s%s\n", fd->name, sel);
	}

    mvwprintw(state->win, LINES - 2, 0, "%s\n", state->dir);
    refresh();
}

int jfm_input(jfm_state* state, int key)
{
    switch(key)
    {
        case 'k': state->row = state->row ? state->row - 1 : 0; break;
        case 'j': state->row = state->row < state->data.sz - 1 ? state->row + 1 : state->data.sz - 1; break;
        case 'l': jfm_state_cd(state, state->row); break;
        case 'h': jfm_state_cd(state, -1); break;
        case 'v': jfm_fd* fd = &state->data.data[state->row]; fd->flags = fd->flags | JFM_SELECTED; break;
        case 'c': jfm_select(state); break;
        case 'p': jfm_select(state); jfm_copy(state); break;
        case 'd': jfm_select(state); jfm_delete(state); break;
    }

    if (key == 'q' || key == '\003')
        return 1;
    else
        return 0;
}

int main()
{
    initscr();
    cbreak();
    noecho();

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);

    jfm_state state = {};
    jfm_state_init(&state);

    while (1)
    {
        clear();
        jfm_draw(&state);
        move(state.row, state.col);

        if (jfm_input(&state, getch()))
            break;
    }

    endwin();
}

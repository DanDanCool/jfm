#pragma once

#define JFM_VEC(type) jfm_vec_##type

#define JFM_DEFINE_VEC(type) \
	typedef struct \
	{ \
		type* data;\
		int cap;\
		int sz;\
	} jfm_vec_##type

#define JFM_VEC_CREATE(type, vec, sz) jfm_vec_##type##_create(vec, sz)
#define JFM_VEC_CREATE_DECLARE(type) jfm_vec_##type jfm_vec_##type##_create(jfm_vec_##type* vec, int sz)
#define JFM_VEC_CREATE_DEFINE(type) \
	void jfm_vec_##type##_create(jfm_vec_##type* vec, int sz) \
	{ \
		vec->data = (type*)malloc(sizeof(type) * sz);\
		vec->cap = sz;\
	}

#define JFM_VEC_RESIZE(type, vec, sz) jfm_vec_##type##_resize(vec, sz)
#define JFM_VEC_RESIZE_DECLARE(type) jfm_vec_##type##_resize(jfm_vec_##type* vec, int sz)
#define JFM_VEC_RESIZE_DEFINE(type) \
	void jfm_vec_##type##_resize(jfm_vec_##type* vec, int sz) \
	{ \
		vec->data = realloc(vec->data, sizeof(type) * sz); \
		vec->cap = sz;\
		vec->sz = vec->sz > sz ? sz : vec->sz;\
	}

#define JFM_VEC_ADD(type, vec, val) jfm_vec_##type##_add(vec, val)
#define JFM_VEC_ADD_DECLARE(type) jfm_vec_##type##_add(jfm_vec_##type* vec, type* val);
#define JFM_VEC_ADD_DEFINE(type) \
    void jfm_vec_##type##_add(jfm_vec_##type* vec, type* val) \
    { \
        if (!(vec->sz < vec->cap)) \
            jfm_vec_##type##_resize(vec, vec->cap * 2); \
        memcpy(vec->data + vec->sz, val, sizeof(type)); \
        vec->sz++; \
    }

#define JFM_VEC_CLEAR(type, vec) jfm_vec_##type##_clear(vec)
#define JFM_VEC_CLEAR_DECLARE(type) jfm_vec_##type##_clear(jfm_vec_##type* vec)
#define JFM_VEC_CLEAR_DEFINE(type) \
    void jfm_vec_##type##_clear(jfm_vec_##type* vec) \
    { \
        vec->sz = 0; \
        memset(vec->data, 0, vec->cap * sizeof(type)); \
    }

#define JFM_VEC_ALL_DEFINE(type) \
    JFM_VEC_CREATE_DEFINE(type); \
    JFM_VEC_RESIZE_DEFINE(type); \
    JFM_VEC_ADD_DEFINE(type); \
    JFM_VEC_CLEAR_DEFINE(type) \

char* jfm_strcpy(const char* str);

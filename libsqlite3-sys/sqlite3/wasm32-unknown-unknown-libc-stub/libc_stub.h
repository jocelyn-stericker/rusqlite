/**
 * This is a subset of libc required to build sqlite.
 * All implementations taken from musl.
 *
 * Copyright © 2005-2020 Rich Felker, et al.
 * Modifications copyright © 2020 Joshua Netterfield.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __UINTPTR_TYPE__ uintptr_t;
typedef unsigned long size_t;

/* ---- stdlib ---- */

// We're going to use Rust's allocator.
void *rusqlite_wasm32_unknown_unknown_malloc (unsigned long size);
void rusqlite_wasm32_unknown_unknown_free(void *mem);
void *rusqlite_wasm32_unknown_unknown_realloc(void *, unsigned long);

#define malloc(x) rusqlite_wasm32_unknown_unknown_malloc(x)
#define free(x) rusqlite_wasm32_unknown_unknown_free(x)
#define realloc(x,y) rusqlite_wasm32_unknown_unknown_realloc(x,y)

/* ---- assert ---- */

#define assert(expr) ((void) (0))

/* ---- string ---- */

#define memcpy(x, y, z) rusqlite_wasm32_unknown_unknown_memcpy(x, y, z);

void *rusqlite_wasm32_unknown_unknown_memcpy(void *restrict dest, const void *restrict src, size_t n);

#define memset(x, y, z) rusqlite_wasm32_unknown_unknown_memset(x, y, z)

void *rusqlite_wasm32_unknown_unknown_memset(void *dest, int c, size_t n);

#define memmove(x, y, z) rusqlite_wasm32_unknown_unknown_memmove(x, y, z)

void *rusqlite_wasm32_unknown_unknown_memmove(void *dest, const void *src, size_t n);

#define strcmp(x, y) rusqlite_wasm32_unknown_unknown_strcmp(x, y)

int rusqlite_wasm32_unknown_unknown_strcmp(const char *l, const char *r);

#define strncmp(x, y, z) rusqlite_wasm32_unknown_unknown_strncmp(x, y, z)

int rusqlite_wasm32_unknown_unknown_strncmp(const char *_l, const char *_r, size_t n);

#define memcmp(x, y, z) rusqlite_wasm32_unknown_unknown_memcmp(x, y, z)

int rusqlite_wasm32_unknown_unknown_memcmp(const void *vl, const void *vr, size_t n);

#define strlen(x) rusqlite_wasm32_unknown_unknown_strlen(x)

size_t rusqlite_wasm32_unknown_unknown_strlen(const char *s);

#define strrchr(x, y) rusqlite_wasm32_unknown_unknown_strrchr(x, y)

char *rusqlite_wasm32_unknown_unknown_strrchr(const char *s, int c);

#define strcspn(x, y) rusqlite_wasm32_unknown_unknown_strcspn(x, y)

size_t rusqlite_wasm32_unknown_unknown_strcspn(const char *s, const char *c);

/* ---- math ---- */

// We'll use rust's version.
#define log(x) rusqlite_wasm32_unknown_unknown_log(x)
double rusqlite_wasm32_unknown_unknown_log(double x);

/* ---- qsort ---- */

#define qsort(w, x, y, z) rusqlite_wasm32_unknown_unknown_qsort(w, x, y, z)

typedef int (*rusqlite_wasm32_unknown_unknown_cmpfun)(const void *, const void *);
void rusqlite_wasm32_unknown_unknown_qsort(void *base, size_t nel, size_t width, rusqlite_wasm32_unknown_unknown_cmpfun cmp);

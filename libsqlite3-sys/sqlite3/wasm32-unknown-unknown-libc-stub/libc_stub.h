/**
 * This is the bare-minimum subset of libc required to build sqlite.
 * All implementations taken from musl.
 *
 * Copyright © 2005-2020 Rich Felker, et al.
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

static void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;

#ifdef __GNUC__

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define LS >>
#define RS <<
#else
#define LS <<
#define RS >>
#endif

    typedef uint32_t __attribute__((__may_alias__)) u32;
    uint32_t w, x;

    for (; (uintptr_t)s % 4 && n; n--) *d++ = *s++;

    if ((uintptr_t)d % 4 == 0) {
        for (; n>=16; s+=16, d+=16, n-=16) {
            *(u32 *)(d+0) = *(u32 *)(s+0);
            *(u32 *)(d+4) = *(u32 *)(s+4);
            *(u32 *)(d+8) = *(u32 *)(s+8);
            *(u32 *)(d+12) = *(u32 *)(s+12);
        }
        if (n&8) {
            *(u32 *)(d+0) = *(u32 *)(s+0);
            *(u32 *)(d+4) = *(u32 *)(s+4);
            d += 8; s += 8;
        }
        if (n&4) {
            *(u32 *)(d+0) = *(u32 *)(s+0);
            d += 4; s += 4;
        }
        if (n&2) {
            *d++ = *s++; *d++ = *s++;
        }
        if (n&1) {
            *d = *s;
        }
        return dest;
    }

    if (n >= 32) switch ((uintptr_t)d % 4) {
    case 1:
        w = *(u32 *)s;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        n -= 3;
        for (; n>=17; s+=16, d+=16, n-=16) {
            x = *(u32 *)(s+1);
            *(u32 *)(d+0) = (w LS 24) | (x RS 8);
            w = *(u32 *)(s+5);
            *(u32 *)(d+4) = (x LS 24) | (w RS 8);
            x = *(u32 *)(s+9);
            *(u32 *)(d+8) = (w LS 24) | (x RS 8);
            w = *(u32 *)(s+13);
            *(u32 *)(d+12) = (x LS 24) | (w RS 8);
        }
        break;
    case 2:
        w = *(u32 *)s;
        *d++ = *s++;
        *d++ = *s++;
        n -= 2;
        for (; n>=18; s+=16, d+=16, n-=16) {
            x = *(u32 *)(s+2);
            *(u32 *)(d+0) = (w LS 16) | (x RS 16);
            w = *(u32 *)(s+6);
            *(u32 *)(d+4) = (x LS 16) | (w RS 16);
            x = *(u32 *)(s+10);
            *(u32 *)(d+8) = (w LS 16) | (x RS 16);
            w = *(u32 *)(s+14);
            *(u32 *)(d+12) = (x LS 16) | (w RS 16);
        }
        break;
    case 3:
        w = *(u32 *)s;
        *d++ = *s++;
        n -= 1;
        for (; n>=19; s+=16, d+=16, n-=16) {
            x = *(u32 *)(s+3);
            *(u32 *)(d+0) = (w LS 8) | (x RS 24);
            w = *(u32 *)(s+7);
            *(u32 *)(d+4) = (x LS 8) | (w RS 24);
            x = *(u32 *)(s+11);
            *(u32 *)(d+8) = (w LS 8) | (x RS 24);
            w = *(u32 *)(s+15);
            *(u32 *)(d+12) = (x LS 8) | (w RS 24);
        }
        break;
    }
    if (n&16) {
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n&8) {
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n&4) {
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n&2) {
        *d++ = *s++; *d++ = *s++;
    }
    if (n&1) {
        *d = *s;
    }
    return dest;
#endif

    for (; n; n--) *d++ = *s++;
    return dest;
}

static void *memset(void *dest, int c, size_t n)
{
    unsigned char *s = dest;
    size_t k;

    /* Fill head and tail with minimal branching. Each
     * conditional ensures that all the subsequently used
     * offsets are well-defined and in the dest region. */

    if (!n) return dest;
    s[0] = c;
    s[n-1] = c;
    if (n <= 2) return dest;
    s[1] = c;
    s[2] = c;
    s[n-2] = c;
    s[n-3] = c;
    if (n <= 6) return dest;
    s[3] = c;
    s[n-4] = c;
    if (n <= 8) return dest;

    /* Advance pointer to align it at a 4-byte boundary,
     * and truncate n to a multiple of 4. The previous code
     * already took care of any head/tail that get cut off
     * by the alignment. */

    k = -(uintptr_t)s & 3;
    s += k;
    n -= k;
    n &= -4;

#ifdef __GNUC__
    typedef uint32_t __attribute__((__may_alias__)) u32;
    typedef uint64_t __attribute__((__may_alias__)) u64;

    u32 c32 = ((u32)-1)/255 * (unsigned char)c;

    /* In preparation to copy 32 bytes at a time, aligned on
     * an 8-byte bounary, fill head/tail up to 28 bytes each.
     * As in the initial byte-based head/tail fill, each
     * conditional below ensures that the subsequent offsets
     * are valid (e.g. !(n<=24) implies n>=28). */

    *(u32 *)(s+0) = c32;
    *(u32 *)(s+n-4) = c32;
    if (n <= 8) return dest;
    *(u32 *)(s+4) = c32;
    *(u32 *)(s+8) = c32;
    *(u32 *)(s+n-12) = c32;
    *(u32 *)(s+n-8) = c32;
    if (n <= 24) return dest;
    *(u32 *)(s+12) = c32;
    *(u32 *)(s+16) = c32;
    *(u32 *)(s+20) = c32;
    *(u32 *)(s+24) = c32;
    *(u32 *)(s+n-28) = c32;
    *(u32 *)(s+n-24) = c32;
    *(u32 *)(s+n-20) = c32;
    *(u32 *)(s+n-16) = c32;

    /* Align to a multiple of 8 so we can fill 64 bits at a time,
     * and avoid writing the same bytes twice as much as is
     * practical without introducing additional branching. */

    k = 24 + ((uintptr_t)s & 4);
    s += k;
    n -= k;

    /* If this loop is reached, 28 tail bytes have already been
     * filled, so any remainder when n drops below 32 can be
     * safely ignored. */

    u64 c64 = c32 | ((u64)c32 << 32);
    for (; n >= 32; n-=32, s+=32) {
        *(u64 *)(s+0) = c64;
        *(u64 *)(s+8) = c64;
        *(u64 *)(s+16) = c64;
        *(u64 *)(s+24) = c64;
    }
#else
    /* Pure C fallback with no aliasing violations. */
    for (; n; n--, s++) *s = c;
#endif

    return dest;
}
#ifdef __GNUC__
typedef __attribute__((__may_alias__)) size_t WT;
#define WS (sizeof(WT))
#endif

static void *memmove(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;

    if (d==s) return d;
    if ((uintptr_t)s-(uintptr_t)d-n <= -2*n) return memcpy(d, s, n);

    if (d<s) {
#ifdef __GNUC__
        if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
            while ((uintptr_t)d % WS) {
                if (!n--) return dest;
                *d++ = *s++;
            }
            for (; n>=WS; n-=WS, d+=WS, s+=WS) *(WT *)d = *(WT *)s;
        }
#endif
        for (; n; n--) *d++ = *s++;
    } else {
#ifdef __GNUC__
        if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
            while ((uintptr_t)(d+n) % WS) {
                if (!n--) return dest;
                d[n] = s[n];
            }
            while (n>=WS) n-=WS, *(WT *)(d+n) = *(WT *)(s+n);
        }
#endif
        while (n) n--, d[n] = s[n];
    }

    return dest;
}


static int strcmp(const char *l, const char *r)
{
    for (; *l==*r && *l; l++, r++);
    return *(unsigned char *)l - *(unsigned char *)r;
}

static int strncmp(const char *_l, const char *_r, size_t n)
{
    const unsigned char *l=(void *)_l, *r=(void *)_r;
    if (!n--) return 0;
    for (; *l && *r && n && *l == *r ; l++, r++, n--);
    return *l - *r;
}

static int memcmp(const void *vl, const void *vr, size_t n)
{
    const unsigned char *l=vl, *r=vr;
    for (; n && *l == *r; n--, l++, r++);
    return n ? *l-*r : 0;
}

static void *__memrchr(const void *m, int c, size_t n)
{
    const unsigned char *s = m;
    c = (unsigned char)c;
    while (n--) if (s[n]==c) return (void *)(s+n);
    return 0;
}

#define UCHAR_MAX 255
#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

static size_t strlen(const char *s)
{
    const char *a = s;
#ifdef __GNUC__
    typedef size_t __attribute__((__may_alias__)) word;
    const word *w;
    for (; (uintptr_t)s % ALIGN; s++) if (!*s) return s-a;
    for (w = (const void *)s; !HASZERO(*w); w++);
    s = (const void *)w;
#endif
    for (; *s; s++);
    return s-a;
}

static char *strrchr(const char *s, int c)
{
    return __memrchr(s, c, strlen(s) + 1);
}

static char *__strchrnul(const char *s, int c)
{
    c = (unsigned char)c;
    if (!c) return (char *)s + strlen(s);

#ifdef __GNUC__
    typedef size_t __attribute__((__may_alias__)) word;
    const word *w;
    for (; (size_t)s % ALIGN; s++)
        if (!*s || *(unsigned char *)s == c) return (char *)s;
    size_t k = ONES * c;
    for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w^k); w++);
    s = (void *)w;
#endif
    for (; *s && *(unsigned char *)s != c; s++);
    return (char *)s;
}

#define BITOP(a,b,op) \
 ((a)[(size_t)(b)/(8*sizeof *(a))] op (size_t)1<<((size_t)(b)%(8*sizeof *(a))))

static size_t strcspn(const char *s, const char *c)
{
    const char *a = s;
    size_t byteset[32/sizeof(size_t)];

    if (!c[0] || !c[1]) return __strchrnul(s, *c)-a;

    memset(byteset, 0, sizeof byteset);
    for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++);
    for (; *s && !BITOP(byteset, *(unsigned char *)s, &); s++);
    return s-a;
}

/* ---- math ---- */

// We'll use rust's version.
double log(double x);

/* ---- qsort ---- */

static inline int ntz(unsigned long x)
{
    static const char debruijn32[32] = {
        0, 1, 23, 2, 29, 24, 19, 3, 30, 27, 25, 11, 20, 8, 4, 13,
        31, 22, 28, 18, 26, 10, 7, 12, 21, 17, 9, 6, 16, 5, 15, 14
    };
    return debruijn32[(x&-x)*0x076be629 >> 27];
}

typedef int (*cmpfun)(const void *, const void *);

static inline int pntz(size_t p[2]) {
    int r = ntz(p[0] - 1);
    if(r != 0 || (r = 8*sizeof(size_t) + ntz(p[1])) != 8*sizeof(size_t)) {
        return r;
    }
    return 0;
}

static void cycle(size_t width, unsigned char* ar[], int n)
{
    unsigned char tmp[256];
    size_t l;
    int i;

    if(n < 2) {
        return;
    }

    ar[n] = tmp;
    while(width) {
        l = sizeof(tmp) < width ? sizeof(tmp) : width;
        memcpy(ar[n], ar[0], l);
        for(i = 0; i < n; i++) {
            memcpy(ar[i], ar[i + 1], l);
            ar[i] += l;
        }
        width -= l;
    }
}

/* shl() and shr() need n > 0 */
static inline void shl(size_t p[2], int n)
{
    if(n >= 8 * sizeof(size_t)) {
        n -= 8 * sizeof(size_t);
        p[1] = p[0];
        p[0] = 0;
    }
    p[1] <<= n;
    p[1] |= p[0] >> (sizeof(size_t) * 8 - n);
    p[0] <<= n;
}

static inline void shr(size_t p[2], int n)
{
    if(n >= 8 * sizeof(size_t)) {
        n -= 8 * sizeof(size_t);
        p[0] = p[1];
        p[1] = 0;
    }
    p[0] >>= n;
    p[0] |= p[1] << (sizeof(size_t) * 8 - n);
    p[1] >>= n;
}

static void sift(unsigned char *head, size_t width, cmpfun cmp, int pshift, size_t lp[])
{
    unsigned char *rt, *lf;
    unsigned char *ar[14 * sizeof(size_t) + 1];
    int i = 1;

    ar[0] = head;
    while(pshift > 1) {
        rt = head - width;
        lf = head - width - lp[pshift - 2];

        if((*cmp)(ar[0], lf) >= 0 && (*cmp)(ar[0], rt) >= 0) {
            break;
        }
        if((*cmp)(lf, rt) >= 0) {
            ar[i++] = lf;
            head = lf;
            pshift -= 1;
        } else {
            ar[i++] = rt;
            head = rt;
            pshift -= 2;
        }
    }
    cycle(width, ar, i);
}

static void trinkle(unsigned char *head, size_t width, cmpfun cmp, size_t pp[2], int pshift, int trusty, size_t lp[])
{
    unsigned char *stepson,
                  *rt, *lf;
    size_t p[2];
    unsigned char *ar[14 * sizeof(size_t) + 1];
    int i = 1;
    int trail;

    p[0] = pp[0];
    p[1] = pp[1];

    ar[0] = head;
    while(p[0] != 1 || p[1] != 0) {
        stepson = head - lp[pshift];
        if((*cmp)(stepson, ar[0]) <= 0) {
            break;
        }
        if(!trusty && pshift > 1) {
            rt = head - width;
            lf = head - width - lp[pshift - 2];
            if((*cmp)(rt, stepson) >= 0 || (*cmp)(lf, stepson) >= 0) {
                break;
            }
        }

        ar[i++] = stepson;
        head = stepson;
        trail = pntz(p);
        shr(p, trail);
        pshift += trail;
        trusty = 0;
    }
    if(!trusty) {
        cycle(width, ar, i);
        sift(head, width, cmp, pshift, lp);
    }
}

static void qsort(void *base, size_t nel, size_t width, cmpfun cmp)
{
    size_t lp[12*sizeof(size_t)];
    size_t i, size = width * nel;
    unsigned char *head, *high;
    size_t p[2] = {1, 0};
    int pshift = 1;
    int trail;

    if (!size) return;

    head = base;
    high = head + size - width;

    /* Precompute Leonardo numbers, scaled by element width */
    for(lp[0]=lp[1]=width, i=2; (lp[i]=lp[i-2]+lp[i-1]+width) < size; i++);

    while(head < high) {
        if((p[0] & 3) == 3) {
            sift(head, width, cmp, pshift, lp);
            shr(p, 2);
            pshift += 2;
        } else {
            if(lp[pshift - 1] >= high - head) {
                trinkle(head, width, cmp, p, pshift, 0, lp);
            } else {
                sift(head, width, cmp, pshift, lp);
            }

            if(pshift == 1) {
                shl(p, 1);
                pshift = 0;
            } else {
                shl(p, pshift - 1);
                pshift = 1;
            }
        }

        p[0] |= 1;
        head += width;
    }

    trinkle(head, width, cmp, p, pshift, 0, lp);

    while(pshift != 1 || p[0] != 1 || p[1] != 0) {
        if(pshift <= 1) {
            trail = pntz(p);
            shr(p, trail);
            pshift += trail;
        } else {
            shl(p, 2);
            pshift -= 2;
            p[0] ^= 7;
            shr(p, 1);
            trinkle(head - lp[pshift] - width, width, cmp, p, pshift + 1, 1, lp);
            shl(p, 1);
            p[0] |= 1;
            trinkle(head - width, width, cmp, p, pshift, 1, lp);
        }
        head -= width;
    }
}

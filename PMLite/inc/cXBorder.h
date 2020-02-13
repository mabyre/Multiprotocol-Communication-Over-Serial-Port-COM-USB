/* ===================================================================
   cXBorder.h
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   ===================================================================  */

#ifndef cXBorder_h
#define cXBorder_h

/* ------------------------------------------------------------------- */

#include "cXTypes.h"

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------
   Note:
      
   In those macros, local variables are used to prevent multiple parameter
   evaluation. This reduces the risk of:  
      c_memcpy_16(++p, q);
   
   that will be expanded to:
   
   do
   {
      pmbyte *d = (pmbyte*)(++p), *s = q;
      *d++ = *q++; *d = *q;
   } while (0)
   
   instead of 
   
   do
   {
      *(++p)++ = *(q)++; *(++p) = *(q);
   } while (0)

   that is completely buggy.

   It also allows to use:
      c_memcpy_16(p + 2, 0);
      
   that would have not compiled otherwise due to *(p + 2)++ that would complain
   that p + 2 is not a valid l-value.

   In any case when using it in its simplest way:
      c_memcpy_16(p, q);
      
   the compiler (at least a good one) should be able to optimize out the local 
   variable created.

   ------------------------------------------------------------------- */

/* -------------------------------------------------------------------
   Memory copy

   Here are the macro prototypes:
   
      c_memcpy_16(pmuint16 *aDest, void *aSrc);
      c_memcpy_32(pmuint32 *aDest, void *aSrc);
   ------------------------------------------------------------------- */

#ifdef PMLITE_MEMORY_NON_ALIGNED

#  define   c_memcpy_16(d0, s0) ((*((pmuint16*)(d0))) = (*((pmuint16*)(s0))))
#  define   c_memcpy_32(d0, s0) ((*((pmuint32*)(d0))) = (*((pmuint32*)(s0))))

#else /* !PMLITE_MEMORY_NON_ALIGNED */

#  define   c_memcpy_16(d0, s0)                       \
      do                                     \
      {                                      \
         pmbyte *d = (pmbyte*)(d0), *s = (pmbyte*)(s0);  \
         *d++ = *s++; *d = *s;                     \
      } while (0)

#  define   c_memcpy_32(d0, s0)                       \
      do                                     \
      {                                      \
         pmbyte *d = (pmbyte*)(d0), *s = (pmbyte*)(s0);  \
         *d++ = *s++; *d++ = *s++; *d++ = *s++; *d = *s; \
      } while (0)

#endif /* !PMLITE_MEMORY_NON_ALIGNED */

/* -------------------------------------------------------------------
   Network Byte Order Tools

   Here are the macro prototypes:
   
      c_store_u16(void *aDest, pmuint16 aSrc);
      c_store_u32(void *aDest, pmuint32 aSrc);

      c_get_u16(pmuint16 *aDest, void *aSrc);
      c_get_u32(pmuint32 *aDest, void *aSrc);

   These macro are storing values to memory in NETWORK byte order (Big Endian).
   They are also getting values from memory assuming that they are stored in
   memory in NETWORK byte order.
   ------------------------------------------------------------------- */

#if !defined(PMLITE_LITTLE_ENDIAN) && defined(PMLITE_MEMORY_NON_ALIGNED)

#  define   c_store_u16(d0, v0)  ((*((pmuint16*)(d0))) = (pmuint16)(v0))
#  define   c_store_u32(d0, v0)  ((*((pmuint32*)(d0))) = (pmuint32)(v0))
#  define   c_get_u16(d0, s0) c_memcpy_16(d0, s0)
#  define   c_get_u32(d0, s0) c_memcpy_32(d0, s0)

#else /* PMLITE_LITTLE_ENDIAN || !PMLITE_MEMORY_NON_ALIGNED */

#  define   c_store_u16(d0, v0)           \
      do                         \
      {                          \
         pmbyte      *d = (pmbyte*)(d0);  \
         pmuint16 v = (v0);         \
         *d++ = (pmbyte)(v >> 8);      \
         *d = (pmbyte)v;               \
      } while (0)

#  define   c_store_u32(d0, v0)           \
      do                         \
      {                          \
         pmbyte      *d = (pmbyte*)(d0);  \
         pmuint32 v = (v0);         \
         *d++ = (pmbyte)(v >> 24);     \
         *d++ = (pmbyte)(v >> 16);     \
         *d++ = (pmbyte)(v >> 8);      \
         *d = (pmbyte)v;               \
      } while (0)

#  define   c_get_u16(d0, s0)                               \
      do                                              \
      {                                               \
         pmbyte   *s = (pmbyte*)(s0);                          \
         *(d0) = (pmuint16)(( ((pmuint16)(s)[0]) << 8 ) + (s)[1]);   \
      } while (0)

#  define   c_get_u32(d0, s0)                               \
      do                                              \
      {                                               \
         pmbyte   *s = (pmbyte*)(s0);                          \
         *(d0) = (pmuint32)(( ((pmuint32)(s)[0]) << 24 ) + ( ((pmuint32)(s)[1]) << 16 ) + ( ((pmuint32)(s)[2]) << 8 ) + (s)[3]); \
      } while (0)

#endif /* PMLITE_LITTLE_ENDIAN || !PMLITE_MEMORY_NON_ALIGNED */

/* -------------------------------------------------------------------
   Little Endian Byte Order Tools

   Here are the macro prototypes:
   
      c_store_le_u16(void *aDest, pmuint16 aSrc);
      c_store_le_u32(void *aDest, pmuint32 aSrc);

      c_get_le_u16(pmuint16 *aDest, void *aSrc);
      c_get_le_u32(pmuint32 *aDest, void *aSrc);

   These macro are storing values to memory in LITTLE ENDIAN byte order.
   They are also getting values from memory assuming that they are stored in
   memory in LITTLE ENDIAN byte order.
   ------------------------------------------------------------------- */

#if defined(PMLITE_LITTLE_ENDIAN) && defined(PMLITE_MEMORY_NON_ALIGNED)

#  define   c_store_le_u16(d0, v0)  ((*((pmuint16*)(d0))) = (pmuint16)(v0))
#  define   c_store_le_u32(d0, v0)  ((*((pmuint32*)(d0))) = (pmuint32)(v0))
#  define   c_get_le_u16(d0, s0) c_memcpy_16(d0, s0)
#  define   c_get_le_u32(d0, s0) c_memcpy_32(d0, s0)

#else /* !PMLITE_LITTLE_ENDIAN || !PMLITE_MEMORY_NON_ALIGNED */

#  define   c_store_le_u16(d0, v0)        \
      do                         \
      {                          \
         pmbyte      *d = (pmbyte*)(d0);  \
         pmuint16 v = (v0);         \
         *d++ = (pmbyte)v;          \
         *d = (pmbyte)(v >> 8);        \
      } while (0)

#  define   c_store_le_u32(d0, v0)        \
      do                         \
      {                          \
         pmbyte      *d = (pmbyte*)(d0);  \
         pmuint32 v = (v0);         \
         *d++ = (pmbyte)v;          \
         *d++ = (pmbyte)(v >> 8);      \
         *d++ = (pmbyte)(v >> 16);     \
         *d = (pmbyte)(v >> 24);       \
      } while (0)

#  define   c_get_le_u16(d0, s0)                            \
      do                                              \
      {                                               \
         pmbyte   *s = (pmbyte*)(s0);                          \
         *(d0) = (pmuint16)(( ((pmuint16)(s)[1]) << 8 ) + (s)[0]);   \
      } while (0)

#  define   c_get_le_u32(d0, s0)                            \
      do                                              \
      {                                               \
         pmbyte   *s = (pmbyte*)(s0);                          \
         *(d0) = (pmuint32)(( ((pmuint32)(s)[3]) << 24 ) + ( ((pmuint32)(s)[2]) << 16 ) + ( ((pmuint32)(s)[1]) << 8 ) + (s)[0]); \
      } while (0)

#endif /* PMLITE_LITTLE_ENDIAN || !PMLITE_MEMORY_NON_ALIGNED */

/* -------------------------------------------------------------------
   These functions do the same as c_get_u16 and c_get_u32 but are functions.
   They read an u16 or u32 stored in network order in memory and return it.
   ------------------------------------------------------------------- */

pmuint16 c_read_u16(pmbyte* aPtr);
pmuint32 c_read_u32(pmbyte* aPtr);

/* -------------------------------------------------------------------
   These functions do the same as c_get_le_u16 and c_get_le_u32 but are functions.
   They read an u16 or u32 stored in LITTLE ENDIAN order in memory and return it.
   ------------------------------------------------------------------- */

pmuint16 c_read_le_u16(pmbyte* aPtr);
pmuint32 c_read_le_u32(pmbyte* aPtr);

/* -------------------------------------------------------------------
   Byte ordering management
   
   The c_swap_u16 function returns a pmuint16 that has its byte swapped regarding
   the original pmuint16. For Example:
      c_swap_u16(0x1234)   returns 0x3412
   
   The c_swap_u32 function returns a pmuint32 that has its byte swapped regarding
   the original pmuint32. For Example:
      c_swap_u32(0x12345678)  returns 0x78563412
   
   ------------------------------------------------------------------- */

pmuint16 c_swap_u16(pmuint16 anU16);
pmuint32 c_swap_u32(pmuint32 anU32);

/* -------------------------------------------------------------------
   Byte ordering management
   
   The c_ntoh16 function converts a pmuint16 that has its bytes ordered in the network
   byte ordering (ie: big endian) to the platform processor byte ordering.
   For Example:
      c_ntoh16(0x1234) returns 0x3412 for a little endian platform (like x86 processor)
      c_ntoh16(0x1234) returns 0x1234 for a big endian platform (like 68K processor)
   
   The c_ntoh32 function acts the same for pmuint32.

   
   The c_hton16 function converts a pmuint16 that has its bytes ordered in the the 
   platform processor byte ordering to the network byte ordering (ie: big endian)
   For Example:
      c_hton16(0x1234) returns 0x3412 for a little endian platform (like x86 processor)
      c_hton16(0x1234) returns 0x1234 for a big endian platform (like 68K processor)
   
   The c_hton32 function acts the same for pmuint32.

   Here are the macro prototypes:
   
      pmuint16 c_ntoh16(pmuint16 anInt16);
      pmuint16 c_hton16(pmuint32 anInt16);
      pmuint32 c_ntoh32(pmuint32 anInt32);
      pmuint32 c_hton32(pmuint32 anInt32);
   
   ------------------------------------------------------------------- */

#ifdef PMLITE_LITTLE_ENDIAN

#  define c_ntoh16   c_swap_u16
#  define c_hton16   c_swap_u16
#  define c_ntoh32   c_swap_u32
#  define c_hton32   c_swap_u32

#else /* !PMLITE_LITTLE_ENDIAN */

#  define c_ntoh16(x)   ((pmuint16) (x))
#  define c_hton16(x)   ((pmuint16) (x))
#  define c_ntoh32(x)   ((pmuint32) (x))
#  define c_hton32(x)   ((pmuint32) (x))

#endif /* PMLITE_LITTLE_ENDIAN */

/* ------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------- */

#endif

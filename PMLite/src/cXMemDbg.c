/* ===================================================================
   cXMemDbg.c
   Copyright (C) 1996-98 AbyreSoft - All rights reserved.
   ===================================================================  */

#include "cXCore.h"
#include "cXStack.h"
#include "cXTrace.h"
#include "cXMemDbg.h"

/* ------------------------------------------------------------------- */

#ifdef PMLITE_MEMORY_DEBUG

/* ------------------------------------------------------------------- */

#define MEM_SAFETY_GAP_SIZE      4
#define MEM_SAFETY_GAP_CONTENT   0xFD
#define MEM_FREED_CONTENT     0xDD
#define MEM_NEW_CONTENT       0xCD

/* ------------------------------------------------------------------- */

struct _PM_MBH;
typedef struct _PM_MBH PM_MBH;

struct _PM_MBH
{
   PM_MBH*     itsNext;
   PM_MBH*     itsPrev;
   const char* itsFileName;
   int         itsLine;
   size_t      itsSize;
   pmuint32 itsID;
   pmbyte      itsGap1[MEM_SAFETY_GAP_SIZE];
};

/* ------------------------------------------------------------------- */

#define PM_MBH_H2B(h)   ( (pmbyte*) (((PM_MBH*) h) + 1) )
#define PM_MBH_B2H(b)   ( ((PM_MBH*) b) - 1 )

/* ------------------------------------------------------------------- */

static pmuint32   sCurID = 1;
static pmuint32   sTotalAlloc;
static pmuint32   sCurAlloc;
static pmuint32   sPeakAlloc;

/* If this one is not 0 then it is the maximum memory that can be allocated.  */
static pmuint32   sMaxAlloc;

static PM_MBH  *sMBHHead;
static PM_MBH  *sMBHTail;

/* ------------------------------------------------------------------- */

static pmbool CheckBytes(pmbyte* aBlock, pmbyte aChar, size_t aSize)
{
   pmbool thefOK = pmtrue;
   
   while (aSize-- && thefOK)
   {
      if (*aBlock++ != aChar)
      {
         c_trace(TL(pmT_Core, "Memory block corrupted at address: [%lx]", aBlock - 1));
         thefOK = pmfalse;
      }
   }
   
   c_assert(thefOK, "Memory corruption.");
   
   return thefOK;
}

/* ------------------------------------------------------------------- */

#pragma message("Compiling c_malloc_dbg_imp")

void *c_malloc_dbg_imp(size_t aSize, const char* aFileName, int aLine)
{
   pmuint32 theID = sCurID;
   size_t      theSize;
   PM_MBH      *theMBH;
   pmbyte      *theBlock;

   PMLITE_BEGIN_PROC(c_malloc_dbg_imp);

   if ((sMaxAlloc != 0) && ((sCurAlloc + aSize) > sMaxAlloc))
   {
      c_trace(TL(pmT_Core, "c_malloc_dbg_imp(): Out of memory when allocating [%lu] bytes in file [%s], line [%d]", aSize, aFileName, aLine));
      return 0;
   }

   theSize = sizeof(PM_MBH) + aSize + MEM_SAFETY_GAP_SIZE;
   theMBH = (PM_MBH*) c_malloc_proc(theSize);
   if (theMBH == 0)
      return 0;

      /* Allocation is done.  */
   sCurID++;
   sTotalAlloc += aSize;
   sCurAlloc += aSize;
   if (sCurAlloc > sPeakAlloc)
      sPeakAlloc = sCurAlloc;

      /* Link the new block into the list.   */
   if (sMBHHead == 0)
   {
         /* Empty list. */
      sMBHHead = sMBHTail = theMBH;
      theMBH->itsPrev = 0;
   }
   else
   {
      theMBH->itsPrev = sMBHTail;
      sMBHTail->itsNext = theMBH;
      sMBHTail = theMBH;
   }

      /* Initialize all other fields.  */
   theMBH->itsNext = 0;
   theMBH->itsFileName = aFileName;
   theMBH->itsLine = aLine;
   theMBH->itsSize = aSize;
   theMBH->itsID = theID;

      /* Fill in all gaps...  */
   c_memset(theMBH->itsGap1, MEM_SAFETY_GAP_CONTENT, MEM_SAFETY_GAP_SIZE);
   theBlock = PM_MBH_H2B(theMBH);
   c_memset(theBlock, MEM_NEW_CONTENT, aSize);
   c_memset(theBlock + theMBH->itsSize, MEM_SAFETY_GAP_CONTENT, MEM_SAFETY_GAP_SIZE);

   return theBlock;
}

/* ------------------------------------------------------------------- */

void *c_realloc_dbg_imp(void* aBlock, size_t aSize, const char* aFileName, int aLine)
{
   PM_MBH   *theMBH, *theMBH2;
   size_t   theSize, theOldSize;
   pmbyte   *theBlock;

   PMLITE_BEGIN_PROC(c_realloc_dbg_imp);

   if (aBlock == 0)
      return c_malloc_dbg_imp(aSize, aFileName, aLine);
      
   if (aSize == 0)
   {
      c_assert(aBlock != 0, "c_realloc_dbg_imp(): aBlock and aSize == 0");
      c_free_dbg_imp(aBlock);
      return 0;
   }

   theMBH = PM_MBH_B2H(aBlock);
   CheckBytes(theMBH->itsGap1, MEM_SAFETY_GAP_CONTENT, MEM_SAFETY_GAP_SIZE);
   CheckBytes(((unsigned char*)aBlock) + theMBH->itsSize, MEM_SAFETY_GAP_CONTENT, MEM_SAFETY_GAP_SIZE);

   if (sMaxAlloc != 0 && sCurAlloc + aSize - theMBH->itsSize > sMaxAlloc)
   {
      c_trace(TL(pmT_Core, "c_malloc_dbg_imp(): Out of memory when reallocating [%lu] bytes in file [%s], line [%d]", aSize, aFileName, aLine));
      return 0;
   }

   theOldSize = theMBH->itsSize;
   theSize = sizeof(PM_MBH) + aSize + MEM_SAFETY_GAP_SIZE;
   theMBH2 = (PM_MBH*)c_realloc_proc(theMBH, theSize);
   if (theMBH2 == 0)
   {
      void* theNewBlock = c_malloc_dbg_imp(aSize, theMBH->itsFileName, theMBH->itsLine);
      if (theNewBlock == 0)
         return 0;
      c_memcpy(theNewBlock, aBlock, theMBH->itsSize);
      c_free_dbg_imp(aBlock);
      return theNewBlock;
   }

   if (aSize > theOldSize)
      sTotalAlloc += (aSize - theOldSize);
   sCurAlloc += (aSize - theOldSize);
   if (sCurAlloc > sPeakAlloc)
      sPeakAlloc = sCurAlloc;

      /* Change the links in the list. */
   if (theMBH2->itsPrev == 0)
      sMBHHead = theMBH2;
   else
      theMBH2->itsPrev->itsNext = theMBH2;

   if (theMBH2->itsNext == 0)
      sMBHTail = theMBH2;
   else
      theMBH2->itsNext->itsPrev = theMBH2;

      /* Initialize all other fields.  */
   theMBH2->itsSize = aSize;

      /* Fill in all gaps...  */
   theBlock = PM_MBH_H2B(theMBH2);
   if (aSize > theOldSize)
      c_memset(theBlock + theOldSize, MEM_NEW_CONTENT, aSize - theOldSize);
   c_memset(theBlock + theMBH2->itsSize, MEM_SAFETY_GAP_CONTENT, MEM_SAFETY_GAP_SIZE);

   return theBlock;
}

/* ------------------------------------------------------------------- */

void c_free_dbg_imp(void* aBlock)
{
   PM_MBH   *theMBH;

   PMLITE_BEGIN_PROC(c_free_dbg_imp);

   if (aBlock == 0)
      return;

   theMBH = PM_MBH_B2H(aBlock);

   CheckBytes(theMBH->itsGap1, MEM_SAFETY_GAP_CONTENT, MEM_SAFETY_GAP_SIZE);
   CheckBytes(((unsigned char*)aBlock) + theMBH->itsSize, MEM_SAFETY_GAP_CONTENT, MEM_SAFETY_GAP_SIZE);

   sCurAlloc -= theMBH->itsSize;

      /* Unlink it from the list.   */
   if (theMBH->itsPrev == 0)
      sMBHHead = theMBH->itsNext;
   else
      theMBH->itsPrev->itsNext = theMBH->itsNext;

   if (theMBH->itsNext == 0)
      sMBHTail = theMBH->itsPrev;
   else
      theMBH->itsNext->itsPrev = theMBH->itsPrev;

   c_memset(aBlock, MEM_FREED_CONTENT, theMBH->itsSize);
   c_free_proc(theMBH);
}

/* ------------------------------------------------------------------- */

void c_xmemdbg_dump_state_imp()
{
   PM_MBH      *theMBH = sMBHHead;
   pmuint32 theCount = 0;
   
   c_trace(TL(pmT_Core, "--------------"));
   c_trace(TL(pmT_Core, "Memory report:"));
   c_trace(TL(pmT_Core, "Blocks allocated: %lu", (sCurID - 1)));
   c_trace(TL(pmT_Core, "Peak allocated: %lu", sPeakAlloc));
   c_trace(TL(pmT_Core, "Total allocated: %lu", sTotalAlloc));
   c_trace(TL(pmT_Core, "--------------"));
   
   if (sMBHHead != 0)
      c_trace(TL(pmT_Core, "*** Memory leaks ***"));
   else
      c_trace(TL(pmT_Core, "No memory leak. Well done."));
   
   while (theMBH != 0)
   {
      theCount++;
      c_trace(TL(pmT_Core, "ID %lu, Size %lu, Allocated in %s, Line %d", theMBH->itsID, theMBH->itsSize, theMBH->itsFileName, theMBH->itsLine));
      theMBH = theMBH->itsNext;
   }

   if (theCount != 0)
   {
      c_trace(TL(pmT_Core, "--------------"));
      c_trace(TL(pmT_Core, "Blocks not deallocated: %lu", theCount));
      c_trace(TL(pmT_Core, "Total not deallocated: %lu", sCurAlloc));
   }
   
   c_trace(TL(pmT_Core, "--------------"));
}

/* ------------------------------------------------------------------- */

void c_xmemdbg_set_max_imp(size_t aSize)
{
   sMaxAlloc = aSize;
}

/* ------------------------------------------------------------------- */

void c_xmemdbg_check_imp(void)
{
   PM_MBH   *theMBH = sMBHHead;
   pmbyte   *theBlock;

   while (theMBH != 0)
   {
      theBlock = PM_MBH_H2B(theMBH);
      
      CheckBytes(theMBH->itsGap1, MEM_SAFETY_GAP_CONTENT, MEM_SAFETY_GAP_SIZE);
      CheckBytes(theBlock + theMBH->itsSize, MEM_SAFETY_GAP_CONTENT, MEM_SAFETY_GAP_SIZE);
      
      theMBH = theMBH->itsNext;
   }
}

/* ------------------------------------------------------------------- */

#endif /* PMLITE_MEMORY_DEBUG */

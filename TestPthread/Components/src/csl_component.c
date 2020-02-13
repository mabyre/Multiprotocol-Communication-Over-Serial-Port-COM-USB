/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright2016                           */
/*****************************************************************************/
/*  Name: cl_component.c                                                     */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description : memory allocation functions                                 */
/*****************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "csl.h"
#include "cl_readers.h"
#include "cltypes.h"

#include "..\..\pmInterface.h"
#include "cXMemDbg.h"

/*---------------------------------------------------------------------------*/

e_Result csl_AllocDebug( void **pptBuff, clu32 ulLen, const char* aFileName, int aLine)
{
	*pptBuff = c_malloc_dbg_imp( ulLen, aFileName, aLine );
	if (*pptBuff == NULL)
	{
		return CL_ERROR;
	}
	return CL_OK;
}

/*---------------------------------------------------------------------------*/

e_Result csl_FreeDebug( void *ptBuff )
{
	c_free_dbg_imp( ptBuff );

	ptBuff = CL_NULL; // clairement ca marche pas,
	                  // apres le free, je fais = CL_NULL,
					  // mais faudrait comprendre pourquoi,
					  // il faut un ** sans doute pour modifier le contenu
	return CL_OK;
}

/*---------------------------------------------------------------------------*/

e_Result csl_FreeSafeDebug( void **ptBuff )
{
	c_free_dbg_imp( *ptBuff );
	*ptBuff = (void *)0;
	return CL_OK;
}


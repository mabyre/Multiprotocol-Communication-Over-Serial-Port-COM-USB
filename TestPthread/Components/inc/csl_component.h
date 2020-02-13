/*--------------------------------------------------------------------------*\
 * Copyright (c) 2015 SoDevLog - Written by Bruno Raby.
 *--------------------------------------------------------------------------*
 * csl_component.h
\*--------------------------------------------------------------------------*/

#ifndef CSL_COMPONENT_H_
#define CSL_COMPONENT_H_

#include "csl.h"

e_Result csl_AllocDebug( clvoid **pptBuff, clu32 ulLen, const char* aFileName, int aLine);
e_Result csl_FreeDebug( clvoid *ptBuff );
e_Result csl_FreeSafeDebug( clvoid **ptBuff );

#endif /* CSL_COMPONENT_H_ */

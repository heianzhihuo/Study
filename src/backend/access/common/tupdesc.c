/*-------------------------------------------------------------------------
 *
 * tupdesc.c--
 *    POSTGRES tuple descriptor support code
 *
 * Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *    $Header: /usr/local/cvsroot/postgres95/src/backend/access/common/tupdesc.c,v 1.11 1996/11/05 07:42:46 scrappy Exp $
 *
 * NOTES
 *    some of the executor utility code such as "ExecTypeFromTL" should be
 *    moved here.
 *
 *-------------------------------------------------------------------------
 */

#include <stdio.h>

#include <postgres.h>

#include <parser/catalog_utils.h>
#include <nodes/parsenodes.h>
#include <utils/builtins.h>
#include <utils/fcache.h>
#include <utils/tqual.h>
#include <utils/syscache.h>

#ifndef HAVE_MEMMOVE
# include <regex/utils.h>
#else
# include <string.h>
#endif

/* ----------------------------------------------------------------
 *	CreateTemplateTupleDesc
 *
 *	This function allocates and zeros a tuple descriptor structure.
 * ----------------------------------------------------------------
 */
TupleDesc
CreateTemplateTupleDesc(int natts)
{
    uint32	size;
    TupleDesc desc;
    
    /* ----------------
     *	sanity checks
     * ----------------
     */
    AssertArg(natts >= 1);
    
    /* ----------------
     *  allocate enough memory for the tuple descriptor and
     *  zero it as TupleDescInitEntry assumes that the descriptor
     *  is filled with NULL pointers.
     * ----------------
     */
    size = natts * sizeof (AttributeTupleForm);
    desc = (TupleDesc) palloc(sizeof(struct tupleDesc));
    desc->attrs = (AttributeTupleForm*) palloc(size);
    memset(desc->attrs, 0, size);

    desc->natts = natts;

    return (desc);
}


/*-------------------------------------------------------------------------
 *
 * indexing.c--
 *    This file contains routines to support indices defined on system
 *    catalogs.
 *
 * Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *    $Header: /usr/local/cvsroot/postgres95/src/backend/catalog/indexing.c,v 1.9 1997/01/10 20:16:47 momjian Exp $
 *
 *-------------------------------------------------------------------------
 */
#include <string.h>

#include <postgres.h>

#include <utils/builtins.h>
#include <utils/oidcompos.h>
#include <access/heapam.h>
#include <access/genam.h>
#include <storage/bufmgr.h>
#include <nodes/execnodes.h>
#include <catalog/catalog.h>
#include <catalog/catname.h>
#include <catalog/pg_index.h>
#include <catalog/pg_proc.h>
#include <utils/syscache.h>
#include <catalog/indexing.h>
#include <catalog/index.h>
#include <miscadmin.h>


/*
 * This is needed at initialization when reldescs for some of the crucial
 * system catalogs are created and nailed into the cache.
 */
bool
CatalogHasIndex(char *catName, Oid catId)
{
    Relation    pg_class;
    HeapTuple   htup;
    Form_pg_class pgRelP;
    int i;
    
    Assert(IsSystemRelationName(catName));
    
    /*
     * If we're bootstraping we don't have pg_class (or any indices).
     */
    if (IsBootstrapProcessingMode())
	return false;
    
    if (IsInitProcessingMode()) {
	for (i = 0; IndexedCatalogNames[i] != NULL; i++) {
	    if ( strcmp(IndexedCatalogNames[i], catName) == 0)
		return (true);
	}
	return (false);
    }
    
    pg_class = heap_openr(RelationRelationName);
    htup = ClassOidIndexScan(pg_class, catId);
    heap_close(pg_class);
    
    if (! HeapTupleIsValid(htup)) {
	elog(NOTICE, "CatalogHasIndex: no relation with oid %d", catId);
	return false;
    }
    
    pgRelP = (Form_pg_class)GETSTRUCT(htup);
    return (pgRelP->relhasindex);
}

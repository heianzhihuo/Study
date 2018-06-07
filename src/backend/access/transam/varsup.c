/*-------------------------------------------------------------------------
 *
 * varsup.c--
 *    postgres variable relation support routines
 *
 * Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *    $Header: /usr/local/cvsroot/postgres95/src/backend/access/transam/varsup.c,v 1.7 1997/01/16 07:59:11 vadim Exp $
 *
 *-------------------------------------------------------------------------
 */

#include <postgres.h>

#include <access/transam.h>
#include <storage/spin.h>
#include <access/xact.h>
#include <access/heapam.h>
#include <catalog/catname.h>

/* ---------------------
 *	spin lock for oid generation
 * ---------------------
 */
int OidGenLockId;

/* ----------------------------------------------------------------
 *	      variable relation query/update routines
 * ----------------------------------------------------------------
 */

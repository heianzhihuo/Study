/*-------------------------------------------------------------------------
 *
 * name.c--
 *    Functions for the built-in type "name".
 * name replaces char16 and is carefully implemented so that it
 * is a string of length NAMEDATALEN.  DO NOT use hard-coded constants anywhere
 * always use NAMEDATALEN as the symbolic constant!   - jolly 8/21/95
 * 
 *
 * Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *    $Header: /usr/local/cvsroot/postgres95/src/backend/utils/adt/name.c,v 1.1.1.1 1996/07/09 06:22:04 scrappy Exp $
 *
 *-------------------------------------------------------------------------
 */
#include <string.h>
#include "postgres.h"
#include "utils/builtins.h"	/* where the declarations go */
#include "utils/palloc.h"	/* where the declarations go */


int 
namestrcpy(Name name, char *str)
{
    if (!name || !str)
	return(-1);
    memset(name->data, 0, sizeof(NameData));
    (void) strncpy(name->data, str, NAMEDATALEN);
    return(0);
}

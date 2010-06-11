/** @file oyStruct.h
 *
 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    r
 */


#ifndef OY_STRUCT_H
#define OY_STRUCT_H

typedef struct oyStruct_s* oyStruct;


const char * oyStruct_GetText( oyStruct_s * obj, oyNAME_e name_type, uint32_t flags );


#endif /* OY_STRUCT_H */

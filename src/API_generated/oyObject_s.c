/** @file oyObject_s.c

   [Template file inheritance graph]
   +-- oyObject_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2020 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#include "oyranos_object_internal.h"

#include "oyObject_s.h"
#include "oyObject_s_.h"

#include "oyName_s_.h"


/* Include "Object.public_methods_definitions.c" { */
#include "oyranos_types.h"           /* uint64_t uintptr_t */

#if OY_USE_OBJECT_POOL_
static oyObject_s oy_object_pool_[100] = {
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,

0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0
};
#endif

#include "oyranos_config_internal.h"
#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif

/* oyObject common object Functions { */

/** @brief   object management 
 *  @ingroup  objects_generic
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2009/06/02
 */
oyObject_s         oyObject_New      ( const char        * name )
{
  return oyObject_NewWithAllocators( oyAllocateFunc_, oyDeAllocateFunc_, name );
}

void               oyObject_Track    ( oyObject_s          obj,
                                       const char        * name );
void               oyObject_UnTrack    ( oyObject_s          obj );

/** @brief   object management 
 *  @ingroup  objects_generic
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2009/06/02
 */
oyObject_s         oyObject_NewWithAllocators (
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc,
                                       const char        * name )
{
  oyObject_s o = 0;
  int error = 0;
  int len = sizeof(struct oyObject_s_);
  int id = oyGetNewObjectID();

#if OY_USE_OBJECT_POOL_
  int old_obj = 0, i = 0;

  for(i = 0; i < 100; ++i)
    if(oy_object_pool_[i] != 0)
    {
      old_obj = 1;
      o = oy_object_pool_[i];
      oy_object_pool_[i] = 0;
      break;
    }

  if(old_obj == 0)
#endif
  switch(id) /* give valgrind a glue, which object was created */
  {
    case 1: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 2: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 3: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 4: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 5: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 6: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 7: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 8: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 9: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 10: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 11: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 12: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 13: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 14: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 15: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 16: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 17: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 18: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 19: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 20: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 21: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 22: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 23: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 24: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 25: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 26: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 27: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 28: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 29: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 30: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 31: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 32: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 33: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 34: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 35: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 36: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 37: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 38: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 39: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 40: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 41: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 42: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 43: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 44: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 45: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 46: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 47: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 48: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 49: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 50: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 51: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 52: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 53: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 54: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 55: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 56: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 57: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 58: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 59: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 60: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 61: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 62: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 63: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 64: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 65: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 66: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 67: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 68: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 69: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 70: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 71: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 72: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 73: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 74: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 75: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 76: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 77: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 78: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 79: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 80: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 81: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 82: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 83: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 84: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 85: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 86: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 87: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 88: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 89: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 90: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 91: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 92: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 93: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 94: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 95: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 96: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 97: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 98: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 99: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 100: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 101: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 102: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 103: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 104: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 105: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 106: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 107: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 108: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 109: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 110: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 111: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 112: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 113: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 114: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 115: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 116: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 117: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 118: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 119: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 120: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 121: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 122: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 123: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 124: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 125: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 126: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 127: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 128: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 129: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 130: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 131: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 132: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 133: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 134: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 135: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 136: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 137: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 138: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 139: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 140: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 141: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 142: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 143: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 144: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 145: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 146: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 147: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 148: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 149: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 150: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 151: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 152: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 153: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 154: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 155: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 156: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 157: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 158: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 159: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 160: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 161: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 162: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 163: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 164: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 165: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 166: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 167: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 168: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 169: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 170: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 171: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 172: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 173: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 174: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 175: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 176: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 177: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 178: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 179: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 180: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 181: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 182: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 183: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 184: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 185: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 186: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 187: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 188: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 189: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 190: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 191: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 192: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 193: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 194: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 195: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 196: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 197: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 198: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 199: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 200: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 201: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 202: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 203: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 204: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 205: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 206: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 207: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 208: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 209: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 210: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 211: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 212: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 213: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 214: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 215: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 216: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 217: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 218: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 219: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 220: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 221: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 222: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 223: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 224: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 225: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 226: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 227: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 228: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 229: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 230: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 231: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 232: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 233: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 234: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 235: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 236: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 237: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 238: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 239: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 240: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 241: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 242: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 243: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 244: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 245: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 246: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 247: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 248: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 249: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 250: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 251: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 252: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 253: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 254: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 255: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 256: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 257: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 258: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 259: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 260: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 261: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 262: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 263: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 264: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 265: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 266: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 267: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 268: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 269: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 270: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 271: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 272: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 273: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 274: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 275: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 276: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 277: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 278: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 279: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 280: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 281: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 282: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 283: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 284: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 285: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 286: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 287: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 288: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 289: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 290: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 291: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 292: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 293: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 294: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 295: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 296: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 297: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 298: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 299: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 300: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 301: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 302: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 303: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 304: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 305: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 306: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 307: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 308: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 309: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 310: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 311: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 312: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 313: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 314: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 315: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 316: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 317: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 318: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 319: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 320: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 321: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 322: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 323: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 324: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 325: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 326: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 327: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 328: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 329: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 330: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 331: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 332: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 333: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 334: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 335: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 336: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 337: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 338: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 339: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 340: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 341: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 342: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 343: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 344: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 345: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 346: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 347: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 348: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 349: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 350: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 351: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 352: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 353: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 354: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 355: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 356: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 357: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 358: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 359: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 360: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 361: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 362: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 363: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 364: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 365: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 366: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 367: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 368: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 369: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 370: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 371: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 372: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 373: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 374: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 375: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 376: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 377: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 378: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 379: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 380: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 381: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 382: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 383: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 384: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 385: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 386: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 387: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 388: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 389: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 390: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 391: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 392: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 393: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 394: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 395: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 396: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 397: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 398: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 399: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 400: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 401: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 402: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 403: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 404: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 405: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 406: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 407: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 408: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 409: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 410: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 411: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 412: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 413: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 414: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 415: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 416: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 417: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 418: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 419: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 420: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 421: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 422: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 423: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 424: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 425: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 426: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 427: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 428: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 429: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 430: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 431: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 432: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 433: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 434: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 435: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 436: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 437: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 438: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 439: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 440: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 441: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 442: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 443: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 444: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 445: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 446: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 447: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 448: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 449: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 450: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 451: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 452: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 453: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 454: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 455: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 456: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 457: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 458: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 459: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 460: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 461: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 462: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 463: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 464: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 465: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 466: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 467: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 468: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 469: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 470: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 471: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 472: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 473: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 474: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 475: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 476: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 477: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 478: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 479: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 480: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 481: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 482: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 483: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 484: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 485: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 486: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 487: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 488: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 489: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 490: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 491: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 492: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 493: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 494: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 495: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 496: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 497: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 498: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 499: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 500: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 501: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 502: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 503: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 504: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 505: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 506: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 507: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 508: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 509: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 510: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 511: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 512: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 513: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 514: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 515: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 516: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 517: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 518: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 519: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 520: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 521: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 522: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 523: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 524: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 525: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 526: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 527: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 528: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 529: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 530: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 531: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 532: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 533: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 534: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 535: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 536: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 537: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 538: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 539: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 540: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 541: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 542: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 543: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 544: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 545: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 546: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 547: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 548: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 549: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 550: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 551: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 552: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 553: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 554: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 555: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 556: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 557: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 558: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 559: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 560: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 561: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 562: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 563: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 564: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 565: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 566: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 567: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 568: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 569: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 570: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 571: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 572: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 573: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 574: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 575: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 576: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 577: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 578: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 579: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 580: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 581: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 582: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 583: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 584: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 585: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 586: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 587: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 588: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 589: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 590: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 591: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 592: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 593: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 594: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 595: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 596: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 597: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 598: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 599: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 600: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 601: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 602: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 603: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 604: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 605: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 606: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 607: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 608: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 609: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 610: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 611: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 612: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 613: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 614: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 615: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 616: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 617: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 618: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 619: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 620: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 621: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 622: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 623: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 624: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 625: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 626: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 627: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 628: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 629: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 630: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 631: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 632: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 633: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 634: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 635: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 636: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 637: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 638: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 639: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 640: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 641: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 642: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 643: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 644: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 645: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 646: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 647: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 648: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 649: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 650: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 651: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 652: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 653: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 654: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 655: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 656: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 657: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 658: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 659: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 660: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 661: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 662: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 663: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 664: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 665: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 666: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 667: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 668: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 669: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 670: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 671: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 672: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 673: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 674: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 675: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 676: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 677: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 678: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 679: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 680: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 681: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 682: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 683: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 684: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 685: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 686: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 687: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 688: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 689: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 690: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 691: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 692: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 693: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 694: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 695: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 696: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 697: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 698: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 699: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 700: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 701: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 702: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 703: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 704: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 705: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 706: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 707: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 708: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 709: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 710: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 711: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 712: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 713: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 714: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 715: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 716: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 717: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 718: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 719: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 720: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 721: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 722: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 723: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 724: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 725: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 726: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 727: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 728: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 729: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 730: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 731: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 732: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 733: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 734: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 735: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 736: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 737: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 738: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 739: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 740: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 741: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 742: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 743: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 744: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 745: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 746: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 747: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 748: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 749: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 750: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 751: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 752: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 753: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 754: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 755: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 756: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 757: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 758: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 759: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 760: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 761: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 762: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 763: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 764: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 765: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 766: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 767: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 768: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 769: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 770: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 771: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 772: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 773: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 774: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 775: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 776: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 777: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 778: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 779: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 780: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 781: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 782: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 783: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 784: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 785: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 786: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 787: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 788: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 789: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 790: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 791: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 792: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 793: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 794: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 795: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 796: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 797: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 798: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 799: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 800: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 801: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 802: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 803: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 804: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 805: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 806: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 807: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 808: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 809: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 810: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 811: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 812: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 813: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 814: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 815: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 816: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 817: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 818: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 819: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 820: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 821: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 822: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 823: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 824: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 825: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 826: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 827: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 828: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 829: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 830: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 831: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 832: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 833: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 834: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 835: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 836: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 837: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 838: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 839: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 840: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 841: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 842: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 843: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 844: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 845: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 846: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 847: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 848: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 849: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 850: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 851: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 852: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 853: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 854: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 855: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 856: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 857: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 858: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 859: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 860: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 861: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 862: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 863: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 864: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 865: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 866: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 867: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 868: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 869: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 870: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 871: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 872: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 873: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 874: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 875: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 876: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 877: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 878: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 879: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 880: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 881: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 882: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 883: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 884: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 885: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 886: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 887: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 888: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 889: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 890: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 891: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 892: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 893: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 894: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 895: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 896: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 897: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 898: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 899: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 900: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 901: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 902: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 903: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 904: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 905: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 906: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 907: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 908: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 909: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 910: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 911: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 912: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 913: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 914: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 915: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 916: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 917: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 918: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 919: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 920: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 921: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 922: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 923: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 924: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 925: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 926: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 927: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 928: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 929: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 930: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 931: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 932: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 933: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 934: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 935: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 936: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 937: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 938: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 939: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 940: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 941: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 942: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 943: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 944: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 945: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 946: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 947: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 948: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 949: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 950: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 951: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 952: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 953: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 954: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 955: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 956: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 957: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 958: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 959: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 960: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 961: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 962: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 963: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 964: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 965: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 966: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 967: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 968: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 969: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 970: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 971: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 972: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 973: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 974: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 975: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 976: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 977: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 978: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 979: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 980: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 981: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 982: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 983: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 984: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 985: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 986: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 987: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 988: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 989: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 990: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 991: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 992: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 993: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 994: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 995: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 996: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 997: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 998: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    case 999: o = oyAllocateWrapFunc_( len, allocateFunc ); break;
    default: o = oyAllocateWrapFunc_( len, allocateFunc );
  }

  if(!o) return 0;

#if OY_USE_OBJECT_POOL_
  if(old_obj == 0)
#endif
    error = !memset( o, 0, len );

  if(error)
    return NULL;
  
  o = oyObject_SetAllocators_( o, allocateFunc, deallocateFunc );
  o->copy = oyObject_Copy;
  o->release = oyObject_Release;
  o->ref_ = 1;
 
#if OY_USE_OBJECT_POOL_
  if(old_obj)
    return o;
#endif

  o->id_ = id;
  o->type_ = oyOBJECT_OBJECT_S;
  o->version_ = oyVersion(0);
  o->hash_ptr_ = 0;
  o->lock_ = NULL;
  o->parent_types_ = o->allocateFunc_(sizeof(oyOBJECT_e)*2);
  memset(o->parent_types_,0,sizeof(oyOBJECT_e)*2);

  if(oy_debug_objects >= 0 || oy_debug_objects <= -2)
    oyObject_Track(o, name);

  return o;
}

/** @brief   object management 
 *  @ingroup  objects_generic
 *
 *  @param[in]     object              the object
 *  @param[in]     name                a debug string containing the parent class
 *
 *  @version Oyranos: 0.9.7
 *  @since   2007/12/17 (Oyranos: 0.1.8)
 *  @date    2020/01/16
 */
oyObject_s   oyObject_NewFrom        ( oyObject_s          object,
                                       const char        * name )
{
  oyObject_s o = 0;
  int error = 0;

  if(object && object->type_ != oyOBJECT_OBJECT_S)
  {
    WARNc_S("Attempt to manipulate a non oyObject_s object.")
    return o;
  }

  if(object)
    o = oyObject_NewWithAllocators( object->allocateFunc_,
                                    object->deallocateFunc_,
                                    name );
  else
    o = oyObject_New( name );

  if(!o)
    error = 1;

  if(error <= 0 && object && object->name_)
    error = oyObject_SetNames( o, object->name_->nick, object->name_->name,
                               object->name_->description );

  if(error)
    WARNc1_S("error: %d", error )

  return o;
}

/** @brief   object management 
 *  @ingroup  objects_generic
 *
 *  @param[in]    object         the object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s
oyObject_Copy ( oyObject_s      object )
{
  if(!object)
    return 0;

  oyObject_Ref( object );

  return object;
}

/** @brief   release an Oyranos object
 *  @ingroup  objects_generic
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyObject_Release         ( oyObject_s      * obj )
{
  /* ---- start of common object destructor ----- */
  oyObject_s s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( s->type_ != oyOBJECT_OBJECT_S)
  {
    WARNc_S("Attempt to release a non oyObject_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s))
    return 0;
  /* ---- end of common object destructor ------- */

  oyName_release_( &s->name_, s->deallocateFunc_ );

#if OY_USE_OBJECT_POOL_
  {
  int i;
  for(i = 0; i < 100; ++i)
    if(oy_object_pool_[i] == 0)
    {
      if(s->hash_ptr_)
        memset( s->hash_ptr_, 0, OY_HASH_SIZE*2 );
      oy_object_pool_[i] = s;
      s->ref_ = 0;
      s->parent_ = 0;
      return 0;
    }
  }
#endif

  if(oy_debug_objects >= 0 || oy_debug_objects <= -2)
    oyObject_UnTrack( s );

  if(oy_debug_objects == -3 || (oy_debug_objects == s->id_ && getenv(OY_DEBUG_OBJECTS_PRINT_TREE))) /* animate the history of object releases */
  {
    char * text = NULL;
    OY_BACKTRACE_STRING(7)
    oyObjectTreePrint( 0x01 | 0x02 | 0x08, text ? text : __func__ );
    fprintf( stderr, "%s\n", text ? text : __func__ );
    oyFree_m_( text )
  }
  //s->id_ = 0;

  if(s->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->deallocateFunc_;
    oyPointer lock = s->lock_;

    if(s->hash_ptr_)
    { deallocateFunc(s->hash_ptr_); s->hash_ptr_ = 0; }

    if(s->parent_types_)
    { deallocateFunc( s->parent_types_ ); s->parent_types_ = 0; }

    if(s->backdoor_)
    { deallocateFunc( s->backdoor_ ); s->backdoor_ = 0; }

    if(s->handles_ && s->handles_->release)
    { s->handles_->release( (oyStruct_s**)&s->handles_ ); }

    deallocateFunc( s );
    if(lock)
      oyLockReleaseFunc_( lock, __FILE__, __LINE__ );
  }

  return 0;
}

/* } oyObject common object Functions */


/** @brief   custom object memory managers
 *  @ingroup  objects_generic
 *
 *  Normally a sub object should not know about its parent structure, but
 *  follow a hierarchical design. We break this here to allow the observation of
 *  all objects including the parents structures as a feature in oyObject_s.
 *  Then we just need a list of all oyObject_s objects and have an overview.
 *  This facility is intented to work even without debugging tools.
 *
 *  Each inheritent initialiser should call this function separately during
 *  normal object allocation and class initialisation.
 *
 *  @param[in]    o              the object to modify
 *  @param[in]    type           the parents struct type
 *  @param[in]    parent         a pointer to the parent struct
 *
 *  @version Oyranos: 0.3.0
 *  @date    2011/02/02
 *  @since   2007/11/00 (API 0.1.8)
 */
oyObject_s   oyObject_SetParent      ( oyObject_s        o,
                                       oyOBJECT_e        type,
                                       oyPointer         parent )
{
  int error = 0;
  if(error <= 0 && type)
  {
    oyOBJECT_e * tmp = 0;
    if(type != oyOBJECT_NONE)
    {
      tmp = o->allocateFunc_( sizeof(oyOBJECT_e) * (o->parent_types_[0] + 3 ));
      memset(tmp,0,sizeof(oyOBJECT_e) *  (o->parent_types_[0] + 3 ));
      memcpy(tmp, o->parent_types_, sizeof(oyOBJECT_e)*(o->parent_types_[0]+1));

      if(o->deallocateFunc_) o->deallocateFunc_( o->parent_types_ );
      o->parent_types_ = tmp;
      tmp = 0;

      o->parent_types_[0] += 1;
      o->parent_types_[o->parent_types_[0]] = type;
    }

    if(parent)
      o->parent_ = parent;
  }

  return o;
}

/** @brief   object naming
 *  @ingroup  objects_generic
 *
 *  @param[in]     object              the object
 *  @param[in]     text                the name to set
 *  @param[in]     type                the kind of name
 *  @return                            0 == success, > 0 error, < 0 issue
 *
 *  @version Oyranos: 0.3.0
 *  @date    2011/04/09
 *  @since   2007/11/00 (API 0.1.8)
 */
int          oyObject_SetName        ( oyObject_s          object,
                                       const char        * text,
                                       oyNAME_e            type )
{
  int error = 0;

  if( !object || object->type_ != oyOBJECT_OBJECT_S )
    return -1;

  if(type <= oyNAME_DESCRIPTION)
  {
    object->name_ = oyName_set_( object->name_, text, type,
                                 object->allocateFunc_, object->deallocateFunc_ );
    if(text)
      error = !object->name_;
  } else
  {
    char key[24];
    sprintf(key, "///oyNAME_s+%d", type );
    error = oyOptions_SetFromString( &object->handles_,
                                   key,
                                   text, OY_CREATE_NEW );
  }
  return !(type >= oyNAME_NAME && object && error <= 0);
}


/** @brief   object naming
 *  @ingroup  objects_generic
 *
 *  @param[in]    object         the object
 *  @param[in]    nick           short name, about 1-4 letters
 *  @param[in]    name           the name should fit into usual labels
 *  @param[in]    description    the description 
 *  @return                      0 == success, > 0 error, < 0 issue
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyObject_SetNames        ( oyObject_s        object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description )
{
  int error = 0;
  

  if(!object) return 0;

  if( object->type_ != oyOBJECT_OBJECT_S )
    return 0;

  if(error <= 0)
    error = oyObject_SetName( object, nick, oyNAME_NICK );
  if(error <= 0)
    error = oyObject_SetName( object, name, oyNAME_NAME );
  if(error <= 0)
    error = oyObject_SetName( object, description, oyNAME_DESCRIPTION );

  return error;
}

/** @brief   object naming
 *  @ingroup  objects_generic
 *
 *  @param[in]    dest           the object to modify
 *  @param[in]    src            the object to take names from
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int          oyObject_CopyNames       ( oyObject_s        dest,
                                        oyObject_s        src )
{
  int error = 0;

  if(!dest || !src) return 0;

  if( dest->type_ != oyOBJECT_OBJECT_S ||
      src->type_ != oyOBJECT_OBJECT_S )
    return 0;

  if(error <= 0)
    error = oyObject_SetNames( dest,
                               oyObject_GetName( src, oyNAME_NICK ),
                               oyObject_GetName( src, oyNAME_NAME ),
                               oyObject_GetName( src, oyNAME_DESCRIPTION ) );

  return error;
}

/** Function oyObject_GetName
 *  @ingroup  objects_generic
 *  @brief   object get names
 *
 *  @param[in]    obj            the object
 *  @param[in]    type           name type
 *  @return                      the text
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2007/11/00 (API 0.1.8)
 */
const oyChar * oyObject_GetName       ( const oyObject_s        obj,
                                        oyNAME_e                type )
{
  const char * text = 0;
  if(!obj)
    return 0;

  if( obj->type_ != oyOBJECT_OBJECT_S)
    return 0;

  if(type <= oyNAME_DESCRIPTION && !obj->name_)
    return 0;

  if(type <= oyNAME_DESCRIPTION)
    text = oyName_get_( obj->name_, type );
  else
  {
    char key[24];
    sprintf(key, "///oyNAME_s+%d", type );
    text = oyOptions_FindString( obj->handles_, key, NULL );
  }

  return text;
}

/** Function oyObject_Lock
 *  @ingroup  objects_generic
 *  @brief   Lock a object
 *
 *  @see   oyThreadLockingSet
 *
 *  @param[in]     object              the object
 *  @param[in]     marker              debug hints, e.g. __FILE__
 *  @param[in]     line                source line number, e.g. __LINE__
 *  @return                            error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/22 (API 0.1.8)
 */
int          oyObject_Lock           ( oyObject_s          object,
                                       const char        * marker,
                                       int                 line )
{
  int error = !object;

  if(error <= 0)
  {
    if( object->type_ != oyOBJECT_OBJECT_S)
      return 0;

    if(!object->lock_)
      object->lock_ = oyStruct_LockCreateFunc_( object->parent_ );

    oyLockFunc_( object->lock_, marker, line );
  }

  return error;
}

/** Function oyObject_UnLock
 *  @ingroup  objects_generic
 *  @brief   Unlock a object
 *
 *  @see   oyThreadLockingSet
 *
 *  @param[in]     object              the object
 *  @param[in]     marker              debug hints, e.g. __FILE__
 *  @param[in]     line                source line number, e.g. __LINE__
 *  @return                            error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/22 (API 0.1.8)
 */
int          oyObject_UnLock         ( oyObject_s          object,
                                       const char        * marker,
                                       int                 line )
{
  int error = !object;

  if(error <= 0)
  {
    if( object->type_ != oyOBJECT_OBJECT_S)
      return 0;

    error = !object->lock_;
    if(error <= 0)
      oyUnLockFunc_( object->lock_, marker, line );
  }

  return error;
}

/** Function oyObject_UnSetLocking
 *  @ingroup  objects_generic
 *  @brief   remove a object's lock pointer
 *
 *  @see     oyThreadLockingSet
 *
 *  @param[in]     object              the object
 *  @param[in]     marker              debug hints, e.g. __FILE__
 *  @param[in]     line                source line number, e.g. __LINE__
 *  @return                            error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/22 (API 0.1.8)
 */
int          oyObject_UnSetLocking   ( oyObject_s          object,
                                       const char        * marker,
                                       int                 line )
{
  int error = !object;

  if(error <= 0)
  {
    if( object->type_ != oyOBJECT_OBJECT_S)
      return 1;

    if(object->lock_)
      oyLockReleaseFunc_( object->lock_, marker, line );
  }

  return error;
}

/** Function oyObject_GetRefCount
 *  @ingroup  objects_generic
 *  @brief   get the identification number of a object 
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/19 (Oyranos: 0.1.10)
 *  @date    2009/02/19
 */
int            oyObject_GetRefCount  ( oyObject_s          obj )
{
  if( obj && obj->type_ != oyOBJECT_OBJECT_S)
    return -1;

  if(obj)
    return obj->ref_;

  return -1;
}

/**
 *  Function oyObject_UnRef
 *  @ingroup  objects_generic
 *  @brief   decrease the ref counter and return the above zero ref value
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/07
 *  @since   2008/02/07 (Oyranos: 0.1.8)
 */
int          oyObject_UnRef          ( oyObject_s          obj )
{
  int ref = 0;
  oyObject_s s = obj;
  int error = !s;

  if( !obj || s->type_ != oyOBJECT_OBJECT_S)
    return 1;

  if(error <= 0)
  {
    oyObject_Lock( s, __FILE__, __LINE__ );

    if(s->ref_ < 0 && (oy_debug_objects >= 0 || oy_debug_objects <= -2 || oy_debug))
      WARNc3_S( "%s[%d] refs: %d",
                oyStructTypeToText( s->parent_types_[s->parent_types_[0]] ),
                s->id_, s->ref_ )

    if(s->ref_ < 0)
      ref = 0;

    if(error <= 0 && --s->ref_ > 0)
      ref = s->ref_;

    if(oy_debug_objects == 1 || oy_debug_objects == s->id_)
      OY_BACKTRACE_PRINT
    if(oy_debug_objects >= 0 && s->id_ > 0)
      /* track object */
      oyObject_GetId( obj );

#   ifndef DEBUG_OBJECT
    if(s->ref_ < -1)
#   else
    if(s->id_ == 247)
#   endif
      WARNc3_S( "%s[%d] refs: %d",
                oyStructTypeToText( s->parent_types_[s->parent_types_[0]] ),
                s->id_, s->ref_ )

    if((uintptr_t)obj->parent_types_ < (uintptr_t)oyOBJECT_MAX)
    {
      WARNc1_S( "non plausible inheritance pointer: %s", 
                (s->id_ > 0)?oyStruct_GetInfo(obj,oyNAME_NAME,0):"----" );
      return -1;
    }

    if(obj->parent_types_[obj->parent_types_[0]] == oyOBJECT_NAMED_COLORS_S)
    {
      int e_a = error;
      error = pow(e_a,2.1);
      error = e_a;
    }

    oyObject_UnLock( s, __FILE__, __LINE__ );
  }

  return ref;
}

/** Function oyObject_GetAlloc
 *  @ingroup  objects_generic
 *  @brief   get objects allocator
 *
 *  @param         object              the object
 *  @return                            the allocator function
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/14 (Oyranos: 0.9.5)
 *  @date    2013/06/14
 */
OYAPI oyAlloc_f  OYEXPORT
                 oyObject_GetAlloc   ( oyObject_s        object )
{
  oyAlloc_f f = oyAllocateFunc_;
  if(object) f = object->allocateFunc_;
  return f;
}

/** Function oyObject_GetDeAlloc
 *  @ingroup  objects_generic
 *  @brief   get objects deallocator
 *
 *  @param         object              the object
 *  @return                            the deallocator function
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/14 (Oyranos: 0.9.5)
 *  @date    2013/06/14
 */
OYAPI oyDeAlloc_f  OYEXPORT
                 oyObject_GetDeAlloc ( oyObject_s        object )
{
  oyDeAlloc_f f = oyDeAllocateFunc_;
  if(object) f = object->deallocateFunc_;
  return f;
}

/* } Include "Object.public_methods_definitions.c" */


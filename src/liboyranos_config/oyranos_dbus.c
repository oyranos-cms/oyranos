/** @file oyranos_dbus.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2017-2024 (C) Kai-Uwe Behrmann
 *
 *  @brief    dbus dependent functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2017/09/27
 *
 *  The file contains some macros and boilerplate for DBus updates from 
 *  settings changes. A direct dependency in the Oyranos libraries can not
 *  be cross platform and thus is not desireable.
 *
 *  Defined are DBus callback and functions,
 *  oyJob_s callbacks: @code
    #include <oyranos_threads.h>
     oyWatchDBus_m( oyDBusFilter )
     oyFinishDBus_m
     int config_state_changed = 0;
     oyCallbackDBusCli_m( config_state_changed ) @endcode
 *  and
 *  some Boiler Plate to place before and to be called inside the used loop:
 *   ::oyStartDBusObserver, ::oyLoopDBusObserver
 */

#include "oyranos.h"
#include "oyranos_string.h"

#include <dbus/dbus.h>
#include <errno.h>
#include <stdio.h>
#include <oyjl.h>


/** ping to oyWatchDBus_m/oyDbusReceiveMessage_m
 */
void oyDBusSendPing( oySCOPE_e scope, const char * keyName )
{
  char * elkey = NULL; oyjlStringAdd( &elkey, 0,0, "%s/%s", scope==oySCOPE_SYSTEM ? "system" : "user", keyName );
  DBusError error;
  dbus_error_init (&error);
  if(oy_debug) fputs( oyjlBT(0), stderr );
  DBusConnection * connection = dbus_bus_get( scope==oySCOPE_SYSTEM ? DBUS_BUS_SYSTEM : DBUS_BUS_SESSION, &error );
  if(oy_debug) fputs( oyjlBT(0), stderr );
  DBusMessage * message = dbus_message_new_signal( "/org/libelektra/configuration"/*path*/, "org.libelektra"/*interfacce*/, "Commit" );
  if(oy_debug) fputs( oyjlBT(0), stderr );
  if (connection == NULL || dbus_error_is_set(&error))
  {
    fputs( oyjlBT(0), stderr );
    fprintf (stderr, "Failed to open connection to \"%s\" message bus: %s\n", (scope == oySCOPE_SYSTEM) ? "system" : "session",
             error.message?error.message:"----");
    if(connection) dbus_error_free( &error );
    goto oyDBusSendPing_clean;
  }
  if(oy_debug) fputs( oyjlBT(0), stderr );

  dbus_message_append_args( message, DBUS_TYPE_STRING, &elkey, DBUS_TYPE_INVALID );
  dbus_connection_send( connection, message, NULL );
  dbus_message_unref( message );
  if(oy_debug) fprintf( stderr, "send DBus: %s\n", oyNoEmptyString_m_(elkey) );
  dbus_connection_unref( connection );
  dbus_error_free (&error);
oyDBusSendPing_clean:
  if(elkey) free(elkey);
}


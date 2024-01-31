/**
 +  @file     oyranos_dbus_macros.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    DBus helper macros
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
 *   ::oyStartDBusObserver, ::oyLoopDBusObserver */


#ifndef OYRANOS_DBUS_MACROS_H
#define OYRANOS_DBUS_MACROS_H

#include "oyranos.h"

#include <dbus/dbus.h>
#include <unistd.h> /* usleep() */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	
/* --- DBus callbacks --- */

/** DBus filter function */
#define oyDBusFilter_m \
DBusHandlerResult  oyDBusFilter      ( DBusConnection    * connection OY_UNUSED, \
                                       DBusMessage       * message, \
                                       void              * user_data ) \
{ \
  DBusMessageIter iter; \
  int arg_type; \
  char * value = NULL; \
  oyJob_s * job = (oyJob_s*)user_data; \
  const char * tmp = NULL; \
  char * hit = NULL; \
 \
  dbus_message_iter_init( message, &iter ); \
  arg_type = dbus_message_iter_get_arg_type( &iter ); \
  if(arg_type == DBUS_TYPE_STRING) \
    dbus_message_iter_get_basic( &iter, &value ); \
  if(job && job->context) \
    tmp = oyOption_GetRegistration( (oyOption_s*)job->context ); \
  if(tmp && value) \
    hit = strstr( value, tmp ); \
  if(hit) \
  { \
    value = oyStringCopy( value, 0 ); \
    /* send a message */ \
    oyMsg_Add( job, 0.5, &value ); \
  } \
  \
  if(dbus_message_is_signal( message, DBUS_INTERFACE_LOCAL, "Disconnected" )) \
  { WARNc_S("obtained unexpected DBus Signal"); \
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED; } \
  \
  return DBUS_HANDLER_RESULT_HANDLED; \
}

/** borrowed from libelektra project receivemessage.c
 * BSD License */
#define oyDbusReceiveMessage_m \
oySleep_m \
int oyDbusReceiveMessage (DBusBusType type, DBusHandleMessageFunction filter_func, void * user_data) \
{ \
  DBusConnection * connection; \
  DBusError error; \
 \
  dbus_error_init (&error); \
  connection = dbus_bus_get (type, &error); \
  if (connection == NULL || dbus_error_is_set(&error)) \
  { \
    fprintf (stderr, "Failed to open connection to %s message bus: %s\n", (type == DBUS_BUS_SYSTEM) ? "system" : "session", \
             error.message); \
    dbus_error_free( &error ); \
    goto oyDbusReceiveMessage_error; \
  } \
 \
  dbus_bus_add_match (connection, "type='signal',interface='org.libelektra',path='/org/libelektra/configuration',member=Commit", &error); \
  if (dbus_error_is_set (&error)) goto oyDbusReceiveMessage_error; \
 \
  if (!dbus_connection_add_filter (connection, filter_func, user_data, NULL)) \
  { \
    goto oyDbusReceiveMessage_error; \
  } \
 \
  while(1) \
  { \
    int r;\
    dbus_connection_read_write(connection, 0); \
    if((r = dbus_connection_dispatch(connection)) != DBUS_DISPATCH_DATA_REMAINS) \
      oySleep(0.25); \
  } \
 \
  return 0; \
 \
oyDbusReceiveMessage_error: \
  printf ("Error occurred\n"); \
  dbus_error_free (&error); \
  return -1; \
}
#define oySleep_m \
static void        oySleep           ( double              seconds ) \
{ \
  usleep((useconds_t)(seconds*(double)1000000)); \
}

void oyDBusSendPing                  ( oySCOPE_e           scope,
                                       const char        * keyName );

/* --- oyJob_s callbacks --- */

/** covers oyDbusReceiveMessage_m
 *  oyJob_s::work for the processing thread
 */
#define oyWatchDBus_m( filter_function ) \
oyDbusReceiveMessage_m \
static int oyWatchDBus( oyJob_s * job ) \
{ \
  char * t = NULL; \
  const char * tmp = oyOption_GetRegistration((oyOption_s*)job->context); \
  if(job->cb_progress) \
  { \
    if(tmp) \
    { \
      t = (char*) malloc(80+strlen(__func__)+strlen(tmp)); \
      sprintf( t, "%s():%d --->\n%s", __func__,__LINE__, tmp ); \
    } \
    /* send a first message from inside the worker thread */ \
    oyMsg_Add(job, .1, &t); \
    oyDbusReceiveMessage(DBUS_BUS_SESSION, filter_function, job); \
  } \
  return 0; \
}
/** oyJob_s::finish for the main thread
 */
#define oyFinishDBus_m \
static int oyFinishDBus( oyJob_s * job ) \
{ \
  char * t = NULL; \
  const char * tmp = oyOption_GetRegistration((oyOption_s*)job->context); \
  if(job->cb_progress) \
  { \
    if(tmp) \
    { \
      t = (char*) malloc(80+strlen(__func__)+strlen(tmp)); \
      sprintf( t, "%s():%d --->\n%s\n<--- finished", __func__,__LINE__, tmp ); \
    } \
    /* send a final message from inside the managing thread */ \
    oyMsg_Add(job, 0.9, &t); \
  } \
  return 0; \
}
/** oyJob_s::cb_progress for the main thread;
 *
 *  will be called inside
 *  oyJobResult()
 */
#define oyCallbackDBusCli_m(check_var) \
static void oyCallbackDBus           ( double              progress_zero_till_one, \
                                       char              * status_text, \
                                       int                 thread_id_, \
                                       int                 job_id, \
                                       oyStruct_s        * cb_progress_context OY_UNUSED ) \
{ fprintf( stderr,"%s():%d %02f %s %d/%d\n",__func__,__LINE__, \
           progress_zero_till_one, \
           status_text?status_text:"",thread_id_,job_id); \
  /* Clear the changed state, before a new check. */ \
  check_var = 1; \
}


/* --- Boiler Plate --- */

/** setup the thread running the DBus observer
 *  simple version:
    oyStartDBusObserver( oyWatchDBus, oyFinishDBus, oyCallbackDBus, OY_STD )
 */
#define oyStartDBusObserver( watch_, finish_, callback_, key_fragment, cb_context_ ) \
  /* use asynchron DBus observation \
   * It is unknown when DBus message stream ends. Our DBus observer needs to \
   * collect all messages and sends one update event after a reasonable time. \
   */ \
  oyJob_s * job = oyJob_New( NULL ); \
  /* This observer function keeps alive during the process run. */ \
  job->work = watch_; \
  /* Just a informational callback in case DBus quits. */ \
  job->finish = finish_; \
  oyOption_s * o = oyOption_FromRegistration( key_fragment, NULL ); \
  oyOption_SetFromString( o, key_fragment, 0 ); \
  job->context = (oyStruct_s*)o; \
  /* The callback informs about DBus events now from inside the main thread. \
   * Here we should set a update state. */ \
  job->cb_progress = callback_; \
  job->cb_progress_context = (oyStruct_s*)cb_context_; \
  id = oyJob_Add( &job, 0, oyJOB_ADD_PERSISTENT_JOB );

/** poll for updates 
 *  double hour: current time
 *  double repeat_hour: repeat a full update in h
 *  int check_var: 1 means check, 0 means no check needed
 *  function update: will be called, when check_var is 1 */
#define oyLoopDBusObserver( hour, repeat_hour, check_var, update ) \
    double hour_diff = hour_old - hour; \
 \
    if(hour_diff < 0.0) \
      hour_diff += 24.0; \
    if(hour_diff > 12) \
      hour_diff = 24.0 - hour_diff; \
 \
    /* Here in the main/managing thread we wait for a changed state. */ \
    oyJobResult(); \
 \
    if(check_var || hour_diff > repeat_hour ) \
    /* check the sunrise / sunset state */ \
    { \
      error = update; \
      hour_old = hour; \
    } \
 \
    /* Clear the changed state, before a new check. */ \
    check_var = 0; 


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_DBUS_MACROS_H */

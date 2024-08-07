/** @file oyjl_args_web.c
 *
 *  Oyjl Args Web is a http server renderer of UI files.
 *
 *  @par Copyright:
 *            2021-2023 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2021/07/04
 *
 *  basic Microserver handling for the -R=web option
 */

// cc -Wall -Wextra -g -O0 mhd-largepost.c -o mhd-largepost -L /home/kuwe/.local/lib64/ -lmicrohttpd-static -lgnutls -lpthread
/* Feel free to use this example code in any way
   you see fit (Public Domain) */

#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include "oyjl.h"
#include "oyjl_debug.h"
#include "oyjl_io.h"
#include "oyjl_macros.h"
#include "oyjl_i18n_internal.h"
#include "oyjl_tree_internal.h" /* oyjl_debug */
#include "oyjl_version.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h> /* setlocale LC_NUMERIC */
#endif

int oyjlArgsWebFileNameSecurity      ( const char       ** full_filename,
                                       int                 write_size );

#ifdef _MSC_VER
#ifndef strcasecmp
#define strcasecmp(a,b) _stricmp((a),(b))
#endif /* !strcasecmp */
#endif /* _MSC_VER */

#if defined(_MSC_VER) && _MSC_VER+0 <= 1800
/* Substitution is OK while return value is not used */
#define snprintf _snprintf
#endif

#define PORT            8888
#define POSTBUFFERSIZE  512
#define MAXCLIENTS      2

enum ConnectionType
  {
    GET = 0,
    POST = 1
  };

static unsigned int oyjl_nr_of_uploading_clients = 0;
static const char * oyjl_args_web_rexexp = "((([a-z]+://)?[a-zA-Z0-9-]+\\.[a-zA-Z0-9-]+\\.[a-zA-Z0-9-]+)|([a-z]+://)+[a-zA-Z0-9-]+\\.[a-zA-Z0-9-]+)(:[0-9]{1,5})?([/a-zA-Z0-9+-.?=%_*]*)?";
static const char * oyjl_args_web_replacement = "<a href=\"%s\">%s</a>";
char * oyjlStringLinkify( const char * html );

typedef enum {
  oyjlSECURITY_READONLY,
  oyjlSECURITY_INTERACTIVE,
  oyjlSECURITY_CHECK,          /* check read and write file i/o */
  oyjlSECURITY_CHECK_READ,     /* check read file input */
  oyjlSECURITY_CHECK_WRITE,    /* check write file output */
  oyjlSECURITY_LAZY
} oyjlSECURITY_e;

struct oyjl_mhd_context_struct
{
  char * html;
  oyjlSECURITY_e sec;
  char * path;
  int (*callback)(int argc, const char ** argv);
  const char * tool_name;
  int debug;
  const char * ignore;
  const char * css;
  const char * css2;
};

/**
 * Information we keep per connection.
 */
struct oyjl_mhd_connection_info_struct
{
  enum ConnectionType connectiontype;

  /**
   * Handle to the POST processing state.
   */
  struct MHD_PostProcessor *postprocessor;

  /**
   * File handle where we write uploaded data.
   */
  FILE *fp;

  /**
   * HTTP response body we will return, NULL if not yet known.
   */
  const char *answerstring;

  /**
   * HTTP status code we will return, 0 for undecided.
   */
  unsigned int answercode;

  /** Oyjl tree */
  oyjl_val answernode;

  oyjlSECURITY_e sec;

  /* part after the TLD url */
  const char * path;

  /* arguments for the tool */
  char * command;
};


const char *askpage = "<html>\n<head><title>%s</title></head>\n<body>\n\
                       Upload a file, please!<br>\n\
                       There are %u clients uploading at the moment.<br>\n\
                       <form action=\"/result\" method=\"post\" enctype=\"multipart/form-data\">\n\
                       <input type=\"text\" name=\"textParam\" value=\"textValue\"/><br/>\n\
                       <input type=\"checkbox\" name=\"checkboxParam\" id=\"checkboxParam\" value=\"true\" checked=\"true\"/><label for=\"checkboxParam\">Checkbox</label><br/>\n\
                       <input name=\"file\" type=\"file\">\n\
                       <input type=\"submit\" value=\" Send \"></form>\n\
                       </body></html>";
const char *busypage =
  "<html><body>This server is busy, please try again later.</body></html>";
const char *completepage =
  "<html><body>The upload has been completed.</body></html>";
const char *errorpage =
  "<html><body>This doesn't seem to be right.</body></html>";
const char *servererrorpage =
  "<html><body>Invalid request.</body></html>";
const char *fileexistspage =
  "<html><body>This file already exists.</body></html>";
const char *fileioerror =
  "<html><body>IO error writing to disk.</body></html>";
const char* const postprocerror =
  "<html><head><title>Error</title></head><body>Error processing POST data</body></html>";
const char *responsepage =
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\">\n\
<head>\n\
  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\
  <meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n\
  <style type=\"text/css\">%s%s%scode{white-space: pre;}\n\
  </style>\n\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
</head><body>\n\
%s</body></html>";

#define OYJL_PNG                       0x200
#define OYJL_SVG                       0x400
static int oyjlMhdSendPage           ( struct MHD_Connection * connection,
                                       const char        * page,
                                       int                 size,
                                       int                 format,
                                       int                 status_code )
{
  int ret;
  struct MHD_Response * response;
  char * t = oyjlBT(0);
  const char * content_type = "text/html";

  switch(format)
  {
    case OYJL_PNG: content_type = "image/png"; break;
    case OYJL_XML: content_type = "text/xml"; break;
  }

  response = MHD_create_response_from_buffer( size, (void*)page, MHD_RESPMEM_MUST_COPY );
  if(!response)
    return MHD_NO;
  MHD_add_response_header( response,
                           MHD_HTTP_HEADER_CONTENT_TYPE,
                           content_type );
  ret = MHD_queue_response( connection,
                            status_code,
                            response );
  MHD_destroy_response( response );

  //fprintf(stderr, "%ssize:%d: page[%s]:%s status_code: %d\n", t, size, content_type, oyjlTermColorFromHtml(page,0), status_code );
  free(t);
  return ret;
}


static enum MHD_Result oyjlMhdIteratePost_cb(
                                       void              * coninfo_cls,
                                       enum MHD_ValueKind  kind,
                                       const char        * key,
                                       const char        * filename,
                                       const char        * content_type,
                                       const char        * transfer_encoding,
                                       const char        * data,
                                       uint64_t            off,
                                       size_t              size)
{
  struct oyjl_mhd_connection_info_struct *con_info = coninfo_cls;
  FILE *fp;
  (void)kind;               /* Unused. Silent compiler warning. */
  (void)content_type;       /* Unused. Silent compiler warning. */
  (void)transfer_encoding;  /* Unused. Silent compiler warning. */
  (void)off;                /* Unused. Silent compiler warning. */
  char * text = NULL;

  if(size)
  {
    text = (char*)calloc( size+1, sizeof(char) );
    memcpy( text, data, size );
  }

  fprintf(stderr,"key:%s filename:%s content_type:%s data:%s size:%lu\n", key, OYJL_E(filename,""), OYJL_E(content_type,""), OYJL_E(text,""), size );
  if(size == 0)
    return MHD_YES;

  if(!con_info->answernode) con_info->answernode = oyjlTreeNew("");

  if(0 != strcmp (key, "file"))
  {
    con_info->answerstring = servererrorpage;
    con_info->answercode = MHD_HTTP_BAD_REQUEST;
    if(text && strchr(text,';'))
    {
      int i, n = 0;
      char ** list = oyjlStringSplit2( text, ";", 0, &n, NULL, malloc );
      for(i = 0; i < n; ++i)
      {
        const char * arg = list[i];
        fprintf(stderr, OYJL_DBG_FORMAT "key[%d]:%s \n", OYJL_DBG_ARGS, i, arg );
        oyjlTreeSetStringF( con_info->answernode, OYJL_CREATE_NEW, arg, "%s/[%d]", key, i );
      }
      oyjlStringListRelease( &list, n, free );
    }
    else
      oyjlTreeSetStringF( con_info->answernode, OYJL_CREATE_NEW, text, key );
    if(text) free(text);
    return MHD_YES;
  } else
  {
    oyjlTreeSetStringF( con_info->answernode, OYJL_CREATE_NEW, text, "%s/data", key );
    oyjlTreeSetStringF( con_info->answernode, OYJL_CREATE_NEW, filename, "%s/filename", key );
  }

  if(!oyjlArgsWebFileNameSecurity(&filename, 0))
  if(! con_info->fp && filename)
  {
    if(0 != con_info->answercode) /* something went wrong */
      return MHD_YES;
    if(NULL != (fp = fopen (filename, "rb")))
    {
      fclose (fp);
      con_info->answerstring = fileexistspage;
      con_info->answercode = MHD_HTTP_FORBIDDEN;
      return MHD_YES;
    }
    /* NOTE: This is technically a race with the 'fopen()' above,
       but there is no easy fix, short of moving to open(O_EXCL)
       instead of using fopen(). For the example, we do not care. */
    con_info->fp = oyjlFopen (filename, "ab");
    if(!con_info->fp)
    {
      con_info->answerstring = fileioerror;
      con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
      return MHD_YES;
    }
  }

  if(size > 0)
  {
    if(! fwrite (text, sizeof (char), size, con_info->fp))
    {
      con_info->answerstring = fileioerror;
      con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
      if(text) free(text);
      return MHD_YES;
    }
  }

  if(text) free(text);

  return MHD_YES;
}


static void
oyjlMhdRequestCompleted_cb           ( void              * cls,
                                       struct MHD_Connection * connection,
                                       void             ** con_cls,
                                       enum MHD_RequestTerminationCode toe )
{
  struct oyjl_mhd_connection_info_struct *con_info = *con_cls;
  (void)cls;         /* Unused. Silent compiler warning. */
  (void)connection;  /* Unused. Silent compiler warning. */
  (void)toe;         /* Unused. Silent compiler warning. */

  if(NULL == con_info)
    return;

  if (con_info->connectiontype == POST)
  {
    if(NULL != con_info->postprocessor)
    {
      MHD_destroy_post_processor (con_info->postprocessor);
      oyjl_nr_of_uploading_clients--;
    }

    if(con_info->fp)
      fclose (con_info->fp);
  }

  if(con_info->command) free(con_info->command);
  free (con_info);
  *con_cls = NULL;
}

char * oyjlStringLinkify( const char * html )
{
  char * t = oyjlStringCopy( html, 0 );
  oyjlRegExpReplace( &t, oyjl_args_web_rexexp, oyjl_args_web_replacement );
  oyjlStringReplace( &t, "<a href=\"www.", "<a href=\"http://www.", 0,0 );
  return t;
}


static enum MHD_Result oyjlMhdAnswerToConnection_cb (
                                       void *cls,
                      struct MHD_Connection *connection,
                      const char *url,
                      const char *method,
                      const char *version,
                      const char *upload_data,
                      size_t *upload_data_size,
                      void **con_cls)
{
  (void)version;           /* Unused. Silent compiler warning. */

  struct oyjl_mhd_context_struct * context = (struct oyjl_mhd_context_struct*)cls;
  int format = OYJL_HTML;

  if(!context)
    oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, 0, OYJL_DBG_FORMAT "OyjlArgsWeb option not found: context", OYJL_DBG_ARGS );
  if(!context->callback)
    oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, 0, OYJL_DBG_FORMAT "OyjlArgsWeb option not found: context->callback", OYJL_DBG_ARGS );

  if (NULL == *con_cls)
  {
    /* First call, setup data structures */
    struct oyjl_mhd_connection_info_struct *con_info;

    fprintf(stderr,"url:%s method:%s version:%s upload_data_size:%ln\n", OYJL_E(url,""), OYJL_E(method,""), OYJL_E(version,""), upload_data_size );

    if(oyjl_nr_of_uploading_clients >= MAXCLIENTS)
      return oyjlMhdSendPage (connection,
                        busypage, strlen(busypage), format,
                        MHD_HTTP_SERVICE_UNAVAILABLE);

    con_info = calloc (sizeof (struct oyjl_mhd_connection_info_struct), 1);
    if(NULL == con_info) return MHD_NO;

    con_info->answercode = 0; /* none yet */
    con_info->fp = NULL;
    con_info->path = context->path;
    con_info->sec = context->sec;

    if(0 == strcasecmp (method, MHD_HTTP_METHOD_POST))
    {
      con_info->postprocessor =
        MHD_create_post_processor (connection,
                                   POSTBUFFERSIZE,
                                   &oyjlMhdIteratePost_cb,
                                   (void *) con_info);

      if (NULL == con_info->postprocessor)
      {
        free (con_info);
        return MHD_NO;
      }

      oyjl_nr_of_uploading_clients++;

      con_info->connectiontype = POST;
    }
    else
    {
      con_info->connectiontype = GET;
    }

    *con_cls = (void *) con_info;

    return MHD_YES;
  }

  if(0 == strcasecmp (method, MHD_HTTP_METHOD_GET))
  {
    /* We just return the standard form for uploads on all GET requests */
    char buffer[1024];
    const char * title = url;

    snprintf (buffer,
              sizeof (buffer),
              askpage,
              title,
              oyjl_nr_of_uploading_clients);

    return oyjlMhdSendPage (connection,
                      context->html, strlen(context->html), format,
                      MHD_HTTP_OK);
  }

  if(0 == strcasecmp (method, MHD_HTTP_METHOD_POST))
  {
    struct oyjl_mhd_connection_info_struct *con_info = *con_cls;

    if(0 != *upload_data_size)
    {
      /* Upload not yet done */
      if(0 != con_info->answercode)
      {
        /* we already know the answer, skip rest of upload */
        *upload_data_size = 0;
        return MHD_YES;
      }

      if(MHD_YES !=
          MHD_post_process (con_info->postprocessor,
                            upload_data,
                            *upload_data_size))
      {
        con_info->answerstring = postprocerror;
        con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
      }
      *upload_data_size = 0;

      return MHD_YES;
    }
    /* Upload finished */
    if(NULL != con_info->fp)
    {
      fclose (con_info->fp);
      con_info->fp = NULL;
    }

    if(0 == con_info->answercode)
    {
      /* No errors encountered, declare success */
      con_info->answerstring = completepage;
      con_info->answercode = MHD_HTTP_OK;
    }

    if(oyjlValueCount(con_info->answernode))
    {
      int ret = 0;
      int n = oyjlValueCount(con_info->answernode), argc = 0, i;
      char ** argv = NULL;
      char * ignore = context->ignore ? oyjlStringCopy( context->ignore, 0 ) : NULL;
      con_info->answerstring = NULL;
      fprintf( stderr, "%s prepare", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
      oyjlStringListPush( &argv, &argc, context->tool_name, 0,0 );
      for(i = 1; i < n+1; ++i)
      {
        oyjl_val o = oyjlTreeGetValueF(con_info->answernode, 0, "[%d]", i-1);
        char * path = oyjlTreeGetPath( con_info->answernode, o );
        const char * txt = OYJL_GET_STRING(o);
        int is_bool = txt && (strcmp(txt,"true") == 0 || strcmp(txt,"true_no_dash") == 0);
        int is_no_dash = txt && strcmp(txt,"true_no_dash") == 0;
        int count = oyjlValueCount( o );
        //fprintf(stderr, " count: %d %s %s ", count, OYJL_E(path,"----"), o?"opt":"");
        if(count)
        {
          int j;
          for(j = 0; j < count; ++j)
          {
            char * t;
            o = oyjlTreeGetValueF(con_info->answernode, 0, "[%d]/[%d]", i-1, j);
            path = oyjlTreeGetPath( con_info->answernode, o );
            t = strchr(path,'/');
            if(t)
              t[0] = '\000';
            txt = OYJL_GET_STRING(o);
            if(!(oyjlStringSplitFind(ignore, ",", path, 0, NULL, 0,0) >= 0))
              oyjlStringListAdd( &argv, &argc, 0,0, "%s%s%s%s", is_no_dash?"":strlen(path) == 1?"-":"--", path, is_bool || is_no_dash?"":"=", is_bool?"":txt );
            //fprintf(stderr, " path: %s arg: %s argv[%d]: %s ", path, txt, argc, argv[argc-1]);
            if(path) { free(path); path = NULL; }
          }
        }
        else if(path && !(oyjlStringSplitFind(ignore, ",", path, 0, NULL, 0,0) >= 0))
          oyjlStringListAdd( &argv, &argc, 0,0, "%s%s%s%s", is_no_dash?"":strlen(path) == 1?"-":"--", path, is_bool || is_no_dash?"":"=", is_bool?"":txt );

        if(path) free(path);
      }

      fprintf(stderr, "[%d]: ", argc );
      for(i = 0; i < argc; ++i)
        oyjlStringAdd( &con_info->command, 0,0, "%s ", argv[i] );
      fputs( con_info->command, stderr );
      fputs( "\n", stderr );

      if(con_info->sec >= oyjlSECURITY_CHECK)
      {
        int size_stdout = 0, size_stderr = 0;
        char * data_stdout = NULL, * data_stderr = NULL;
        const char * html;
        char * html_tmp = NULL,
             * html_commented = NULL;
        int data_format = 0;

        int result = oyjlReadFunction( argc, (const char **)argv, context->callback, malloc, &size_stdout, &data_stdout, &size_stderr, &data_stderr );
        int size = size_stdout?size_stdout:size_stderr;
        html = size_stdout?data_stdout:data_stderr;
        if(html) html_commented = oyjlStringCopy( html, 0 );
        data_format = oyjlDataFormat(html);
        if(result || (!size_stdout && !size_stderr) || context->debug)
          fprintf(stderr, "returned: %d size_stdout: %d size_stderr: %d format: %s[%d]\n", result, size_stdout, size_stderr, oyjlDataFormatToString(data_format), data_format );

        if(data_format == 8)
        {
          int len = strlen(html);
          if((len > 32 && oyjlStringStartsWith( html, "<?xml version=\"1.0\"", OYJL_COMPARE_CASE ) && strstr( html, "<svg" ) ) ||
             (len > 32 && oyjlStringStartsWith( html, "<svg", OYJL_COMPARE_CASE) == 1))
          {
            char * css_toc_text = NULL;
            oyjlStringAdd( &html_commented, 0,0, "<br />%s", con_info->command );
            oyjlStringAdd( &html_tmp, 0,0, responsepage, OYJL_E(context->css,""), OYJL_E(context->css2,""), OYJL_E(css_toc_text,""),
                           html_commented );
            html = html_tmp;
            format = OYJL_SVG;
          }
          else
            format = OYJL_XML;
        }
        else
        {
          // test for PNG
          if(size > 16 && (unsigned char)html[0] == 137 && (char)html[1] == 80 && (char)html[2] == 78 && (char)html[3] == 71 && (char)html[4] == 13 && (char)html[5] == 10 && (char)html[6] == 26 && (char)html[7] == 10)
          {
            format = OYJL_PNG;
            fprintf(stderr,  "%s oyjlMhdSendPage: found png of size: %d\n", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN), size );
          }
          else if(data_format != 8)
          {
            char * css_toc_text = NULL;
            int is_rich = size && (html_commented[0] == '<' ||
                                   strstr(html_commented, "\033[") != NULL);
            char * t = oyjlStringLinkify( is_rich ? oyjlTermColorToHtml( html_commented, 0 ) : html_commented ),
                 * tmp = NULL;
            if(!is_rich)
              oyjlStringAdd( &tmp, 0,0, "<code>%s</code>", t );

            oyjlStringAdd( &html_tmp, 0,0, responsepage, OYJL_E(context->css,""), OYJL_E(context->css2,""), OYJL_E(css_toc_text,""),
                           tmp?tmp:t );
            if(tmp) free(tmp);
            free(t);
            html = html_tmp;
          }
        }
        con_info->answerstring = html;
        con_info->answercode = MHD_HTTP_OK;
        if(con_info->answerstring)
        {
          ret = oyjlMhdSendPage( connection, con_info->answerstring, html_tmp?(int)strlen(con_info->answerstring):size,
                                 format, con_info->answercode );
          fprintf(stderr,  "%s oyjlMhdSendPage: %d\n", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN), ret );
        }
        if(data_stdout) free(data_stdout);
        if(data_stderr) free(data_stderr);
        if(html_commented) free(html_commented);
        if(html_tmp) free(html_tmp);
      }
      else
        fprintf( stderr,  "no run\n" );

      oyjlStringListRelease( &argv, argc, free );

      if(!con_info->answerstring)
      {
        char * answerstring = oyjlTreeToText( con_info->answernode, OYJL_JSON );
        con_info->answerstring = oyjlTermColorToHtml( answerstring, 0 );
        free(answerstring); answerstring = NULL;
        ret = oyjlMhdSendPage( connection, con_info->answerstring, strlen(con_info->answerstring),
                               format, con_info->answercode );
        fprintf(stderr,  "%s oyjlMhdSendPage: %d\n", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN), ret );
      }

      oyjlTreeFree( con_info->answernode ); con_info->answernode = NULL;
      return ret;
      
    } else
      return oyjlMhdSendPage(connection,
                        con_info->answerstring, strlen(con_info->answerstring), format,
                        con_info->answercode);
  }

  /* Note a GET or a POST, generate error */
  return oyjlMhdSendPage(connection,
                    errorpage, strlen(errorpage), format,
                    MHD_HTTP_BAD_REQUEST);
}

/* test server key */
const char srv_signed_key_pem[] = "-----BEGIN RSA PRIVATE KEY-----\n"
                                  "MIIEowIBAAKCAQEAvfTdv+3fgvVTKRnP/HVNG81cr8TrUP/iiyuve/THMzvFXhCW\n"
                                  "+K03KwEku55QvnUndwBfU/ROzLlv+5hotgiDRNFT3HxurmhouySBrJNJv7qWp8IL\n"
                                  "q4sw32vo0fbMu5BZF49bUXK9L3kW2PdhTtSQPWHEzNrCxO+YgCilKHkY3vQNfdJ0\n"
                                  "20Q5EAAEseD1YtWCIpRvJzYlZMpjYB1ubTl24kwrgOKUJYKqM4jmF4DVQp4oOK/6\n"
                                  "QYGGh1QmHRPAy3CBII6sbb+sZT9cAqU6GYQVB35lm4XAgibXV6KgmpVxVQQ69U6x\n"
                                  "yoOl204xuekZOaG9RUPId74Rtmwfi1TLbBzo2wIDAQABAoIBADu09WSICNq5cMe4\n"
                                  "+NKCLlgAT1NiQpLls1gKRbDhKiHU9j8QWNvWWkJWrCya4QdUfLCfeddCMeiQmv3K\n"
                                  "lJMvDs+5OjJSHFoOsGiuW2Ias7IjnIojaJalfBml6frhJ84G27IXmdz6gzOiTIer\n"
                                  "DjeAgcwBaKH5WwIay2TxIaScl7AwHBauQkrLcyb4hTmZuQh6ArVIN6+pzoVuORXM\n"
                                  "bpeNWl2l/HSN3VtUN6aCAKbN/X3o0GavCCMn5Fa85uJFsab4ss/uP+2PusU71+zP\n"
                                  "sBm6p/2IbGvF5k3VPDA7X5YX61sukRjRBihY8xSnNYx1UcoOsX6AiPnbhifD8+xQ\n"
                                  "Tlf8oJUCgYEA0BTfzqNpr9Wxw5/QXaSdw7S/0eP5a0C/nwURvmfSzuTD4equzbEN\n"
                                  "d+dI/s2JMxrdj/I4uoAfUXRGaabevQIjFzC9uyE3LaOyR2zhuvAzX+vVcs6bSXeU\n"
                                  "pKpCAcN+3Z3evMaX2f+z/nfSUAl2i4J2R+/LQAWJW4KwRky/m+cxpfUCgYEA6bN1\n"
                                  "b73bMgM8wpNt6+fcmS+5n0iZihygQ2U2DEud8nZJL4Nrm1dwTnfZfJBnkGj6+0Q0\n"
                                  "cOwj2KS0/wcEdJBP0jucU4v60VMhp75AQeHqidIde0bTViSRo3HWKXHBIFGYoU3T\n"
                                  "LyPyKndbqsOObnsFXHn56Nwhr2HLf6nw4taGQY8CgYBoSW36FLCNbd6QGvLFXBGt\n"
                                  "2lMhEM8az/K58kJ4WXSwOLtr6MD/WjNT2tkcy0puEJLm6BFCd6A6pLn9jaKou/92\n"
                                  "SfltZjJPb3GUlp9zn5tAAeSSi7YMViBrfuFiHObij5LorefBXISLjuYbMwL03MgH\n"
                                  "Ocl2JtA2ywMp2KFXs8GQWQKBgFyIVv5ogQrbZ0pvj31xr9HjqK6d01VxIi+tOmpB\n"
                                  "4ocnOLEcaxX12BzprW55ytfOCVpF1jHD/imAhb3YrHXu0fwe6DXYXfZV4SSG2vB7\n"
                                  "IB9z14KBN5qLHjNGFpMQXHSMek+b/ftTU0ZnPh9uEM5D3YqRLVd7GcdUhHvG8P8Q\n"
                                  "C9aXAoGBAJtID6h8wOGMP0XYX5YYnhlC7dOLfk8UYrzlp3xhqVkzKthTQTj6wx9R\n"
                                  "GtC4k7U1ki8oJsfcIlBNXd768fqDVWjYju5rzShMpo8OCTS6ipAblKjCxPPVhIpv\n"
                                  "tWPlbSn1qj6wylstJ5/3Z+ZW5H4wIKp5jmLiioDhcP0L/Ex3Zx8O\n"
                                  "-----END RSA PRIVATE KEY-----\n";

/* test server CA signed certificates */
const char srv_signed_cert_pem[] = "-----BEGIN CERTIFICATE-----\n"
                                   "MIIDGzCCAgWgAwIBAgIES0KCvTALBgkqhkiG9w0BAQUwFzEVMBMGA1UEAxMMdGVz\n"
                                   "dF9jYV9jZXJ0MB4XDTEwMDEwNTAwMDcyNVoXDTQ1MDMxMjAwMDcyNVowFzEVMBMG\n"
                                   "A1UEAxMMdGVzdF9jYV9jZXJ0MIIBHzALBgkqhkiG9w0BAQEDggEOADCCAQkCggEA\n"
                                   "vfTdv+3fgvVTKRnP/HVNG81cr8TrUP/iiyuve/THMzvFXhCW+K03KwEku55QvnUn\n"
                                   "dwBfU/ROzLlv+5hotgiDRNFT3HxurmhouySBrJNJv7qWp8ILq4sw32vo0fbMu5BZ\n"
                                   "F49bUXK9L3kW2PdhTtSQPWHEzNrCxO+YgCilKHkY3vQNfdJ020Q5EAAEseD1YtWC\n"
                                   "IpRvJzYlZMpjYB1ubTl24kwrgOKUJYKqM4jmF4DVQp4oOK/6QYGGh1QmHRPAy3CB\n"
                                   "II6sbb+sZT9cAqU6GYQVB35lm4XAgibXV6KgmpVxVQQ69U6xyoOl204xuekZOaG9\n"
                                   "RUPId74Rtmwfi1TLbBzo2wIDAQABo3YwdDAMBgNVHRMBAf8EAjAAMBMGA1UdJQQM\n"
                                   "MAoGCCsGAQUFBwMBMA8GA1UdDwEB/wQFAwMHIAAwHQYDVR0OBBYEFOFi4ilKOP1d\n"
                                   "XHlWCMwmVKr7mgy8MB8GA1UdIwQYMBaAFP2olB4s2T/xuoQ5pT2RKojFwZo2MAsG\n"
                                   "CSqGSIb3DQEBBQOCAQEAHVWPxazupbOkG7Did+dY9z2z6RjTzYvurTtEKQgzM2Vz\n"
                                   "GQBA+3pZ3c5mS97fPIs9hZXfnQeelMeZ2XP1a+9vp35bJjZBBhVH+pqxjCgiUflg\n"
                                   "A3Zqy0XwwVCgQLE2HyaU3DLUD/aeIFK5gJaOSdNTXZLv43K8kl4cqDbMeRpVTbkt\n"
                                   "YmG4AyEOYRNKGTqMEJXJoxD5E3rBUNrVI/XyTjYrulxbNPcMWEHKNeeqWpKDYTFo\n"
                                   "Bb01PCthGXiq/4A2RLAFosadzRa8SBpoSjPPfZ0b2w4MJpReHqKbR5+T2t6hzml6\n"
                                   "4ToyOKPDmamiTuN5KzLN3cw7DQlvWMvqSOChPLnA3Q==\n"
                                   "-----END CERTIFICATE-----\n";
/* fallback layout */
#define oyjl_args_web_layout_css "\n\
@font-face {\n\
  font-family: Elsie;\n\
  src: url(../fonts/Elsie-Regular.woff) format(woff);\n\
}\n\
@media screen and (orientation:portrait) {\n\
body { margin: 0.25rem; } }\n\
@media screen and (orientation:landscape) {\n\
body { margin: 0.55rem; } }\n\
\n\
/* hide translations initially */\n\
.hide { display: none; }\n\
/* show a browser detected translation */\n\
:lang(de) { display: block; }\n\
li:lang(de) { display: list-item; }\n\
a:lang(de) { display: inline; }\n\
em:lang(de) { display: inline; }\n\
/*span:lang(de) { display: inline; }*/\n\
/* hide default language, if a translation was found */\n\
:lang(de) ~ [lang=en] {\n\
  display: none;\n\
}\n\
\n\
body {\n\
  background-color: #eee;\n\
}\n\
h1,h2,h3 {\n\
  font-family: 'Elsie';\n\
  /*font-weight: bold;\n\
  font-variant: small-caps;*/\n\
  font-style: normal;\n\
  text-align: center;\n\
  color: black;\n\
  background-color: #eee;\n\
}\n\
h1 {\n\
  font-size: 3em;\n\
  padding-top: 0.5em;\n\
  padding-bottom: 0.5em;\n\
  margin: 0;\n\
}\n\
h2 {\n\
  font-size: 2em;\n\
  color: white;\n\
  background-color: #568;\n\
}\n\
h3 {\n\
  font-size: 1.5em;\n\
}\n\
h3,.mandatory {\n\
  color: black;\n\
  background-color: #75bffc;\n\
}\n\
\n\
em {\n\
  font-size: 1.1em;\n\
}\n\
a {\n\
  text-align: center;\n\
  color: black;\n\
  text-decoration: none;\n\
}\n\
a:hover, a:focus {\n\
  transition: 1s color;\n\
  transition: 1s opacity;\n\
  text-decoration: underline;\n\
  opacity: 1.0;\n\
}\n\
\n\
.topmenu a {\n\
  display: block;\n\
  text-align: center;\n\
  line-height: 3;\n\
  font-weight: bold;\n\
  white-space: nowrap;\n\
  padding: 0em 0.8em;\n\
  margin: 0.5em 0em;\n\
  border-radius: 0.2em;\n\
  background: #568;\n\
  box-shadow: 0 0 0.5em #568;\n\
  opacity: 0.8;\n\
  color: white;\n\
}\n\
.topmenu, .topmenu p, .topmenu span {\n\
  width: 100%;\n\
  text-align: justify;\n\
  display: inline-block;\n\
  margin: 0;\n\
}\n\
.topmenu {\n\
  display: block;\n\
}\n\
.topmenu-background {\n\
  background-color: #eee;\n\
}\n\
.topmenu-background div {\n\
  margin: 0px 2vw;\n\
  width: 86vw;\n\
  width: -moz-available;          /* For Mozzila */\n\
  width: -webkit-fill-available;  /* For Chrome. */\n\
}\n\
.mobile {\n\
  display: none;\n\
}\n\
.desktop {\n\
  height: 0px;\n\
  padding-top: 1vw;\n\
  overflow: visible;\n\
  overflow-y: hidden;\n\
  transition: height 0.5s linear;\n\
}\n\
details p { margin: 0; }\n\
details summary::marker { content: '►'; }\n\
details[open] summary::marker { content: '▼'; }\n\
details[open] details summary::marker { content: '►'; }\n\
details[open] details[open] summary::marker { content: '▼'; }\n\
input[type=checkbox].mobile:checked ~ .desktop {\n\
  height: 20em;\n\
  padding-bottom: 4.5vw;\n\
}\n\
label[for].icon-bar {\n\
  font-size: 2em;\n\
  font-weight: bold;\n\
  line-height: 0.25;\n\
  text-align: center;\n\
  white-space: nowrap;\n\
  color: black;\n\
  background-color: #eee;\n\
  opacity: 0.8;\n\
  padding: 0.43em 0.35em;\n\
  border-radius: 0.1em;\n\
  box-shadow: 0 0 0.25em #eee;\n\
}\n\
label[for].icon-bar:hover {\n\
  transition: 0.5s color;\n\
  transition: 0.5s box-shadow;\n\
  transition: 0.5s background-color linear;\n\
\n\
  color: white;\n\
  background-color: #568;\n\
  box-shadow: 0 0 0.25em #568;\n\
}\n\
form select, form label[for] {\n\
  width: 49%;\n\
  display: inline-block;\n\
  height: 1.7em;\n\
}\n\
form label[for] {\n\
  vertical-align: middle;\n\
}\n\
form select {\n\
  padding: 0.1em 0;\n\
}\n\
form input {\n\
  width: 49%;\n\
  display: inline-block;\n\
  text-align: center;\n\
  white-space: nowrap;\n\
  color: black;\n\
  background-color: #eee;\n\
  padding: 0.2em 0;\n\
  border-width: 0;\n\
  border-radius: 0.1em;\n\
}\n\
form input[type=checkbox] {\n\
  width: 49%;\n\
}\n\
form input[type=range] {\n\
  width: 49%;\n\
}\n\
input[type=submit] {\n\
  width: 100%;\n\
  font-weight: bold;\n\
}\n\
p, .tile2, .tile {\n\
  page-break-inside: avoid;\n\
}\n\
\n\
a.img {\n\
  border: 1px solid #ddd;\n\
}\n\
img {\n\
  display: block;\n\
  margin-left: auto;\n\
  margin-right: auto;\n\
  max-width: 97.5%;\n\
}\n\
.figure .picture {\n\
  display: block;\n\
  margin-left: auto;\n\
  margin-right: auto;\n\
  max-width: 97.5%;\n\
}\n\
\n\
@media (min-width: 750px) {\n\
  body {\n\
    margin: 3rem;\n\
  }\n\
  h1 {\n\
    font-size: 5vw;\n\
  }\n\
  h2 {\n\
    font-size: 4.5vw;\n\
  }\n\
  h3 {\n\
    font-size: 3vw;\n\
  }\n\
  .topmenu a {\n\
    display: inline-block;\n\
    padding: 0em 0.3em;\n\
  }\n\
  .desktop, .topmenu p, .topmenu span {\n\
    display: inline-block;\n\
    text-align: justify;\n\
    width: 100%;\n\
    margin: 0;\n\
  }\n\
  input[type=checkbox].mobile:checked ~ .desktop, .desktop {\n\
    height: auto;\n\
    padding-bottom: 1.35vw;\n\
  }\n\
  label[for].icon-bar {\n\
    display: none;\n\
  }\n\
}\n\
\n\
@media (min-width: 1000px) {\n\
  .tiles {\n\
    display: flex;\n\
    justify-content: space-between;\n\
    flex-wrap: wrap;\n\
    align-items: flex-start;\n\
    width: 100%;\n\
  }\n\
  .tile {\n\
    flex: 0 1 49%;\n\
  }\n\
  .tile2 {\n\
    flex: 1 280px;\n\
    margin: 0 1vw 0 0;\n\
  }\n\
  h1,h2,h3 {\n\
    font-weight: normal;\n\
    font-variant: normal;\n\
  }\n\
  .topmenu a {\n\
    padding: 0em 1em;\n\
  }\n\
}\n\
@media (min-width: 1200px) {\n\
  @supports ( display: flex ) {\n\
    .tile {\n\
      flex: 0 1 24%;\n\
    }\n\
    h1 {\n\
      font-size: 3vw;\n\
    }\n\
    h2 {\n\
      font-size: 3vw;\n\
    }\n\
    h3 {\n\
      font-size: 2vw;\n\
    }\n\
  }\n\
  .topmenu a {\n\
    font-size: 1.1em;\n\
  }\n\
}\n\
\n\
\n\
@media (prefers-color-scheme: dark) {\n\
  /* dunkles Farbschema für die Nacht */\n\
  body {\n\
    color: white;\n\
    background: #555;\n\
  }\n\
  a {\n\
    color: #7ea;\n\
  }\n\
  h1,h2,h3,.mandatory {\n\
    color: #7da;\n\
    background-color: #333;\n\
  }\n\
  h2 {\n\
    /*color: black;\n\
    background-color: #75fcbf;*/\n\
  }\n\
  .topmenu a {\n\
    color: white;\n\
    box-shadow: 0 0 0.5em #586;\n\
    background: #586;\n\
  }\n\
  .topmenu-background {\n\
    background-color: #333;\n\
  }\n\
  label[for].icon-bar {\n\
    color: #586;\n\
    background-color: #eee;\n\
    box-shadow: 0 0 0.25em #eee;\n\
  }\n\
  label[for].icon-bar:hover {\n\
    color: white;\n\
    background-color: #586;\n\
    box-shadow: 0 0 0.25em #586;\n\
  }\n\
}\n\
\n\
@media (prefers-color-scheme: light) {\n\
  /* helles Farbschema für den Tag */\n\
  body {\n\
    color: black;\n\
    background-color: white;\n\
  }\n\
  a {\n\
    color: black;\n\
  }\n\
  h1,h2,h3,.mandatory {\n\
    color: black;\n\
    background-color: #eee;\n\
  }\n\
  h2 {\n\
    color: black;\n\
    background-color: #75bffc;\n\
  }\n\
  h3,.mandatory {\n\
    color: black;\n\
    background-color: #75bffc;\n\
  }\n\
  .topmenu a {\n\
    box-shadow: 0 0 0.5em #568;\n\
    background: #568;\n\
  }\n\
  .topmenu-background {\n\
    background-color: #eee;\n\
  }\n\
}\n\
"

/* fallback layout 2 */
#define oyjl_args_web_normalize_css "\n\
/*! normalize.css v5.0.0 | MIT License | github.com/necolas/normalize.css */\n\
\n\
/**\n\
 * 1. Change the default font family in all browsers (opinionated).\n\
 * 2. Correct the line height in all browsers.\n\
 * 3. Prevent adjustments of font size after orientation changes in\n\
 *    IE on Windows Phone and in iOS.\n\
 */\n\
\n\
/* Document\n\
   ========================================================================== */\n\
\n\
html {\n\
  font-family: sans-serif; /* 1 */\n\
  line-height: 1.15; /* 2 */\n\
  -ms-text-size-adjust: 100%; /* 3 */\n\
  -webkit-text-size-adjust: 100%; /* 3 */\n\
}\n\
\n\
/* Sections\n\
   ========================================================================== */\n\
\n\
/**\n\
 * Remove the margin in all browsers (opinionated).\n\
 */\n\
\n\
body {\n\
  margin: 0;\n\
}\n\
\n\
/**\n\
 * Add the correct display in IE 9-.\n\
 */\n\
\n\
article,\n\
aside,\n\
footer,\n\
header,\n\
nav,\n\
section {\n\
  display: block;\n\
}\n\
\n\
/**\n\
 * Correct the font size and margin on `h1` elements within `section` and\n\
 * `article` contexts in Chrome, Firefox, and Safari.\n\
 */\n\
\n\
h1 {\n\
  font-size: 2em;\n\
  margin: 0.67em 0;\n\
}\n\
\n\
/* Grouping content\n\
   ========================================================================== */\n\
\n\
/**\n\
 * Add the correct display in IE 9-.\n\
 * 1. Add the correct display in IE.\n\
 */\n\
\n\
figcaption,\n\
figure,\n\
main { /* 1 */\n\
  display: block;\n\
}\n\
\n\
/**\n\
 * Add the correct margin in IE 8.\n\
 */\n\
\n\
figure {\n\
  margin: 1em 40px;\n\
}\n\
\n\
/**\n\
 * 1. Add the correct box sizing in Firefox.\n\
 * 2. Show the overflow in Edge and IE.\n\
 */\n\
\n\
hr {\n\
  box-sizing: content-box; /* 1 */\n\
  height: 0; /* 1 */\n\
  overflow: visible; /* 2 */\n\
}\n\
\n\
/**\n\
 * 1. Correct the inheritance and scaling of font size in all browsers.\n\
 * 2. Correct the odd `em` font sizing in all browsers.\n\
 */\n\
\n\
pre {\n\
  font-family: monospace, monospace; /* 1 */\n\
  font-size: 1em; /* 2 */\n\
}\n\
\n\
/* Text-level semantics\n\
   ========================================================================== */\n\
\n\
/**\n\
 * 1. Remove the gray background on active links in IE 10.\n\
 * 2. Remove gaps in links underline in iOS 8+ and Safari 8+.\n\
 */\n\
\n\
a {\n\
  background-color: transparent; /* 1 */\n\
  -webkit-text-decoration-skip: objects; /* 2 */\n\
}\n\
\n\
/**\n\
 * Remove the outline on focused links when they are also active or hovered\n\
 * in all browsers (opinionated).\n\
 */\n\
\n\
a:active,\n\
a:hover {\n\
  outline-width: 0;\n\
}\n\
\n\
/**\n\
 * 1. Remove the bottom border in Firefox 39-.\n\
 * 2. Add the correct text decoration in Chrome, Edge, IE, Opera, and Safari.\n\
 */\n\
\n\
abbr[title] {\n\
  border-bottom: none; /* 1 */\n\
  text-decoration: underline; /* 2 */\n\
  text-decoration: underline dotted; /* 2 */\n\
}\n\
\n\
/**\n\
 * Prevent the duplicate application of `bolder` by the next rule in Safari 6.\n\
 */\n\
\n\
b,\n\
strong {\n\
  font-weight: inherit;\n\
}\n\
\n\
/**\n\
 * Add the correct font weight in Chrome, Edge, and Safari.\n\
 */\n\
\n\
b,\n\
strong {\n\
  font-weight: bolder;\n\
}\n\
\n\
/**\n\
 * 1. Correct the inheritance and scaling of font size in all browsers.\n\
 * 2. Correct the odd `em` font sizing in all browsers.\n\
 */\n\
\n\
code,\n\
kbd,\n\
samp {\n\
  font-family: monospace, monospace; /* 1 */\n\
  font-size: 1em; /* 2 */\n\
}\n\
\n\
/**\n\
 * Add the correct font style in Android 4.3-.\n\
 */\n\
\n\
dfn {\n\
  font-style: italic;\n\
}\n\
\n\
/**\n\
 * Add the correct background and color in IE 9-.\n\
 */\n\
\n\
mark {\n\
  background-color: #ff0;\n\
  color: #000;\n\
}\n\
\n\
/**\n\
 * Add the correct font size in all browsers.\n\
 */\n\
\n\
small {\n\
  font-size: 80%;\n\
}\n\
\n\
/**\n\
 * Prevent `sub` and `sup` elements from affecting the line height in\n\
 * all browsers.\n\
 */\n\
\n\
sub,\n\
sup {\n\
  font-size: 75%;\n\
  line-height: 0;\n\
  position: relative;\n\
  vertical-align: baseline;\n\
}\n\
\n\
sub {\n\
  bottom: -0.25em;\n\
}\n\
\n\
sup {\n\
  top: -0.5em;\n\
}\n\
\n\
/* Embedded content\n\
   ========================================================================== */\n\
\n\
/**\n\
 * Add the correct display in IE 9-.\n\
 */\n\
\n\
audio,\n\
video {\n\
  display: inline-block;\n\
}\n\
\n\
/**\n\
 * Add the correct display in iOS 4-7.\n\
 */\n\
\n\
audio:not([controls]) {\n\
  display: none;\n\
  height: 0;\n\
}\n\
\n\
/**\n\
 * Remove the border on images inside links in IE 10-.\n\
 */\n\
\n\
img {\n\
  border-style: none;\n\
}\n\
\n\
/**\n\
 * Hide the overflow in IE.\n\
 */\n\
\n\
svg:not(:root) {\n\
  overflow: hidden;\n\
}\n\
\n\
/* Forms\n\
   ========================================================================== */\n\
\n\
/**\n\
 * 1. Change the font styles in all browsers (opinionated).\n\
 * 2. Remove the margin in Firefox and Safari.\n\
 */\n\
\n\
button,\n\
input,\n\
optgroup,\n\
select,\n\
textarea {\n\
  font-family: sans-serif; /* 1 */\n\
  font-size: 100%; /* 1 */\n\
  line-height: 1.15; /* 1 */\n\
  margin: 0; /* 2 */\n\
}\n\
\n\
/**\n\
 * Show the overflow in IE.\n\
 * 1. Show the overflow in Edge.\n\
 */\n\
\n\
button,\n\
input { /* 1 */\n\
  overflow: visible;\n\
}\n\
\n\
/**\n\
 * Remove the inheritance of text transform in Edge, Firefox, and IE.\n\
 * 1. Remove the inheritance of text transform in Firefox.\n\
 */\n\
\n\
button,\n\
select { /* 1 */\n\
  text-transform: none;\n\
}\n\
\n\
/**\n\
 * 1. Prevent a WebKit bug where (2) destroys native `audio` and `video`\n\
 *    controls in Android 4.\n\
 * 2. Correct the inability to style clickable types in iOS and Safari.\n\
 */\n\
\n\
button,\n\
html [type=\"button\"], /* 1 */\n\
[type=\"reset\"],\n\
[type=\"submit\"] {\n\
  -webkit-appearance: button; /* 2 */\n\
}\n\
\n\
/**\n\
 * Remove the inner border and padding in Firefox.\n\
 */\n\
\n\
button::-moz-focus-inner,\n\
[type=\"button\"]::-moz-focus-inner,\n\
[type=\"reset\"]::-moz-focus-inner,\n\
[type=\"submit\"]::-moz-focus-inner {\n\
  border-style: none;\n\
  padding: 0;\n\
}\n\
\n\
/**\n\
 * Restore the focus styles unset by the previous rule.\n\
 */\n\
\n\
button:-moz-focusring,\n\
[type=\"button\"]:-moz-focusring,\n\
[type=\"reset\"]:-moz-focusring,\n\
[type=\"submit\"]:-moz-focusring {\n\
  outline: 1px dotted ButtonText;\n\
}\n\
\n\
/**\n\
 * Change the border, margin, and padding in all browsers (opinionated).\n\
 */\n\
\n\
fieldset {\n\
  border: 1px solid #c0c0c0;\n\
  margin: 0 2px;\n\
  padding: 0.35em 0.625em 0.75em;\n\
}\n\
\n\
/**\n\
 * 1. Correct the text wrapping in Edge and IE.\n\
 * 2. Correct the color inheritance from `fieldset` elements in IE.\n\
 * 3. Remove the padding so developers are not caught out when they zero out\n\
 *    `fieldset` elements in all browsers.\n\
 */\n\
\n\
legend {\n\
  box-sizing: border-box; /* 1 */\n\
  color: inherit; /* 2 */\n\
  display: table; /* 1 */\n\
  max-width: 100%; /* 1 */\n\
  padding: 0; /* 3 */\n\
  white-space: normal; /* 1 */\n\
}\n\
\n\
/**\n\
 * 1. Add the correct display in IE 9-.\n\
 * 2. Add the correct vertical alignment in Chrome, Firefox, and Opera.\n\
 */\n\
\n\
progress {\n\
  display: inline-block; /* 1 */\n\
  vertical-align: baseline; /* 2 */\n\
}\n\
\n\
/**\n\
 * Remove the default vertical scrollbar in IE.\n\
 */\n\
\n\
textarea {\n\
  overflow: auto;\n\
}\n\
\n\
/**\n\
 * 1. Add the correct box sizing in IE 10-.\n\
 * 2. Remove the padding in IE 10-.\n\
 */\n\
\n\
[type=\"checkbox\"],\n\
[type=\"radio\"] {\n\
  box-sizing: border-box; /* 1 */\n\
  padding: 0; /* 2 */\n\
}\n\
\n\
/**\n\
 * Correct the cursor style of increment and decrement buttons in Chrome.\n\
 */\n\
\n\
[type=\"number\"]::-webkit-inner-spin-button,\n\
[type=\"number\"]::-webkit-outer-spin-button {\n\
  height: auto;\n\
}\n\
\n\
/**\n\
 * 1. Correct the odd appearance in Chrome and Safari.\n\
 * 2. Correct the outline style in Safari.\n\
 */\n\
\n\
[type=\"search\"] {\n\
  -webkit-appearance: textfield; /* 1 */\n\
  outline-offset: -2px; /* 2 */\n\
}\n\
\n\
/**\n\
 * Remove the inner padding and cancel buttons in Chrome and Safari on macOS.\n\
 */\n\
\n\
[type=\"search\"]::-webkit-search-cancel-button,\n\
[type=\"search\"]::-webkit-search-decoration {\n\
  -webkit-appearance: none;\n\
}\n\
\n\
/**\n\
 * 1. Correct the inability to style clickable types in iOS and Safari.\n\
 * 2. Change font properties to `inherit` in Safari.\n\
 */\n\
\n\
::-webkit-file-upload-button {\n\
  -webkit-appearance: button; /* 1 */\n\
  font: inherit; /* 2 */\n\
}\n\
\n\
/* Interactive\n\
   ========================================================================== */\n\
\n\
/*\n\
 * Add the correct display in IE 9-.\n\
 * 1. Add the correct display in Edge, IE, and Firefox.\n\
 */\n\
\n\
details, /* 1 */\n\
menu {\n\
  display: block;\n\
}\n\
\n\
/*\n\
 * Add the correct display in all browsers.\n\
 */\n\
\n\
summary {\n\
  display: list-item;\n\
}\n\
\n\
/* Scripting\n\
   ========================================================================== */\n\
\n\
/**\n\
 * Add the correct display in IE 9-.\n\
 */\n\
\n\
canvas {\n\
  display: inline-block;\n\
}\n\
\n\
/**\n\
 * Add the correct display in IE.\n\
 */\n\
\n\
template {\n\
  display: none;\n\
}\n\
\n\
/* Hidden\n\
   ========================================================================== */\n\
\n\
/**\n\
 * Add the correct display in IE 10-.\n\
 */\n\
\n\
[hidden] {\n\
  display: none;\n\
}\n\
"

int  oyjlArgsWebGroupIsMan_          ( oyjl_val            g )
{
  oyjl_val first_o = oyjlTreeGetValue(g, 0, "options/[0]/option");
  const char * txt = OYJL_GET_STRING(first_o);
  if(oyjlStringStartsWith( txt, "man-", OYJL_COMPARE_CASE))
    return 1;
  return 0;
}

void oyjlArgsWebGroupPrintSection_   ( oyjl_val            g,
                                       char             ** t_,
                                       char             ** css_toc_text OYJL_UNUSED,
                                       int                 gcount,
                                       char             ** description,
                                       int                 level OYJL_UNUSED,
                                       oyjlSECURITY_e      sec)
{
  oyjl_val v;
  char * t = *t_;

  const char * gdesc, * gname, * ghelp, * synopsis;
  v = oyjlTreeGetValue(g, 0, "name");
  gname = OYJL_GET_STRING(v);
  v = oyjlTreeGetValue(g, 0, "description");
  gdesc = OYJL_GET_STRING(v);
  v = oyjlTreeGetValue(g, 0, "help");
  ghelp = OYJL_GET_STRING(v);
  v = oyjlTreeGetValue(g, 0, "synopsis");
  synopsis = OYJL_GET_STRING(v);

  if(!gname) { gname = gdesc; gdesc = NULL; }
  if(!gdesc) { gdesc = ghelp; ghelp = NULL; }

  if(gname || gdesc || ghelp || synopsis)
  {
    if(gname)
      oyjlStringAdd( &t, 0,0, "  <h3 id=\"h%did\">%s</h3>", gcount, gname );

    if(sec)
      oyjlStringAdd( description, 0,0, "  <details><summary></summary><p>\n" );
    else
      oyjlStringAdd( description, 0,0, "  <p>" );

    if(gdesc)
      oyjlStringAdd( description, 0,0, "%s%s", gdesc, ghelp?"<br />\n":synopsis?"<br />\n<br />\n":"" );
    if(ghelp)
    {
      char * t = oyjlStringLinkify( ghelp );
      ghelp = t;
      oyjlStringAdd( description, 0,0, "%s%s", ghelp, synopsis?"<br />\n<br />\n":"" );
      free(t);
    }
    if(synopsis)
      oyjlStringAdd( description, 0,0, "%s<br />\n<br />\n", synopsis );
  }
  *t_ = t;
}


void oyjlArgsWebOptionPrint_         ( oyjl_val            opt,
                                       int                 is_mandatory,
                                       char             ** t_,
                                       char             ** description,
                                       int                 gcount,
                                       oyjlSECURITY_e      sec,
                                       int                 flags )
{
  const char * txt;
  char * t = *t_;

  int k, kn = 0, is_choice = 0;
  const char * key, * name, * desc, * help, * value_name, * default_var, * type, * no_dash, * repetition;
  double default_dbl = 0.0;
  char * text = NULL;
  oyjl_val o = oyjlTreeGetValue(opt, 0, "option");
  oyjl_val choices;
  txt = OYJL_GET_STRING(o);
  if(oyjlStringStartsWith( txt, "man-", OYJL_COMPARE_CASE ))
    return;
  o = oyjlTreeGetValue(opt, 0, "key");
  key = OYJL_GET_STRING(o);
  o = oyjlTreeGetValue(opt, 0, "name");
  name = OYJL_GET_STRING(o);
  o = oyjlTreeGetValue(opt, 0, "description");
  desc = OYJL_GET_STRING(o);
  o = oyjlTreeGetValue(opt, 0, "help");
  help = OYJL_GET_STRING(o);
  o = oyjlTreeGetValue(opt, 0, "value_name");
  value_name = OYJL_GET_STRING(o);
  if(value_name) text = oyjlStringCopy( oyjlStringColor( oyjlITALIC, OYJL_HTML, value_name), 0 );
  o = oyjlTreeGetValue(opt, 0, "default");
  default_var = OYJL_GET_STRING(o);
  o = oyjlTreeGetValue(opt, 0, "repetition");
  repetition = OYJL_GET_STRING(o);
  if(default_var)
  {
    oyjlStringToDouble( default_var, &default_dbl, NULL, OYJL_KEEP_LOCALE );
    if(*oyjl_debug)
      fprintf( stderr, "default_var: %s default_dbl: %g locale: %s\n", default_var, default_dbl, setlocale(LC_NUMERIC, 0 ) );
  }
  o = oyjlTreeGetValue(opt, 0, "type");
  type = OYJL_GET_STRING(o);
  o = oyjlTreeGetValue(opt, 0, "no_dash");
  no_dash = OYJL_GET_STRING(o);
  if(key[0] == '@')
    no_dash = "1";

  choices = oyjlTreeGetValue(opt, 0, "choices");
  oyjlStringAdd( description, 0,0, "%s%s[%s%s%s%s]%s%s%s%s<br />\n",
      name?oyjlStringColor(oyjlBOLD, OYJL_HTML,"%s%s", name, repetition?" ...":""):"", name?"&nbsp;&nbsp;":"",
      no_dash?"":strlen(key) == 1?"-":"--", key[0] == '@'?"":key,
      value_name && key[0] != '@'?"=":"", value_name?text:"",
      desc?" : ":"", desc?desc:"",
      help&&help[0]?" - ":"", help&&help[0]?help:"" );

  if(type && strcmp(type,"bool") == 0)
  {
    if(sec)
      oyjlStringAdd( &t, 0,0, "  <label for=\"%s-%d\"%s>%s</label><input type=\"checkbox\" name=\"%s\" id=\"%s-%d\" value=\"%s\" %s%s/><br />\n",
       /*label for id*/key, gcount, is_mandatory?" class=\"mandatory\"":"", oyjlStringColor(flags?oyjlITALIC:oyjlNO_MARK, OYJL_HTML, OYJL_E(name,"")) /*i18n label*/, key /*name*/, key/* id */, gcount, no_dash?"true_no_dash":"true", (default_var && strcmp(default_var,"1") == 0) || is_mandatory?"checked":"",is_mandatory?" class=\"mandatory\"":"" );
  }
  else if(type && strcmp(type,"double") == 0)
  {
    double start, end, tick;
#ifdef OYJL_HAVE_LOCALE_H
    char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
    setlocale(LC_NUMERIC, "C");
#endif
    o = oyjlTreeGetValue(opt, 0, "start");
    start = OYJL_GET_DOUBLE(o);
    o = oyjlTreeGetValue(opt, 0, "end");
    end = OYJL_GET_DOUBLE(o);
    o = oyjlTreeGetValue(opt, 0, "tick");
    tick = OYJL_GET_DOUBLE(o);
    if(sec)
      oyjlStringAdd( &t, 0,0, "  <label for=\"%s-%d\"%s>%s <span id=span%s%d><font color=gray>%g</font></span> [≥%g ≤%g Δ%g]<br /></label><input type=\"range\" name=\"%s\" id=\"%s-%d\" value=\"%g\" min=\"%g\" max=\"%g\" step=\"%g\"%s oninput=\"span%s%d.innerText = this.value\"/>\n",
       /*label for id*/key, gcount, is_mandatory?" class=\"mandatory\"":"", oyjlStringColor(flags?oyjlITALIC:oyjlNO_MARK, OYJL_HTML, OYJL_E(name,"")) /*i18n label*/, default_dbl, start, end, tick, key/* span id */, gcount, key /*name*/, key/* id */, gcount, default_dbl, start, end, tick, is_mandatory?" class=\"mandatory\"":"", key/* span id */, gcount );
#ifdef OYJL_HAVE_LOCALE_H
    setlocale(LC_NUMERIC, save_locale);
    if(save_locale) free( save_locale );
#endif
  }
  else if(sec)
  {
    kn = oyjlValueCount( choices );
    if(kn)
    {
      if(type && strcmp(type,"choice") == 0)
      {
        is_choice = 1;
        oyjlStringAdd( &t, 0,0, "  <label for=\"%s-%d\"%s>%s</label>\n  <select id=\"%s-%d\" name=\"%s\">\n", key, gcount,
            is_mandatory?" class=\"mandatory\"":"", name?oyjlStringColor(flags?oyjlITALIC:oyjlNO_MARK, OYJL_HTML, name):"", key, gcount, key );
      }
      else if(type && strcmp(type,"string") == 0)
      {
        is_choice = 2;
        oyjlStringAdd( &t, 0,0, "  <label for=\"%s-%d\"%s>%s%s</label>\n  <input id=\"%s-%d\" list=\"%s-%d-states\" name=\"%s\" />\n\
  <datalist id=\"%s-%d-states\">\n", key, gcount, is_mandatory?" class=\"mandatory\"":"", name?oyjlStringColor(flags?oyjlITALIC:oyjlNO_MARK, OYJL_HTML, OYJL_E(name,"")):"", repetition?" ...":"", /* id */key, gcount, key, gcount, key, key, gcount );
      }
    } else if(type && strcmp(type,"string") == 0)
    {
      is_choice = 2;
      oyjlStringAdd( &t, 0,0, "  <label for=\"%s-%d\"%s>%s%s</label>\n  <input id=\"%s-%d\" name=\"%s\" /><br />\n", key, gcount, is_mandatory?" class=\"mandatory\"":"", name?oyjlStringColor(flags?oyjlITALIC:oyjlNO_MARK, OYJL_HTML, OYJL_E(name,"")):"", repetition?" ...":"", /* id */ key, gcount, /* name */ key );
    }
  }
 
  if(kn && sec)
  {
    if(is_choice == 1)
        oyjlStringAdd( &t, 0,0, "    <option value=\"\"></option>\n" );
    else if(is_choice == 2)
        oyjlStringAdd( &t, 0,0, "      <option value=\"\"></option>\n" );
  }
  for(k = 0; k < kn; ++k)
  {
    oyjl_val c = oyjlTreeGetValueF(choices, 0, "[%d]", k);
    oyjl_val cv;
    const char * nick;
    char * lnk, * lnk2, * lnk3;
    int selected = 0;
    cv = oyjlTreeGetValue(c, 0, "name");
    name = OYJL_GET_STRING(cv);
    cv = oyjlTreeGetValue(c, 0, "nick");
    nick = OYJL_GET_STRING(cv);
    cv = oyjlTreeGetValue(c, 0, "description");
    desc = OYJL_GET_STRING(cv);
    cv = oyjlTreeGetValue(c, 0, "help");
    help = OYJL_GET_STRING(cv);
    if((default_var && name && strcasecmp(name, default_var) == 0) ||
       (default_var && nick && strcasecmp(nick, default_var) == 0))
      selected = 1;
    if(sec)
    {
      if(is_choice == 1)
        oyjlStringAdd( &t, 0,0, "    <option value=\"%s\"%s>%s</option>\n", nick, selected?" selected":"", name&&name[0]?name:nick );
      else if(is_choice == 2)
        oyjlStringAdd( &t, 0,0, "      <option value=\"%s\"%s>%s</option>\n", nick, selected?" selected":"", name&&name[0]?name:nick );
    }
    lnk = oyjlStringLinkify( name );
    lnk2 = oyjlStringLinkify( desc );
    lnk3 = oyjlStringLinkify( help );
    oyjlStringAdd( description, 0,0, "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s%s%s%s%s%s%s%s%s%s<br />\n", no_dash?"":strlen(key) == 1?"-":"--", key[0] != '@'?key:"",
      nick && key[0] != '@'?" ":"", nick?nick:"",
      name&&name[0]?"    # ":"", name?lnk:"",
      desc?" : ":"", desc&&desc[0]?lnk2:"",
      help&&help[0]?" - ":"", help&&help[0]?lnk3:"" );
    if(lnk) free(lnk);
    if(lnk2) free(lnk2);
    if(lnk3) free(lnk3);
  }
  if(kn && sec)
  {
    if(is_choice == 1)
      oyjlStringAdd( &t, 0,0, "  </select><br />\n" );
    else if(is_choice == 2)
      oyjlStringAdd( &t, 0,0, "    </datalist>\n\
</input><br />\n" );
  }

  if(text) { free(text); text = NULL; }
  *t_ = t;
}

oyjl_val oyjlArgsWebGroupFindOption_ ( oyjl_val            root,
                                       const char        * option,
                                       int               * gcount )
{
  int i, n;
  oyjl_val v = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules/[0]"); // use UI JSON
  oyjl_val val = oyjlTreeGetValue(v, 0, "groups");
  n = oyjlValueCount( val );
  for(i = 0; i < n; ++i)
  {
    int j, count, has_options;
    oyjl_val g = oyjlTreeGetValueF(val, 0, "[%d]", i);
    has_options = oyjlTreeGetValue(g, 0, "options") == 0?0:oyjlArgsWebGroupIsMan_(g)?0:1;
    if(has_options)
    {
      ++(*gcount);
      v = oyjlTreeGetValue(g, 0, "options");
      count = oyjlValueCount( v );
      for(j = 0; j < count; ++j)
      {
        oyjl_val opt = oyjlTreeGetValueF(v, 0, "[%d]", j);
        oyjl_val o = oyjlTreeGetValue(opt, 0, "key");
        const char * key = OYJL_GET_STRING(o);
        if(key && strcmp(option, key) == 0)
          return opt;
      }
    }
    v = oyjlTreeGetValue(g, 0, "groups");
    count = oyjlValueCount( v );
    for(j = 0; j < count; ++j)
    {
      g = oyjlTreeGetValueF(v, 0, "[%d]", j);
      if(oyjlTreeGetValue(g, 0, "options"))
      {
        ++(*gcount);
      }
    }
  }

  return 0;
}

void oyjlArgsWebGroupPrintNonDetail_ ( oyjl_val            root,
                                       const char        * set,
                                       char             ** t_,
                                       const char        * gname,
                                       int                 gcount,
                                       int                 sec,
                                       int                 debug,
                                       const char        * set_orig,
                                       const char        * ignore_ )
{
  if(set && strlen(set))
  {
    int i, n = 0;
    char * ignore = ignore_ ? oyjlStringCopy( ignore_, 0 ) : NULL;
    char ** list = oyjlStringSplit2( set, "|,", 0, &n, NULL, malloc );
    for( i = 0; i  < n; ++i )
    {
      const char * opt = list[i];
      int src_gcount = 0;
      oyjl_val o = oyjlArgsWebGroupFindOption_( root, opt, &src_gcount );
      char * opt_bold = oyjlStringCopy( oyjlTermColor(oyjlBOLD,opt), 0 );
      if(!o)
        oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, 0, OYJL_DBG_FORMAT "OyjlArgsWeb option not found: %s from set: %s/%s", OYJL_DBG_ARGS, opt_bold, set, set_orig );
      else
      {
        int flags = 0;
        oyjl_val o_key = oyjlTreeGetValue(o, 0, "key");
        const char * key = OYJL_GET_STRING(o_key);
        if(oyjlStringSplitFind(ignore, ",", key, 0, NULL, 0,0) >= 0)
          flags = 1;
        if(debug)
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsWeb add option from group %d to form %s: %s", OYJL_DBG_ARGS, src_gcount-1, gname, opt_bold );
        if(!flags)
          oyjlArgsWebOptionPrint_( o, 0/* is_mandatory */, t_, NULL /* add no repeating description */, gcount, sec, flags );
      }
      free(opt_bold);
    }
    if(ignore) free(ignore);
    oyjlStringListRelease( &list, n, free );
  }
}

void oyjlArgsWebGroupPrint_          ( oyjl_val            groups,
                                       int                 i,
                                       char             ** t_,
                                       char             ** description,
                                       int                 gcount,
                                       oyjlSECURITY_e      sec,
                                       oyjl_val            root,
                                       int                 debug,
                                       const char        * ignore_ )
{
  oyjl_val v;
  const char * gname;
  char * mandatory = NULL, * optional = NULL, * mandatory_orig = NULL, * optional_orig = NULL;
  oyjl_val g = oyjlTreeGetValueF( groups, 0, "[%d]", i );
  char * ignore = ignore_ ? oyjlStringCopy( ignore_, 0 ) : NULL;

  int j, count, mandatory_n = 0, mandatory_found = 0, optional_n = 0, optional_found = 0;

  v = oyjlTreeGetValue(g, 0, "name");
  gname = OYJL_GET_STRING(v);
  if(!gname)
  {
    v = oyjlTreeGetValue(g, 0, "description");
    gname = OYJL_GET_STRING(v);
  }
  v = oyjlTreeGetValue(g, 0, "mandatory");
  mandatory = oyjlStringCopy( OYJL_GET_STRING(v), 0 );
  mandatory_orig = oyjlStringCopy( mandatory, 0 );
  mandatory_n = oyjlStringSplitFind(mandatory, "|,", NULL, OYJL_REMOVE, &mandatory, 0,0);
  v = oyjlTreeGetValue(g, 0, "optional");
  optional = oyjlStringCopy( OYJL_GET_STRING(v), 0 );
  optional_orig = oyjlStringCopy( optional, 0 );
  optional_n = oyjlStringSplitFind(optional, "|,", NULL, OYJL_REMOVE, &optional, 0,0);
  v = oyjlTreeGetValue(g, 0, "options");
  count = oyjlValueCount( v );
  for(j = 0; j < count; ++j)
  {
    int is_mandatory = 0/*, is_optional = 0*/;
    oyjl_val opt = oyjlTreeGetValueF(v, 0, "[%d]", j);
    oyjl_val o = oyjlTreeGetValue(opt, 0, "key");
    const char * key = OYJL_GET_STRING(o);
    int flags = 0;
    if(oyjlStringSplitFind(mandatory, "|,", key, OYJL_REMOVE, &mandatory, 0,0) >= 0) { is_mandatory = 1; ++mandatory_found; };
    if(oyjlStringSplitFind(optional, "|,", key, OYJL_REMOVE, &optional, 0,0) >= 0) { /*is_optional = 1;*/ ++optional_found; };
    if(oyjlStringSplitFind(ignore, ",", key, 0, NULL, 0,0) >= 0)
      flags = 1;
    oyjlArgsWebOptionPrint_( opt, is_mandatory, t_, description, gcount, sec, flags );
  }

  if(mandatory && strlen(mandatory))
    oyjlArgsWebGroupPrintNonDetail_( root, mandatory, t_, gname, gcount, sec, debug, mandatory_orig, ignore );
  if(optional && strlen(optional))
    oyjlArgsWebGroupPrintNonDetail_( root, optional, t_, gname, gcount, sec, debug, optional_orig, ignore );

  if(debug)
  {
    fprintf( stderr, "%s: mandatory(%s): %d[%d], optional(%s): %d[%d] count: %d\n", OYJL_E(gname,""),
           OYJL_E(mandatory,""), mandatory_found, mandatory_n,
           OYJL_E(optional,""),  optional_found,  optional_n,  count );
  }

  if(mandatory) free(mandatory);
  if(optional) free(optional);
  if(mandatory_orig) free(mandatory_orig);
  if(optional_orig) free(optional_orig);
  if(ignore) free(ignore);
}

#include "oyjl_io_internal.h"

char ** oyjl_args_web_file_names_allowed = NULL;
int     oyjl_args_web_file_names_allowed_n = 0;
char ** oyjl_args_web_file_names_no = NULL;
int     oyjl_args_web_file_names_no_n = 0;
const char * oyjl_args_web_file_name_security_feature = NULL;
#define OYJL_FILE_NAME_POLICY(policy_, description) \
{ \
  const char * policy = policy_; \
  policy += 3; \
  if( !error && \
      (!oyjl_args_web_file_name_security_feature || oyjlStringSplitFind(oyjl_args_web_file_name_security_feature, ",", policy_, 0,NULL, 0,0) >= 0) && \
      strstr(fn, policy) ) { error = 1; error_msg = policy_ " - " description; } \
  if( strcmp(fn, "oyjl-list") == 0 ) fprintf( stderr, "        %s\t- %s\n", oyjlTermColor(oyjlITALIC, policy_), description ); \
}
#define OYJL_USE_POLICY(policy_, description) \
  if( strcmp(fn, "oyjl-list") == 0 ) fprintf( stderr, "        %s\t- %s\n", oyjlTermColor(oyjlITALIC, policy_), description ); \
  if( !error && \
      (!oyjl_args_web_file_name_security_feature || oyjlStringSplitFind(oyjl_args_web_file_name_security_feature, ",", policy_, 0, NULL, 0,0) >= 0) )
int oyjl_args_web_debug = 0;
int oyjlArgsWebFileNameSecurity      ( const char       ** full_filename,
                                       int                 write_size OYJL_UNUSED )
{
  const char * fn = *full_filename;
  int error = 0,
      hidden = 0,
      above = 0,
      except = 0;
  const char * error_msg = NULL;
  char * cd = NULL;

  if(!fn) return 1;

  if(oyjlStringListFind( oyjl_args_web_file_names_allowed, &oyjl_args_web_file_names_allowed_n, fn, 0,0 ) >= 0)
    return 0;
  if(oyjlStringListFind( oyjl_args_web_file_names_no, &oyjl_args_web_file_names_no_n, fn, 0,0 ) >= 0)
    return 1;

  if( strcmp(fn, "oyjl-list") == 0 ) fprintf( stderr, "      with %s=\"checkXXX\" use all \"security\" file check policies or add one by one on a as needed base, separated by comma, from %s()\n      Avalable file check policies are:\n", oyjlTermColor(oyjlBOLD,"security"), __func__ );
  OYJL_FILE_NAME_POLICY( "no_/etc", "no system configuration" )
  OYJL_FILE_NAME_POLICY( "no_/root", "no root files" )
  OYJL_FILE_NAME_POLICY( "no_/proc", "no system state files" )
  OYJL_FILE_NAME_POLICY( "no_..", "no file and directory relative above" )

  OYJL_USE_POLICY( "no_stdin", "no input file stream" )
  if( strcmp(fn, "-") == 0 || strcmp(fn, "stdin") == 0 )
  { error = 1; error_msg = "no_stdin - no input file stream"; }

  OYJL_USE_POLICY("no_hidden", "no hidden file and directory")
  if( (fn[0] == '.' && strlen(fn) > 1 && fn[1] != OYJL_SLASH_C) || strstr(fn, "/.") != NULL )
    hidden = 1;

  OYJL_USE_POLICY("no_above", "no file and directory above cwd")
  if( fn[0] == OYJL_SLASH_C )
  {
    int len = strlen(fn),
        clen;

    cd = oyjlResolveDirFile(".");
    clen = strlen(cd);

    if(!len || !clen || len < clen || memcmp( fn, cd, clen ) != 0)
      above = 1;
  }

  if(oyjl_args_web_file_name_security_feature && strstr(oyjl_args_web_file_name_security_feature,"allow_"))
  {
    int i, n = 0;
    char ** list = oyjlStringSplit2( oyjl_args_web_file_name_security_feature, ",", 0, &n, NULL, malloc );
    for(i = 0; i < n; ++i)
    {
      const char * arg = list[i];
      const char * dynamic_policy = arg;
      int allow = 0;
      if(!oyjlStringStartsWith( arg, "allow_", OYJL_COMPARE_CASE )) continue;
      dynamic_policy += 6;
      allow = oyjlStringStartsWith( fn, dynamic_policy, 0 );

      if(allow && hidden && (fn[0] == '.' || strstr(fn,"/.")))
      { except = 1; hidden = 0;
      }

      if(allow && above)
      { except = 2; above = 0; 
      }

      if(except)
      {
        char * msg = oyjlStringCopy(oyjlTermColor(oyjlGREEN,fn), 0);
        if(*oyjl_debug >= 2)
        {
          if(*oyjl_debug >= 3) fprintf( stderr, "%s", oyjlBT(0) );
          fprintf( stderr, OYJL_DBG_FORMAT "allow %s: %s\n", OYJL_DBG_ARGS, except == 1?"hidden":"above", msg );
        }
        free(msg);
        break;
      }
    }
  }
  if( strcmp(fn, "oyjl-list") == 0 ) fprintf( stderr, "        %s\t- allow custom /my/path exceptions, overriding no_above or no_hidden rules: e.g. =check,no_hidden,allow_/home/user/.local/share\n", oyjlTermColor(oyjlITALIC,"allow_/my/path") );

  if(hidden)
  { error = 1; error_msg = ". - no hidden file and directory"; }
  if(above)
  { error = 1; error_msg = " - no file and directory above cwd"; }

  if(0 < error && error < 100)
  {
    char * msg = oyjlStringCopy(oyjlTermColor(oyjlRED,error_msg), 0);
    if(*oyjl_debug >= 3) fprintf( stderr, "%s", oyjlBT(0) );
    if(oyjl_args_web_debug || *oyjl_debug)
    {
      if(cd)
        oyjlMessage_p( oyjlMSG_SECURITY_ALERT, 0, OYJL_DBG_FORMAT "OyjlArgsWeb inhibits: fn:\"%s\" cwd:\"%s\" policy: %s", OYJL_DBG_ARGS, fn, cd, msg );
      else
        oyjlMessage_p( oyjlMSG_SECURITY_ALERT, 0, OYJL_DBG_FORMAT "OyjlArgsWeb inhibits: \"%s\" file policy: %s", OYJL_DBG_ARGS, fn, msg );
    }
    *full_filename = "";
    free(msg);
    oyjlStringListPush( &oyjl_args_web_file_names_no, &oyjl_args_web_file_names_no_n, fn, 0,0 );
  } else if(oyjl_args_web_debug || *oyjl_debug)
  {
    fprintf( stderr, OYJL_DBG_FORMAT, OYJL_DBG_ARGS );
    fprintf( stderr, "%s%s\n", oyjlTermColor(oyjlGREEN,fn), !except ? "" : except == 1?" allow hidden":" allow above" );
    oyjlStringListPush( &oyjl_args_web_file_names_allowed, &oyjl_args_web_file_names_allowed_n, fn, 0,0 );
  }

  if(cd) free(cd);

  return error;
}

int     oyjl_args_web_run = 0;
int oyjlArgsWebStart__               ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands OYJL_UNUSED,
                                       const char        * output OYJL_UNUSED,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv) )
{
  char * input = NULL;
  char * t = NULL;
  int i;
  int port = PORT;
  char * https_key = NULL,
       * https_cert = NULL,
       * ignore = NULL,
       * css = NULL,
       * css2 = NULL,
       * help = NULL;
  oyjlSECURITY_e sec = oyjlSECURITY_READONLY;

  int web_pameters_list_n = 0;
  char ** web_pameters_list;

  if(debug)
  {
    for(i = 0; i < argc; ++i)
      fprintf( stderr, "%s ", argv[i] );
    fprintf( stderr, "\n" );
    oyjl_args_web_debug = debug;
  }

  oyjl_val root = NULL;
  char error_buffer[256] = {0};
  int r = 0;

  ++oyjl_args_web_run;

  if( json && strlen( json ) )
  {
    r = oyjlIsFile( json, "r", OYJL_NO_CHECK, NULL, 0 );
    if(!r && oyjlDataFormat(json) == 7)
    {
      int state = 0;
      root = oyjlTreeParse2( json, 0, __func__, &state );
      if(state)
      {
        fprintf(stderr, "ERROR:\t\"%s\"\n", oyjlPARSE_STATE_eToString(state));
        char * error = NULL;
        oyjlStringAdd( &error, 0,0, "{\"error\": \"%s\"}", json );
        json = error;
        r = -1;
      }
    }
  }

  if(ui)
  {
    const char * web_pameters = NULL;
    oyjlOptions_GetResult( ui->opts, "render", &web_pameters, 0,0 );
    if(!web_pameters)
      oyjlOptions_GetResult( ui->opts, "R", &web_pameters, 0,0 );
    if(web_pameters)
    {
      long lo = 8888;
      web_pameters_list = oyjlStringSplit( web_pameters, ':', &web_pameters_list_n, 0 );
      if(debug)
        fprintf (stderr, "Parameters for rendering web page: \"%s\" %d\n", web_pameters, web_pameters_list_n);

      for(i = 1 /* zero param is "web" */; i < web_pameters_list_n; ++i)
      {
        char * param = web_pameters_list[i],
             * arg = strchr(param, '='),
             * security = NULL;
        if(arg)
        {
          arg[0] = '\000';
          ++arg;
        }
        if(debug)
          fprintf( stderr, "parsing sub param[%d]:%s\targ:%s\n", i, param, OYJL_E(arg,"") );
#define OYJL_SUB_ARG_LONG( param_, position_, lo_ ) { \
        if( ( (i == position_ && !arg) || \
              (arg && strcasecmp(param,param_) == 0) ) && \
            oyjlStringToLong(arg?arg:param, &lo, NULL) == 0 ) \
          lo_ = lo; }
        OYJL_SUB_ARG_LONG( "port", 1, port )
#define OYJL_SUB_ARG_STRING( param_, position_, txt_ ) { \
        if( ( (i == position_ && !arg) || \
              (strcasecmp(param,param_) == 0) ) ) \
          txt_ = arg?arg:param; }
        OYJL_SUB_ARG_STRING( "https_key", 0, https_key )
        OYJL_SUB_ARG_STRING( "https_cert", 0, https_cert )
        OYJL_SUB_ARG_STRING( "security", 0, security )
        if(security)
        {
          if(oyjlStringSplitFind(security, ",", "readonly", 0, NULL, 0,0) >= 0)
            sec = oyjlSECURITY_READONLY;
          else
          if(oyjlStringSplitFind(security, ",", "interactive", 0, NULL, 0,0) >= 0)
            sec = oyjlSECURITY_INTERACTIVE;
          else
          if(oyjlStringSplitFind(security, ",", "check_read", 0, NULL, 0,0) >= 0)
            sec = oyjlSECURITY_CHECK_READ;
          else
          if(oyjlStringSplitFind(security, ",", "check_write", 0, NULL, 0,0) >= 0)
            sec = oyjlSECURITY_CHECK_WRITE;
          else
          if(oyjlStringSplitFind(security, ",", "check", 0, NULL, 0,0) >= 0)
            sec = oyjlSECURITY_CHECK;
          else
          if(oyjlStringSplitFind(security, ",", "lazy", 0, NULL, 0,0) >= 0)
            sec = oyjlSECURITY_LAZY;
          if(strchr(security, ',')) /* contains policies */
            oyjl_args_web_file_name_security_feature = security;
        }
        if(css)
          OYJL_SUB_ARG_STRING( "css", 0, css2 )
        else
          OYJL_SUB_ARG_STRING( "css", 0, css )
        OYJL_SUB_ARG_STRING( "help", 0, help )
        OYJL_SUB_ARG_STRING( "ignore", 0, ignore )
      }
      if(ignore)
      {
        int n = 0;
        char ** list = oyjlStringSplit(ignore, ',', &n, 0);
        for(i = 0; i < n; ++i)
        {
          const char * o = list[i];
          oyjlOption_s * opt = oyjlOptions_GetOptionL( ui->opts, o, 0 );
          if(!opt)
            fprintf (stderr,
               "Failed to get option: %s from ignore:%s\n", oyjlTermColor( oyjlRED, o), ignore);
        }
      }
    }
    else
      fprintf (stderr, "%s\n", oyjlTermColor( oyjlRED, "No parameters for rendering web page" ));
  }

  if(oyjlSECURITY_CHECK <= sec && sec < oyjlSECURITY_LAZY)
  {
    if(sec == oyjlSECURITY_CHECK_READ)
    {
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "set: oyjlArgsWebFileNameSecurity() for oyjlFileRead()", OYJL_DBG_ARGS );
      oyjlFileNameCheckFuncSet( oyjlArgsWebFileNameSecurity, OYJL_IO_READ );
    } else if(sec == oyjlSECURITY_CHECK_WRITE)
    {
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "set: oyjlArgsWebFileNameSecurity() for oyjlFileWrite()", OYJL_DBG_ARGS );
      oyjlFileNameCheckFuncSet( oyjlArgsWebFileNameSecurity, OYJL_IO_WRITE );
    } else
    {
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "set: oyjlArgsWebFileNameSecurity() for oyjlFileRead() and oyjlFileWrite()", OYJL_DBG_ARGS );
      oyjlFileNameCheckFuncSet( oyjlArgsWebFileNameSecurity, OYJL_IO_READ | OYJL_IO_WRITE );
    }
  }


  if( (root && oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules")) || // use UI JSON
      (!root && json && strlen(json)) ) // assume JSON filename
  {
    if(!root && json && strlen(json))
    {
      if(r)
      {
        if(debug)
          fprintf( stderr, "Found file name: %s\n", json );
      }
      else
        fprintf( stderr, "Assume file name or stream: %s\n", json );
    } else
      if(debug)
        fprintf( stderr, "Found Json org/freedesktop/oyjl/modules: %lu\n", strlen(json) );
    //mgr.setUri( QString(json) );
    input = oyjlStringCopy( json, 0 );
  }
  else
  {
    char * merged = NULL, * tmp = NULL;
    json = tmp = oyjlUi_ToJson( ui, 0 ); // generate JSON from ui data struct
    if(debug)
      fprintf( stderr, "oyjlUi_ToJson(): %lu\n", json?strlen(json):0);
    if(root && json)
    {
      oyjl_val module = oyjlTreeParse( json, error_buffer, 256 );
      oyjl_val rv = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/translations");
      oyjl_val mv = oyjlTreeGetValue(module, OYJL_CREATE_NEW, "org/freedesktop/oyjl/translations");
      if(rv && mv) // merge in translations
      {
        size_t size = sizeof(*rv);
        memcpy( mv, rv, size );
        memset( rv, 0, size );
        int level = 0;
        oyjlTreeToJson( module, &level, &merged );
        if(debug)
          fprintf( stderr, "merge UI JSON with translation\n");
      } else
        fprintf( stderr, "expected translation is missing\n");
      oyjlTreeFree( module );
    }
    if(!merged && json)
    {
      merged = oyjlStringCopy( json, NULL );
      if(debug)
        fprintf( stderr, "use generated UI JSON\n");
    }
    input = oyjlStringCopy( merged, 0 );
    if(merged) free(merged);
    if(tmp) free(tmp);
  }

  if(root) { oyjlTreeFree( root ); root = NULL; }

  if(input)
  {
    if(oyjlDataFormat(input) == 7)
    {
      int state = 0;
      root = oyjlTreeParse2( input, 0, __func__, &state );
      if(state)
      {
        fprintf(stderr, "ERROR:\t\"%s\"\n", oyjlPARSE_STATE_eToString(state));
        char * error = NULL;
        oyjlStringAdd( &error, 0,0, "{\"error\": \"%s\"}", input );
        input = error;
        r = -1;
      }
    }
    if(debug)
      fprintf( stderr, "input: %ld\n", strlen(input) );
  }

  if(root)
  {
    oyjl_val v = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules/[0]"); // use UI JSON
    oyjl_val val = oyjlTreeGetValue(v, 0, "nick");
    const char * nick = OYJL_GET_STRING(val),
               * name = NULL, * desc = NULL, * version = NULL, * docu = NULL, * type = NULL, * txt;
    int i = 0, n, size = 0;
    const char * key_list = "name,description,help,label";
    const char * lang = oyjlLang("");
    struct MHD_Daemon * daemon = NULL;
    int tls_flag = 0;
    char * get_page = NULL;
    char * https_key_pem = NULL, * https_cert_pem = NULL,
         * css_text = NULL,
         * css2_text = NULL,
         * css_toc_text = NULL;
    int gcount = 0;
    if(debug)
      fprintf( stderr, "using lang: %s\n", lang );
    oyjlTranslateJson( root, NULL, key_list );

    val = oyjlTreeGetValue(v, 0, "name");
    name = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue(v, 0, "description");
    desc = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue(v, 0, "type");
    type = OYJL_GET_STRING(val);
    if(!type) type = "module";
    if(strcmp(type,"tool") == 0)
      type = _("Tool");
    if(strcmp(type,"module") == 0)
      type = _("Module");
    val = oyjlTreeGetValue(v, 0, "information");
    n = oyjlValueCount( val );
    for(i = 0; i < n; ++i)
    {
      oyjl_val info = oyjlTreeGetValueF(val, 0, "[%d]/type", i);
      txt = OYJL_GET_STRING(info);
      if(txt && strcmp(txt,"version") == 0)
      {
        info = oyjlTreeGetValueF(val, 0, "[%d]/name", i);
        version = OYJL_GET_STRING(info);
      }
      if(txt && strcmp(txt,"documentation") == 0)
      {
        info = oyjlTreeGetValueF(val, 0, "[%d]/description", i);
        docu = OYJL_GET_STRING(info);
      }
    }
    oyjlStringAdd( &t, 0,0, "<h1 id=\"h1intro\">%s</h1>\n", OYJL_E(OYJL_E(name,OYJL_E(nick,type)),type) );
    if((name && nick) || version || desc || docu)
    {
      if(sec == oyjlSECURITY_READONLY)
      {
        oyjlStringAdd( &t, 0,0, "<div class=\"text\">\n" );
        oyjlStringAdd( &t, 0,0, "%s: ", type );
      }
      else
        oyjlStringAdd( &t, 0,0, "  <details><summary>%s %s</summary><p>", _("Description"), type );
      if(name && nick)
        oyjlStringAdd( &t, 0,0, "%s", oyjlStringColor(oyjlBOLD, OYJL_HTML, nick) );
      if(version)
        oyjlStringAdd( &t, 0,0, " v%s", oyjlStringColor(oyjlITALIC, OYJL_HTML, version) );
      if(docu)
        oyjlStringAdd( &t, 0,0, " %s", docu );
      oyjlStringAdd( &t, 0,0, "  </p><br />" );
      for(i = 0; i < n; ++i)
      {
        oyjl_val info = oyjlTreeGetValueF(val, 0, "[%d]/type", i);
        char * tmp;
        txt = OYJL_GET_STRING(info);
        if(i == 0)
          oyjlStringAdd( &t, 0,0, " <details><summary>%s</summary><table>", _("Details") );
        if(txt && strcmp(txt,"documentation") == 0) continue;
        oyjlStringAdd( &t, 0,0, "   <tr>" );
        info = oyjlTreeGetValueF(val, 0, "[%d]/label", i);
        txt = OYJL_GET_STRING(info);
        tmp = oyjlStringLinkify( txt );
        oyjlStringAdd( &t, 0,0, "     <td>%s</td>", OYJL_E(tmp,"") );
        free(tmp);
        info = oyjlTreeGetValueF(val, 0, "[%d]/name", i);
        txt = OYJL_GET_STRING(info);
        tmp = oyjlStringLinkify( txt );
        oyjlStringAdd( &t, 0,0, "     <td>%s</td>", OYJL_E(tmp,"") );
        free(tmp);
        info = oyjlTreeGetValueF(val, 0, "[%d]/description", i);
        txt = OYJL_GET_STRING(info);
        tmp = oyjlStringLinkify( txt );
        oyjlStringAdd( &t, 0,0, "     <td>%s</td>", OYJL_E(tmp,"") );
        free(tmp);
        oyjlStringAdd( &t, 0,0, "   </tr>" );
      }
      if(n)
      {
        oyjlStringAdd( &t, 0,0, " </table>" );
        oyjlStringAdd( &t, 0,0, "</details><br />\n" );
      }
      oyjlStringAdd( &t, 0,0, "%s", _("Hints: Options with colord labels are mandatory. Options with italic labels are ignored. Multiple options '...' can be separated by semicolon ';' in the text field.") );
      if(sec == oyjlSECURITY_READONLY)
        oyjlStringAdd( &t, 0,0, "</div>\n" );
      else
        oyjlStringAdd( &t, 0,0, "</details>\n" );
    }
    oyjlStringAdd( &t, 0,0, "<div class=\"tiles\">\n" );

    val = oyjlTreeGetValue(v, 0, "groups/[0]/properties/oyjl_args/web");
    {
      const char * oyjl_args_properties = OYJL_GET_STRING(val);
      oyjl_val defaults = oyjlOptionStringToJson( oyjl_args_properties );
      oyjlUiJsonSetDefaults( root, defaults );
      oyjlTreeFree( defaults ); defaults = NULL;
    }

    val = oyjlTreeGetValue(v, 0, "groups");
    n = oyjlValueCount( val );
    for(i = 0; i < n; ++i)
    {
      int j, count, has_options;
      char * description = NULL;
      oyjl_val g = oyjlTreeGetValueF(val, 0, "[%d]", i);
      has_options = oyjlTreeGetValue(g, 0, "options") == 0?0:oyjlArgsWebGroupIsMan_(g)?0:1;
      v = oyjlTreeGetValue(g, 0, "groups");
      count = oyjlValueCount( v );
      if(count || has_options)
      {
        oyjlStringAdd( &t, 0,0, "<div class=\"tile2\">\n" );
        oyjlArgsWebGroupPrintSection_(g, &t, &css_toc_text, gcount, &description, 0, sec);
      }
      if(has_options)
      {
        if(sec)
          oyjlStringAdd( &t, 0,0, "<form action=\"/result\" method=\"post\" enctype=\"multipart/form-data\">\n" );
        oyjlArgsWebGroupPrint_(val, i, &t, &description, gcount, sec, root, debug, ignore);
        if(sec)
          oyjlStringAdd( &t, 0,0, "  <input type=\"submit\" value=\" %s \"></form>\n", _("Send") );
        if(description)
        {
          oyjlStringAdd( &t, 0,0, "%s  </p></details>\n", description );
          free(description);
          description = NULL;
        }
        ++gcount;
      }
      for(j = 0; j < count; ++j)
      {
        g = oyjlTreeGetValueF(v, 0, "[%d]", j);
        if(oyjlTreeGetValue(g, 0, "options"))
        {
          oyjlArgsWebGroupPrintSection_(g, &t, &css_toc_text, gcount, &description, 1, sec);
          if(sec)
            oyjlStringAdd( &t, 0,0, "<form action=\"/result\" method=\"post\" enctype=\"multipart/form-data\">\n" );
          oyjlArgsWebGroupPrint_(val, i, &t, &description, gcount, sec, root, debug, ignore);
          if(sec)
            oyjlStringAdd( &t, 0,0, "<input type=\"submit\" value=\" %s \"></form>\n", _("Send") );
          if(description)
          {
            oyjlStringAdd( &t, 0,0, "%s  </p>\n", description );
            free(description);
            description = NULL;
          }
          ++gcount;
        }
      }
      if(count || has_options)
        oyjlStringAdd( &t, 0,0, "</div>\n" );
    }
    if(n)
      oyjlStringAdd( &t, 0,0, "<br />\n" );
    oyjlStringAdd( &t, 0,0, "</div>\n" );


    if(https_key && https_cert)
    {
      tls_flag = MHD_USE_TLS;
      https_key_pem = oyjlReadFile( https_key, 0, &size );
      if(!https_key_pem)
      {
        https_key_pem = oyjlStringCopy(srv_signed_key_pem,0);
        fprintf(stderr,
                "%s not found. Falling back to server side https_key\n", oyjlTermColor(oyjlRED, https_key));
      }
      https_cert_pem = oyjlReadFile( https_cert, 0, &size );
      if(!https_cert_pem)
      {
        https_cert_pem = oyjlStringCopy(srv_signed_cert_pem,0);
        fprintf(stderr,
                "%s not found. Falling back to server side https_cert\n", oyjlTermColor(oyjlRED, https_cert));
      }
    }
    if(css)
    {
      css_text = oyjlReadFile( css, 0, &size );
      if(!css_text)
      {
        css_text = oyjlStringCopy(oyjl_args_web_normalize_css,0);
        fprintf(stderr,
                "%s not found. Falling back to inbuild default oyjl_args_web_normalize_css\n", oyjlTermColor(oyjlRED, css));
        css = "oyjl_args_web_normalize_css";
      }
    }
    if(css2)
    {
      css2_text = oyjlReadFile( css2, 0, &size );
      if(!css2_text)
      {
        css2_text = oyjlStringCopy(oyjl_args_web_layout_css,0);
        fprintf(stderr,
                "%s not found. Falling back to inbuild default oyjl_args_web_layout_css\n", oyjlTermColor(oyjlRED, css2));
        css2 = "oyjl_args_web_layout_css";
      }
    }
    oyjlStringAdd( &get_page, 0,0, responsepage, OYJL_E(css_text,""), OYJL_E(css2_text,""), OYJL_E(css_toc_text,""), t );
    if(debug)
      oyjlWriteFile("oyjl_args_web-debug.html", get_page, strlen(get_page+1) );
    struct oyjl_mhd_context_struct context = { get_page, sec, "/", callback, argv[0], debug, ignore, css_text, css2_text };
#define OYJL_WEB_OPT_HELP( bold_, txt_, has_args) fprintf( stderr, "      %s%s\t%s\n", oyjlTermColor(oyjlBOLD, bold_), has_args?"=":"", txt_ )
#define OYJL_WEB_ARG_HELP( bold_, txt_ ) fprintf( stderr, "        =%s\t- %s\n", oyjlTermColor(oyjlITALIC, bold_), txt_ )
    if(help)
    {
      const char * arg = "oyjl-list";
      fprintf( stderr, "  %s:\n", oyjlTermColor(oyjlUNDERLINE,_("Help")) );
      fprintf( stderr, "    %s\n\n",
        oyjlTermColor(oyjlBOLD,"--render=web:port=8888:https_key=filename.tls:https_cert=filename.tls:css=first.css:css=second.css:security=level:ignore=o,option") );
      OYJL_WEB_OPT_HELP( "port", "select the port for the host; default is 8888", 1 );
      OYJL_WEB_OPT_HELP( "https_key", "adds a https key file; default is none, if no filename is provided it uses a self certified inbuild key", 1 );
      OYJL_WEB_OPT_HELP( "https_cert", "adds a certificate for https; default is none, if no filename is provided it uses a self certified inbuild certificate", 1 );
      OYJL_WEB_OPT_HELP( "css", "can by called two times to add CSS layout file(s), which will by embedded into the HTML code, if it has no arg, it uses a default", 1 );
      OYJL_WEB_OPT_HELP( "ignore", "add comma separated list of skip options, which are marked in HTML and get not accepted, e.g. ignore=\"o,option,v,verbose",1  );
      OYJL_WEB_OPT_HELP( "help", _("Print help text"), 0 );
      OYJL_WEB_OPT_HELP( "security", "specifies the security level used; default is readonly inactive web page generation", 1 );
      OYJL_WEB_ARG_HELP( "readonly", "is passive and default" );
      OYJL_WEB_ARG_HELP( "interactive", "contains interactive forms element and returns the respond JSON" );
      OYJL_WEB_ARG_HELP( "check_read", "for this level and above set oyjlArgsWebFileNameSecurity() for oyjlFileRead() and calls the specified callback from oyjlArgsRender(callback)" );
      OYJL_WEB_ARG_HELP( "check_write", "for this level and above set oyjlArgsWebFileNameSecurity() for oyjlFileWrite()" );
      OYJL_WEB_ARG_HELP( "check", "for this level and above set oyjlArgsWebFileNameSecurity() for oyjlFileRead() and oyjlFileWrite()" );
      OYJL_WEB_ARG_HELP( "lazy", "calls the specified callback from oyjlArgsRender(callback) unchecked; use only for secured connection with verified login" );
      oyjlArgsWebFileNameSecurity( &arg, 0 );
      fputs( "\n", stderr );
    }

    if(oyjl_args_web_run == 1)
    {
      daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD | tls_flag,
                               port, NULL, NULL,
                               &oyjlMhdAnswerToConnection_cb, &context,
                               /* Optionally, the gnutls_load_file() can be used to
                                  load the key and the certificate from file. */
                               tls_flag?MHD_OPTION_HTTPS_MEM_KEY:MHD_OPTION_HTTPS_PRIORITIES, tls_flag?https_key_pem:"NORMAL",
                               tls_flag?MHD_OPTION_HTTPS_MEM_CERT:MHD_OPTION_HTTPS_PRIORITIES, tls_flag?https_cert_pem:"NORMAL",
                               MHD_OPTION_NOTIFY_COMPLETED, &oyjlMhdRequestCompleted_cb, NULL,
                               MHD_OPTION_END);
      if(NULL == daemon)
      {
        fprintf (stderr,
                 "Failed to start daemon, port:%s\n", oyjlTermColorF( oyjlRED, "%d", port));
        --oyjl_args_web_run;
        return 1;
      }
    }
    fprintf( stderr, "%s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
    fprintf( stderr, "port=%s%s%s%s%s %s ",
        oyjlTermColorF( oyjl_args_web_run==1?oyjlGREEN:oyjlBLUE, "%d", port ),
        https_key?" https_key=":"", https_key?https_key:"",
        https_cert?" https_cert=":"", https_cert?https_cert:"",
        tls_flag?"MHD_USE_TLS":https_key||https_cert?"noTLS:need both https_key and https_cert filenames":"" );
    {
      const char * sec_rules = NULL;
      if(sec>=oyjlSECURITY_CHECK)
        sec_rules = oyjl_args_web_file_name_security_feature;
      if(!sec_rules)
        sec_rules = sec == oyjlSECURITY_LAZY?"lazy":sec==oyjlSECURITY_INTERACTIVE?"interactive":sec==oyjlSECURITY_CHECK?"check":sec==oyjlSECURITY_CHECK_READ?"check_read":sec==oyjlSECURITY_CHECK_WRITE?"check_write":"readonly";
      fprintf( stderr, "security=%s ", oyjlTermColor(oyjlITALIC,sec_rules) );
    }
    fprintf( stderr, "css=%s ", OYJL_E(oyjlTermColor(css_text?oyjlITALIC:oyjlRED,css),"") );
    fprintf( stderr, "css(2)=%s ", OYJL_E(oyjlTermColor(css2_text?oyjlITALIC:oyjlRED,css2),"") );
    fprintf( stderr, "ignore=%s ", OYJL_E(oyjlTermColor(oyjlITALIC,ignore),"") );
    fprintf( stderr, "%s %s\n", oyjl_args_web_run==1?"connect to":"simulate", oyjlTermColorF( oyjl_args_web_run==1?oyjlBOLD:oyjlBLUE, "%s//localhost:%d", tls_flag?"https":"http", port) );

    if(oyjl_args_web_run == 1)
    {
      if(!help)
        (void) getchar ();

      MHD_stop_daemon (daemon);
    }

    oyjlTreeFree( root );
    free(get_page);
    free(t);
    if(https_key_pem) free(https_key_pem);
    if(https_cert_pem) free(https_cert_pem);
    if(css_toc_text) free(css_toc_text);
    if(css_text) free(css_text);
    if(css2_text) free(css2_text);
    if(input) free(input);
  }

  oyjlStringListRelease( &web_pameters_list, web_pameters_list_n, free );
  --oyjl_args_web_run;

  return 0;
}


#ifdef __cplusplus
extern "C" {
#endif
// internal API entry for public API in libOyjlCore
int oyjlArgsWeb_                     ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
    int r = oyjlArgsWebStart__(argc, argv, json, commands, output, debug, ui, callback );
    return r;
}

#if defined(COMPILE_STATIC) && !defined(NO_RENDER_START)
#include "oyjl_tree_internal.h" /* oyjlStringToLower_() */
static int oyjlArgsRendererSelect   (  oyjlUi_s          * ui )
{
  const char * name = NULL;
  char * arg = NULL;
  oyjlOption_s * R;
  int error = -1;

  if( !ui )
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"ui\" argument passed in", OYJL_DBG_ARGS );
    return 1;
  }

  R = oyjlOptions_GetOptionL( ui->opts, "R", 0 );
  if(!R)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" argument found: Can not select", OYJL_DBG_ARGS );
    return 1;
  }

  if(R->variable_type != oyjlSTRING)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" oyjlSTRING variable declared", OYJL_DBG_ARGS );
    return 1;
  }

  arg = oyjlStringCopy( *R->variable.s, NULL );
  if(!arg)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" oyjlSTRING variable found", OYJL_DBG_ARGS );
    return 1;
  }
  else
  {
    if(arg[0])
    {
      if(oyjlStringStartsWith(arg,"gui", OYJL_COMPARE_CASE))
        name = "OyjlArgsQml";
      else
      if(oyjlStringStartsWith(arg,"qml", OYJL_COMPARE_CASE))
        name = "OyjlArgsQml";
      else
      if(oyjlStringStartsWith(arg,"cli", OYJL_COMPARE_CASE))
        name = "OyjlArgsCli";
      else
      if(oyjlStringStartsWith(arg,"web", OYJL_COMPARE_CASE))
        name = "OyjlArgsWeb";
      if(!name)
      {
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s", OYJL_DBG_ARGS, arg );
        return 1;
      }
      if(strcmp(name,"OyjlArgsWeb") == 0)
        error = 0;
      else
      if(strcmp(name,"OyjlArgsCli") == 0)
        error = -2;
      else
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s", OYJL_DBG_ARGS, arg );
    }
    else /* report all available renderers */
    {
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsWeb available - option -R=\"cli\"", OYJL_DBG_ARGS );
    }
    free(arg); arg = NULL;
  }

  return error;
}
int oyjlArgsCli_                     ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv));
// public API for liboyjl-args-web-static.a
int oyjlArgsRender                   ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
  int result = 1;
  int select = oyjlArgsRendererSelect(ui);
  if(select == 0)
    result = oyjlArgsWeb_(argc, argv, json, commands, output, debug, ui, callback );
  else if(select == -2)
    result = oyjlArgsCli_(argc, argv, json, commands, output, debug, ui, callback );
  fflush(stdout);
  fflush(stderr);
  return result;
}

#endif /* COMPILE_STATIC */

#ifdef __cplusplus
} /* extern "C" */
#endif

/** @file oyjl_args_web.c
 *
 *  Oyjl Args Web is a http server renderer of UI files.
 *
 *  @par Copyright:
 *            2021-2022 (C) Kai-Uwe Behrmann
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


static int
oyjlMhdSendPage (struct MHD_Connection *connection,
           const char *page,
           int status_code)
{
  int ret;
  struct MHD_Response *response;

  response =
    MHD_create_response_from_buffer (strlen (page),
                                     (void *) page,
				     MHD_RESPMEM_MUST_COPY);
  if(!response)
    return MHD_NO;
  MHD_add_response_header (response,
                           MHD_HTTP_HEADER_CONTENT_TYPE,
                           "text/html");
  ret = MHD_queue_response (connection,
                            status_code,
                            response);
  MHD_destroy_response (response);
  fprintf(stderr, "page: %s status_code: %d\n", oyjlTermColorFromHtml(page,0), status_code );

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


  fprintf(stderr,"key:%s filename:%s content_type:%s data:%s size:%lu\n", key, OYJL_E(filename,""), OYJL_E(content_type,""), OYJL_E(data,""), size );
  if(size == 0)
    return MHD_YES;

  if(!con_info->answernode) con_info->answernode = oyjlTreeNew("");

  if(0 != strcmp (key, "file"))
  {
    con_info->answerstring = servererrorpage;
    con_info->answercode = MHD_HTTP_BAD_REQUEST;
    oyjlTreeSetStringF( con_info->answernode, OYJL_CREATE_NEW, data, key );
    return MHD_YES;
  } else
  {
    oyjlTreeSetStringF( con_info->answernode, OYJL_CREATE_NEW, data, "%s/data", key );
    oyjlTreeSetStringF( con_info->answernode, OYJL_CREATE_NEW, filename, "%s/filename", key );
  }

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
    con_info->fp = fopen (filename, "ab");
    if(!con_info->fp)
    {
      con_info->answerstring = fileioerror;
      con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
      return MHD_YES;
    }
  }

  if(size > 0)
  {
    if(! fwrite (data, sizeof (char), size, con_info->fp))
    {
      con_info->answerstring = fileioerror;
      con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
      return MHD_YES;
    }
  }

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

  free (con_info);
  *con_cls = NULL;
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
  (void)cls;               /* Unused. Silent compiler warning. */
  (void)url;               /* Unused. Silent compiler warning. */
  (void)version;           /* Unused. Silent compiler warning. */

  if (NULL == *con_cls)
  {
    /* First call, setup data structures */
    struct oyjl_mhd_connection_info_struct *con_info;

    if(oyjl_nr_of_uploading_clients >= MAXCLIENTS)
      return oyjlMhdSendPage (connection,
                        busypage,
                        MHD_HTTP_SERVICE_UNAVAILABLE);

    con_info = calloc (sizeof (struct oyjl_mhd_connection_info_struct), 1);
    if(NULL == con_info) return MHD_NO;

    con_info->answercode = 0; /* none yet */
    con_info->fp = NULL;

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
                      cls,
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
      char * answerstring = oyjlTreeToText( con_info->answernode, OYJL_JSON );
      con_info->answerstring = oyjlTermColorToHtml( answerstring, 0 );
      free(answerstring); answerstring = NULL;
      int ret = oyjlMhdSendPage(connection, con_info->answerstring, con_info->answercode);
      fprintf(stderr,  "oyjlMhdSendPage: %d\n", ret );
      return ret;
      
    } else
      return oyjlMhdSendPage(connection,
                        con_info->answerstring,
                        con_info->answercode);
  }

  /* Note a GET or a POST, generate error */
  return oyjlMhdSendPage(connection,
                    errorpage,
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

#define OYJL_HTML                      0x100
extern char * oyjl_term_color_html_;
const char * oyjlStringColor         ( oyjlTEXTMARK_e      mark,
                                       int                 flags,
                                       const char        * format,
                                                           ... )
{
  char * tmp = NULL;
  const char * t = NULL,
             * text = format;

  if(strchr(format, '%'))
  { OYJL_CREATE_VA_STRING(format, tmp, malloc, return NULL)
    text = tmp;
  }

  if(flags & OYJL_HTML)
  {
    if(oyjl_term_color_html_) free(oyjl_term_color_html_);
    oyjl_term_color_html_ = NULL;
    switch(mark)
    {
      case oyjlNO_MARK: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "%s", text ); break;
      case oyjlRED: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<font color=red>%s</font>", text ); break;
      case oyjlGREEN: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<font color=green>%s</font>", text ); break;
      case oyjlBLUE: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<font color=blue>%s</font>", text ); break;
      case oyjlBOLD: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<strong>%s</strong>", text ); break;
      case oyjlITALIC: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<em>%s</em>", text ); break;
      case oyjlUNDERLINE: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<u>%s</u>", text ); break;
    }
    t = oyjl_term_color_html_;
  }
  else
    t = oyjlTermColor( mark, text );

  if(tmp) free(tmp);
  return t;
}

int  oyjlArgsWebGroupIsMan_          ( oyjl_val            g )
{
  oyjl_val first_o = oyjlTreeGetValue(g, 0, "options/[0]/option");
  const char * txt = OYJL_GET_STRING(first_o);
  if(txt && strlen(txt) > 4 && memcmp( txt, "man-", 4 ) == 0)
    return 1;
  return 0;
}

typedef enum {
  oyjlSECURITY_READONLY,
  oyjlSECURITY_INTERACTIVE,
  oyjlSECURITY_LAZY
} oyjlSECURITY_e;

void oyjlArgsWebGroupPrintSection_   ( oyjl_val            g,
                                       char             ** t_,
                                       char             ** css_toc_text,
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
  if(gname || gdesc || ghelp || synopsis)
  {
    if(gname)
    {
      oyjlStringAdd( css_toc_text, 0,0, "#h%did:focus ~ p#h%ddescription { display: block; }\n", gcount, gcount );
      oyjlStringAdd( &t, 0,0, "  <h3 id=\"h%did\" tabindex=\"0\">%s</h3>", gcount, gname );
    }
    oyjlStringAdd( description, 0,0, "  <p id=\"h%ddescription\"%s>", gcount, sec?" class=\"hide\"":"" );
    if(gdesc)
      oyjlStringAdd( description, 0,0, "%s%s", gdesc, ghelp?"<br />\n":synopsis?"<br />\n<br />\n":"" );
    if(ghelp)
      oyjlStringAdd( description, 0,0, "%s%s", ghelp, synopsis?"<br />\n<br />\n":"" );
    if(synopsis)
      oyjlStringAdd( description, 0,0, "%s<br />\n<br />\n", synopsis );
  }
  *t_ = t;
}

void oyjlArgsWebGroupPrint_          ( oyjl_val            g,
                                       char             ** t_,
                                       char             ** description,
                                       int                 gcount,
                                       oyjlSECURITY_e      sec )
{
  oyjl_val v;
  const char * txt;
  char * t = *t_;

  int j, count;
  v = oyjlTreeGetValue(g, 0, "options");
  count = oyjlValueCount( v );
  for(j = 0; j < count; ++j)
  {
    int k, kn = 0, is_choice = 0;
    const char * key, * name, * desc, * help, * value_name/*, * default_var*/, * type, * no_dash;
    char * text = NULL;
    oyjl_val opt = oyjlTreeGetValueF(v, 0, "[%d]", j);
    oyjl_val o = oyjlTreeGetValue(opt, 0, "option");
    oyjl_val choices;
    txt = OYJL_GET_STRING(o);
    if(txt && strlen(txt) > 4 && memcmp( txt, "man-", 4 ) == 0)
      continue;
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
    /*o = oyjlTreeGetValue(opt, 0, "default");
    default_var = OYJL_GET_STRING(o);*/
    o = oyjlTreeGetValue(opt, 0, "type");
    type = OYJL_GET_STRING(o);
    o = oyjlTreeGetValue(opt, 0, "no_dash");
    no_dash = OYJL_GET_STRING(o);
    if(key[0] == '@')
      no_dash = "1";

    choices = oyjlTreeGetValue(opt, 0, "choices");
    oyjlStringAdd( description, 0,0, "%s%s[%s%s%s%s]%s%s%s%s<br />\n",
        name?oyjlStringColor(oyjlBOLD, OYJL_HTML,name):"", name?"&nbsp;&nbsp;":"",
        no_dash?"":strlen(key) == 1?"-":"--", key[0] == '@'?"":key,
        value_name && key[0] != '@'?"=":"", value_name?text:"",
        desc?" : ":"", desc?desc:"",
        help&&help[0]?" - ":"", help&&help[0]?help:"" );

    if(type && strcmp(type,"bool") == 0)
    {
      if(sec)
        oyjlStringAdd( &t, 0,0, "  <label for=\"%s-%d\">%s</label><input type=\"checkbox\" name=\"%s\" id=\"%s-%d\" value=\"true\" checked=\"true\"/><br />\n",
         /*label for id*/key, gcount, name /*i18n label*/, key /*name*/, key/* id */, gcount );
    }
    else
    {
      kn = oyjlValueCount( choices );
      if(kn && sec )
      {
        if(type && strcmp(type,"choice") == 0)
        {
          is_choice = 1;
  	      oyjlStringAdd( &t, 0,0, "  <label for=\"%s-%d\">%s</label>\n  <select id=\"%s-%d\" name=\"%s\">\n", key, gcount, name?name:"", key, gcount, key );
        }
        else if(type && strcmp(type,"string") == 0)
        {
          is_choice = 2;
  	      oyjlStringAdd( &t, 0,0, "  <label for=\"%s-%d\">%s</label>\n  <input id=\"%s-%d\" list=\"%s-%d-states\" name=\"%s\" />\n\
    <datalist id=\"%s-%d-states\">\n", key, gcount, name?name:"", key, gcount, key, gcount, key, key, gcount );
        }
      } else if(type && strcmp(type,"string") == 0)
      {
        is_choice = 2;
  	    oyjlStringAdd( &t, 0,0, "  <label for=\"%s-%d\">%s</label>\n  <input id=\"%s-%d\" name=\"%s\" /><br />\n", key, gcount, name?name:"", key, gcount, key );
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
      cv = oyjlTreeGetValue(c, 0, "name");
      name = OYJL_GET_STRING(cv);
      cv = oyjlTreeGetValue(c, 0, "nick");
      nick = OYJL_GET_STRING(cv);
      cv = oyjlTreeGetValue(c, 0, "description");
      desc = OYJL_GET_STRING(cv);
      cv = oyjlTreeGetValue(c, 0, "help");
      help = OYJL_GET_STRING(cv);
      if(sec)
      {
        if(is_choice == 1)
	        oyjlStringAdd( &t, 0,0, "    <option value=\"%s\">%s</option>\n", nick, name?name:nick );
        else if(is_choice == 2)
	        oyjlStringAdd( &t, 0,0, "      <option value=\"%s\">%s</option>\n", nick, name?name:nick );
      }
      oyjlStringAdd( description, 0,0, "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s%s%s%s%s%s%s%s%s%s<br />\n", no_dash?"":strlen(key) == 1?"-":"--", key[0] != '@'?key:"",
        nick && key[0] != '@'?" ":"", nick?nick:"",
        name&&name[0]?"    # ":"", name?name:"",
        desc?" : ":"", desc&&desc[0]?desc:"",
        help&&help[0]?" - ":"", help&&help[0]?help:"" );
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
  }
  *t_ = t;
}

int oyjlArgsWebStart__               ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands OYJL_UNUSED,
                                       const char        * output OYJL_UNUSED,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv) OYJL_UNUSED)
{
  char * input = NULL;
  char * t = NULL;
  int i;
  int port = PORT;
  char * https_key = NULL,
       * https_cert = NULL,
       * css = NULL,
       * css2 = NULL;
  oyjlSECURITY_e sec = oyjlSECURITY_READONLY;

  if(debug)
  {
    for(i = 0; i < argc; ++i)
      fprintf( stderr, "%s ", argv[i] );
    fprintf( stderr, "\n" );
  }

  oyjl_val root = NULL;
  char error_buffer[256] = {0};
  int r = 0;
  if( json && strlen( json ) )
  {
    r = oyjlIsFile( json, "r", NULL, 0 );
    if(!r && oyjlDataFormat(json) == 7)
    {
      root = oyjlTreeParse( json, error_buffer, 256 );
      if(error_buffer[0] != '\000')
      {
        fprintf(stderr, "ERROR:\t\"%s\"\n", error_buffer);
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
    int list_n = 0;
    char ** list;
    oyjlOptions_GetResult( ui->opts, "R", &web_pameters, 0,0 );
    if(web_pameters)
    {
      long lo = 8888;
      list = oyjlStringSplit( web_pameters, ':', &list_n, 0 );

      for(i = 1 /* zero param is "web" */; i < list_n; ++i)
      {
        char * param = list[i],
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
              (arg && strcasecmp(param,param_) == 0) ) ) \
          txt_ = arg?arg:param; }
        OYJL_SUB_ARG_STRING( "https_key", 0, https_key )
        OYJL_SUB_ARG_STRING( "https_cert", 0, https_cert )
        OYJL_SUB_ARG_STRING( "security", 0, security )
        if(security)
        {
          if(strcasecmp(security,"readonly") == 0)
            sec = oyjlSECURITY_READONLY;
          else
          if(strcasecmp(security,"interactive") == 0)
            sec = oyjlSECURITY_INTERACTIVE;
          else
          if(strcasecmp(security,"lazy") == 0)
            sec = oyjlSECURITY_LAZY;
        }
        if(css)
          OYJL_SUB_ARG_STRING( "css", 0, css2 )
        else
          OYJL_SUB_ARG_STRING( "css", 0, css )
      }
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
    json = oyjlUi_ToJson( ui, 0 ); // generate JSON from ui data struct
    char * merged = NULL;
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
    if(merged)
      free(merged);
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
    struct MHD_Daemon * daemon;
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
    oyjlStringAdd( &t, 0,0, "<h1 id=\"h1intro\" tabindex=\"0\">%s</h1>\n", OYJL_E(OYJL_E(name,OYJL_E(nick,type)),type) );
    if((name && nick) || version || desc || docu)
    {
      oyjlStringAdd( &t, 0,0, "<div class=\"text\">\n\
  <p%s id=\"introdescription\">\n", sec?" class=\"hide\"":"" );
      oyjlStringAdd( &t, 0,0, "%s: ", type );
      if(name && nick)
        oyjlStringAdd( &t, 0,0, "%s", oyjlStringColor(oyjlBOLD, OYJL_HTML, nick) );
      if(version)
        oyjlStringAdd( &t, 0,0, " v%s", oyjlStringColor(oyjlITALIC, OYJL_HTML, version) );
      if(docu)
        oyjlStringAdd( &t, 0,0, " %s", docu );
      oyjlStringAdd( &t, 0,0, "\n  </p>\n" );
      oyjlStringAdd( &t, 0,0, "</div>\n" );
    }
    oyjlStringAdd( &t, 0,0, "<div class=\"tiles\">\n" );
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
        oyjlArgsWebGroupPrint_(g, &t, &description, gcount, sec);
        if(sec)
          oyjlStringAdd( &t, 0,0, "  <input type=\"submit\" value=\" Send \"></form>\n" );
        if(description)
        {
          oyjlStringAdd( &t, 0,0, "%s  </p>\n", description );
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
          oyjlArgsWebGroupPrint_(g, &t, &description, gcount, sec);
          if(sec)
            oyjlStringAdd( &t, 0,0, "<input type=\"submit\" value=\" Send \"></form>\n" );
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
      https_key_pem = oyjlReadFile( https_key, &size );
      if(!https_key_pem)
      {
        https_key_pem = oyjlStringCopy(srv_signed_key_pem,0);
        fprintf(stderr,
                "%s not found. Falling back to server side https_key\n", oyjlTermColor(oyjlRED, https_key));
      }
      https_cert_pem = oyjlReadFile( https_cert, &size );
      if(!https_cert_pem)
      {
        https_cert_pem = oyjlStringCopy(srv_signed_cert_pem,0);
        fprintf(stderr,
                "%s not found. Falling back to server side https_cert\n", oyjlTermColor(oyjlRED, https_cert));
      }
    }
    if(css)
      css_text = oyjlReadFile( css, &size );
    if(css2)
      css2_text = oyjlReadFile( css2, &size );
    oyjlStringAdd( &get_page, 0,0, "\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\">\n\
<head>\n\
  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\
  <meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n\
  <style type=\"text/css\">%s%s%scode{white-space: pre;}\n\
  #h1intro:focus ~ div p#introdescription { display: block; }\n\
  </style>\n\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
</head><body>\n\
%s</body></html>", OYJL_E(css_text,""), OYJL_E(css2_text,""), OYJL_E(css_toc_text,""), t );
    if(debug)
      oyjlWriteFile("oyjl_args_web-debug.html", get_page, strlen(get_page+1) );
    daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD | tls_flag,
                               port, NULL, NULL,
                               &oyjlMhdAnswerToConnection_cb, get_page,
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
      return 1;
    }
    fprintf( stderr, "port:%s%s%s%s%s %s ",
        oyjlTermColorF( oyjlGREEN, "%d", port ),
        https_key?" https_key:":"", https_key?https_key:"",
        https_cert?" https_cert:":"", https_cert?https_cert:"",
        tls_flag?"MHD_USE_TLS":https_key||https_cert?"noTLS:need both https_key and https_cert filenames":"" );
    fprintf( stderr, "sec:%s ", oyjlTermColor(oyjlITALIC,sec == oyjlSECURITY_LAZY?"lazy":sec==oyjlSECURITY_INTERACTIVE?"interactive":"readonly" ) );
    fprintf( stderr, "css:%s ", OYJL_E(oyjlTermColor(css_text?oyjlITALIC:oyjlRED,css),"") );
    fprintf( stderr, "css2:%s ", OYJL_E(oyjlTermColor(css2_text?oyjlITALIC:oyjlRED,css2),"") );
    fprintf( stderr, "connect to %s\n", oyjlTermColorF( oyjlBOLD, "%s//localhost:%d", tls_flag?"https":"http", port) );

    (void) getchar ();

    MHD_stop_daemon (daemon);
  }

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

#ifdef COMPILE_STATIC
#include "oyjl_tree_internal.h" /* oyjlStringToLower() */
static int oyjlArgsRendererSelect   (  oyjlUi_s          * ui )
{
  const char * arg = NULL, * name = NULL;
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
      char * low = oyjlStringToLower( arg );
      if(low)
      {
        if(strlen(low) >= strlen("gui") && memcmp("gui",low,strlen("gui")) == 0)
          name = "OyjlArgsQml";
        else
        if(strlen(low) >= strlen("qml") && memcmp("qml",low,strlen("qml")) == 0)
          name = "OyjlArgsQml";
        else
        if(strlen(low) >= strlen("cli") && memcmp("cli",low,strlen("cli")) == 0)
          name = "OyjlArgsCli";
        else
        if(strlen(low) >= strlen("web") && memcmp("web",low,strlen("web")) == 0)
          name = "OyjlArgsWeb";
        if(!name)
        {
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s|%s", OYJL_DBG_ARGS, arg,low );
          free(low);
          return 1;
        }
        if(strcmp(name,"OyjlArgsWeb") == 0)
          error = 0;
        else
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s|%s", OYJL_DBG_ARGS, arg,low );
        free(low);
      }
    }
    else /* report all available renderers */
    {
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsWeb available - option -R=\"cli\"", OYJL_DBG_ARGS );
    }
  }

  return error;
}
// public API for liboyjl-args-cli-static.a
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
  if(oyjlArgsRendererSelect(ui) == 0)
    result = oyjlArgsWeb_(argc, argv, json, commands, output, debug, ui, callback );
  fflush(stdout);
  fflush(stderr);
  return result;
}

#endif /* COMPILE_STATIC */

#ifdef __cplusplus
} /* extern "C" */
#endif

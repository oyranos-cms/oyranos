#include <stdlib.h>
#include <stdio.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

#include "oyjl_version.h"
#include "oyjl.h"

int number_detection = 0;
int main(int argc, char *argv[])
{
    int number;
    int start = 0;
    int error = 0;

    if(argc > 1 && strcmp(argv[1],"-n") == 0)
    {
      fprintf(stderr, "skip number detection\n");
      number_detection = 0;
      ++start;
    }

    if (argc-start < 2) {
        printf("Usage: [-n (skip number detection)] %s file1.yaml ...\n", argv[start]);
        return 0;
    }

    for (number = start + 1; number < argc && !error; number ++)
    {
        FILE *fp;

        char * json = NULL, * text = NULL;
        oyjl_val jroot = NULL;
        char error_buffer[256] = {0};
        int level = 0;
        int size = 0;

        if(strcmp(argv[number],"-") == 0)
          fp = stdin;
        else
          fp = fopen(argv[number], "rb");
        assert(fp);

        text = oyjlReadFileStreamToMem( fp, &size );

        jroot = oyjlTreeParseYaml( text, number_detection ? OYJL_NUMBER_DETECTION : 0, error_buffer, 256 );

        if(error_buffer[0] != 0)
          fprintf( stderr, "%s: %s\n", argv[number], "" );

        /* reformat */
        oyjlTreeToJson( jroot, &level, &json );
        if( json )
          puts( json );

        if(fp != stdin)
          assert(!fclose(fp));
    }

    return error;
}

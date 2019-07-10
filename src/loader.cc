//-*- C++ -*-

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

#include "defs.h"
#include "core.h"

void print_usage() {
    printf("Usage: loader [OPTIONS]\n\n");
    printf("  -i infile.wav             input WAV file\n");
    printf("  -o outfile.tzx            output TZX file (default 'out.tzx')\n");
    printf("  -h, --help                print this help and exit\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    char CONFIG_FILE[256];

    const char* audio_file = nullptr;
    const char* tzx_file = "out.tzx";

    const char*   short_opt = "i:o:";
    struct option long_opt[] = {
        {"help",          no_argument,       NULL, 'h'},
        {NULL,            0,                 NULL, 0  }
    };

    int c;

    while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch(c) {
        case -1:       /* no more arguments */
        case 0:        /* long options toggles */
            break;

        case 'i':
            audio_file = optarg;
            break;

        case 'o':
            tzx_file = optarg;
            break;

        case 'h':
            print_usage();
            return(0);

        case ':':
        case '?':
            fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
            return(-2);

        default:
            fprintf(stderr, "%s: invalid option -- %c\n", argv[0], c);
            fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
            return(-2);
        };
    };

    if (!audio_file) {
        print_usage();
        return 0;
    }

    snprintf(CONFIG_FILE, sizeof(CONFIG_FILE) - 1, "%s/.config/loader/loader.conf", getenv("HOME"));

    Core core(tzx_file, audio_file);
    bool ok = core.run();

    if (ok) {
        printf("Result written to output file '%s'\n", tzx_file);
    }

    return ok ? 0 : -1;
}

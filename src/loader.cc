//-*- C++ -*-

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

#include "defs.h"
#include "config.h"
#include "core.h"

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
            printf("Usage: %s [OPTIONS]\n\n", argv[0]);
            printf("  -i infile.wav             input WAV file (read from '/dev/dsp' if omitted)\n");
            printf("  -o outfile.tzx            output TZX file\n");
            printf("  -h, --help                print this help and exit\n");
            printf("\n");
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

    snprintf(CONFIG_FILE, sizeof(CONFIG_FILE) - 1, "%s/.config/loader/loader.conf", getenv("HOME"));

    Config conf; // new configuration object.

    // if config file doesn't exists, i will create it.
    FILE* fp = fopen(CONFIG_FILE, "r");
    if (!fp) {

        char config_dir[256];
        snprintf(config_dir, sizeof(config_dir) - 1, "%s/.config/loader/", getenv("HOME"));
        printf("creating directory %s ...\n", config_dir);
        mkdir(config_dir, 0777); // creo el directorio.
        printf("creating file %s ...\n", CONFIG_FILE);

        conf.saveConfigFile(CONFIG_FILE);

        printf("ok\n");
    } else {
        fclose(fp);
        conf.parseConfigFile(CONFIG_FILE);
    }

    Core C(&conf, tzx_file, audio_file);
    bool ok = C.run();

    if (ok) {
        printf("Result written to output file '%s'\n", tzx_file);
    }

    return ok ? 0 : -1;
}

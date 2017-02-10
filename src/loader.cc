//-*- C++ -*-

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "defs.h"
#include "config.h"
#include "core.h"

char CONFIG_FILE[100];

int main(int argc, char *argv[])
{
  /*  bindtextdomain (PACKAGE, LOCALEDIR);
      textdomain (PACKAGE);*/

  char* audio_file = NULL;

  if ((argc > 3) || (argc == 1))  {
    printf("\nussage: loader out_file.tzx [in_file.wav]\n\n");
    return -1;
  } else if (argc > 2) {
    audio_file = argv[2];
  }

  sprintf(CONFIG_FILE, "%s/%s", getenv("HOME"), CONFIG_FILE_HOME);

  Config conf; // new configuration object.

  // if config file doesn't exists, i will create it.
  FILE* fp;
  if ((fp = fopen(CONFIG_FILE, "r")) == NULL) {
    
    char config_dir[100];
    sprintf(config_dir, "%s/.loader/", getenv("HOME"));
    printf("creating directory %s ...\n", config_dir);
    mkdir(config_dir, 0777); // creo el directorio.
    printf("creating file %s ...\n", CONFIG_FILE);
    
    conf.saveConfigFile(CONFIG_FILE);
    
    printf("ok\n");
    
  } else {
    fclose(fp);
    conf.parseConfigFile(CONFIG_FILE);
  }

  Core C(&conf, argv[1], audio_file);
  C.start();

  return 0;
}

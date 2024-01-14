/*
 * portdbc - CRUX's portdb command line client
 *
 * Copyright 2010-2024 Jose Beneyto
 *
 * Licensed under GPLv2, see file COPYING in this source tree.
 */

#include "defines.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

int main(int argc, char** argv)
{

  xmlDoc *doc = NULL;
  xmlNode *r_node = NULL;

  char *url = NULL;
  char *portdb_url = "https://crux.nu/portdb/";
  char *tmpfile = "/tmp/.portdbc.xml";
  char *configfile = "/tmp/.portdbc.conf";

  FILE *file_handle = NULL;

  // use HOME directory when available
  char *home_dir = NULL;
  home_dir = getenv("HOME");

  if (home_dir != NULL)
  {
    asprintf(&tmpfile, "%s/.portdbc.xml", home_dir);
    asprintf(&configfile, "%s/.portdbc.conf", home_dir);
    // try to read config values
    if ((file_handle = fopen(configfile, "r+")))
    {
      char line[256], option[256], value[256];
      int linenum=0;
      while(fgets(line, 256, file_handle) != NULL)
      {
        linenum++;
        // ignore comments from config file
        if(line[0] == '#') continue;
        // try to read the pair 'option = value'
        if(sscanf(line, "%s = %s", option, value) != 2)
        {
          //fprintf(stderr, "Invalid syntax, line %d\n", linenum);
          continue;
        }
        //fprintf(stdout, "%-2d: %s = %s\n", linenum, option, value);
        if(strcmp(option, "portdb_url") == 0)
        {
          asprintf(&portdb_url, "%s", value);
        }
      }
      //fprintf(stdout, "portdb_url: %s\n", portdb_url);
    }
  }

  int command = NOCMD;

  if (argc > 1)
  {
    if (strcmp("repos", argv[1]) == 0)
    {
      asprintf(&url, "%s?f=xml", portdb_url);
      command = CMD_REPOS;
    }
    else if (strcmp("dups", argv[1]) == 0)
    {
      asprintf(&url, "%s?f=xml&a=dups", portdb_url);
      command = CMD_DUPS;
    }
    else if (strcmp("search", argv[1]) == 0 && argc > 2)
    {
      asprintf(&url, "%s?f=xml&a=search&q=%s", portdb_url, argv[2]);
      command = CMD_SEARCH;
    }
    else if (strcmp("list", argv[1]) == 0 && argc > 2)
    {
      asprintf(&url, "%s?f=xml&a=repo&q=%s", portdb_url, argv[2]);
      command = CMD_LIST;
    }
    else if (strcmp("getup", argv[1]) == 0 && argc > 2)
    {
      asprintf(&url, "%s?a=getup&q=%s", portdb_url, argv[2]);
      command = CMD_GETUP;
      // at this point we should print the output since it is not an xml
      printHttpFile(url);
      // and exit
      return 0;
    }
    else if (strcmp("version", argv[1]) == 0)
    {
      printVersion();
      return 0;
    }
    else
    {
      printUsage();
      return 0;
    }
  }
  else
  {
    printUsage();
    return 0;
  }

  saveHttpFile(url, tmpfile);
  free(url);

  doc = xmlReadFile(tmpfile, NULL, 0);

  if (doc == NULL)
  {
    fprintf(stderr, "Error, could not parse file %s\n", tmpfile);
    return -1;
  }

  // get the root element node
  r_node = xmlDocGetRootElement(doc);

  switch (command)
  {
    case CMD_REPOS:
      // validate as portdb xml compatible
      if (!r_node || !r_node->name || xmlStrcmp(r_node->name, (const xmlChar *) "repos"))
      {
        xmlFreeDoc(doc);
        fprintf(stderr, "Error, temporary file not compatible\n");
        return -1;
      }
      // print all repositories
      listRepos(r_node);
      break;

    case CMD_DUPS:
      listDups(r_node);
      break;

    case CMD_SEARCH:
    case CMD_LIST:
      listPorts(r_node);
      break;
  }

  // free
  xmlFreeDoc(doc);

  // free global variables allocated by the parser
  xmlCleanupParser();

  // remove temporary file
  remove(tmpfile);

  return 0;
}

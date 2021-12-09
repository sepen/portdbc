/*
 * portdbc - CRUX's portdb command line client
 *
 * Copyright 2010-2019 Jose V Beneyto, <sepen@crux.nu>
 *
 * Licensed under GPLv2, see file COPYING in this source tree.
 */

#define _GNU_SOURCE

#include "defines.h"

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

int printHttpFile(char *url)
{
  CURL *curl_handle;

  /* init the curl session */
  curl_handle = curl_easy_init();

  if(curl_handle)
  {
    /* set URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* no progress meter please */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeCallback);

    /* 302 http -> https  */
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

    /* get it! */
    curl_easy_perform(curl_handle);

    /* always cleanup */
    curl_easy_cleanup(curl_handle);
  }
  return 0;
}
 
int saveHttpFile(char *url, char *filename)
{
  CURL *curl_handle;
  FILE *file_handle;

  /* init the curl session */
  curl_handle = curl_easy_init();

  if(curl_handle)
  {
    /* set URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* no progress meter please */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeCallback);

    /* check for datafile */
    file_handle = fopen(filename,"w");
    if (file_handle == NULL) {
      curl_easy_cleanup(curl_handle);
      return -1;
    }

    /* save results */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, file_handle);

    /* get it! */
    curl_easy_perform(curl_handle);

    /* always cleanup */
    curl_easy_cleanup(curl_handle);

    /* close file handle */
    fclose(file_handle);
  }
  return 0;
}

void listRepos(xmlNode *r_node)
{
  xmlNode *cur_node = NULL;
  xmlNode *child_node = NULL;

  char *name = NULL;
  char *ports = NULL;
  char *type = NULL;
  char *maintainer = NULL;
  char *url = NULL;

  // iterate all "repo" nodes
  for (cur_node = r_node->children; cur_node != NULL; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE && xmlStrcmp(cur_node->name, (const xmlChar *) "repo") == 0)
    {
      // interate all child nodes of "repo"
      for (child_node = cur_node->children; child_node != NULL; child_node = child_node->next)
      {
        if (child_node->type == XML_ELEMENT_NODE )
        {
          if (xmlStrcmp(child_node->name, (const xmlChar *) "name") == 0)
          {
            name = strdup((char *) child_node->children->content);
          }
          else if (xmlStrcmp(child_node->name, (const xmlChar *) "ports") == 0)
          {
            ports = strdup((char *) child_node->children->content);
          }
          else if (xmlStrcmp(child_node->name, (const xmlChar *) "type") == 0)
          {
            type = strdup((char *) child_node->children->content);
          }
          else if (xmlStrcmp(child_node->name, (const xmlChar *) "maintainer") == 0)
          {
            maintainer = strdup((char *) child_node->children->content);
          }
          else if (xmlStrcmp(child_node->name, (const xmlChar *) "url") == 0)
          {
            url = strdup((char *) child_node->children->content);
          }
        }
      }
      // print repo line
      printf("%-14s %-4s %-7s %-40s %s\n", NONULL(name), NONULL(ports), NONULL(type), NONULL(maintainer), NONULL(url));
      // and free memory
      FREENULL(name)
      FREENULL(ports)
      FREENULL(type)
      FREENULL(maintainer)
      FREENULL(url)
    }
  }
}

void listDups(xmlNode *r_node)
{
  xmlNode *cur_node = NULL;
  xmlNode *child_node = NULL;

  char *name = NULL;
  char *count = NULL;

  // iterate all "duplicate" nodes
  for (cur_node = r_node->children; cur_node != NULL; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE && xmlStrcmp(cur_node->name, (const xmlChar *) "duplicate") == 0)
    {
      // iterate all child nodes of "duplicate"
      for (child_node = cur_node->children; child_node != NULL; child_node = child_node->next)
      {
        if (child_node->type == XML_ELEMENT_NODE)
        {
          if (xmlStrcmp(child_node->name, (const xmlChar *) "name") == 0)
          {
            name = strdup((char *) child_node->children->content);
          }
          else if (xmlStrcmp(child_node->name, (const xmlChar *) "count") == 0)
          {
            count = strdup((char *) child_node->children->content);
          }
        }
      }
      // print dup line
      printf("%-28s %s\n", NONULL(name), NONULL(count));
      // and free memory
      FREENULL(name)
      FREENULL(count)
    }
  }
}

void listPorts(xmlNode *r_node)
{
  xmlNode *cur_node = NULL;
  xmlNode *child_node = NULL;

  char *name = NULL;
  char *repo = NULL;
  char *command = NULL;

  // iterate all "port" nodes
  for (cur_node = r_node->children; cur_node != NULL; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE && xmlStrcmp(cur_node->name, (const xmlChar *) "port") == 0)
    {
      // iterate all child nodes of "port"
      for (child_node = cur_node->children; child_node != NULL; child_node = child_node->next)
      {
        if (child_node->type == XML_ELEMENT_NODE)
        {
          if (xmlStrcmp(child_node->name, (const xmlChar *) "name") == 0)
          {
            name = strdup((char *) child_node->children->content);
          }
          else if (xmlStrcmp(child_node->name, (const xmlChar *) "repo") == 0)
          {
            repo = strdup((char *) child_node->children->content);
          }
          else if (xmlStrcmp(child_node->name, (const xmlChar *) "command") == 0)
          {
            command = strdup((char *) child_node->children->content);
          }
        }
      }
      // print port line
      printf("%-28s %-14s %s\n", NONULL(name), NONULL(repo), NONULL(command));
      // and free memory
      FREENULL(name)
      FREENULL(repo)
      FREENULL(command)
    }
  }
}

void printUsage()
{
  printf("Usage: portdbc <command> [arg]\n");
  printf("Where commands are:\n");
  printf("  repos             Overview of all available repositories\n");
  printf("  dups              List of duplicated ports\n");
  printf("  search  <expr>    Show port names containing 'expr'\n");
  printf("  list    <repo>    Show ports in repository\n");
  printf("  getup   <repo>    Print httpup/rsync file for a repository\n");
  printf("  help              Show this help information\n");
  printf("  version           Show version information\n");
}

void printVersion()
{
  printf("%s\n", VERSION_STRING);
}

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

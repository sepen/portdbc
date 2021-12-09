#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
  int written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

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
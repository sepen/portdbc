#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define CMD_REPOS  1
#define CMD_DUPS   2
#define CMD_SEARCH 3
#define CMD_LIST   4
#define CMD_GETUP  5 // not used atm.

#define VERSION_STRING "portdbc 1.0 by Jose V Beneyto, <sepen@crux.nu>"

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

  char *name, *ports, *type, *maintainer, *url = NULL;

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
      printf("%-14s %-4s %-7s %-40s %s\n", name, ports, type, maintainer, url);
    }
  }
}

void listDups(xmlNode *r_node)
{
  xmlNode *cur_node = NULL;
  xmlNode *child_node = NULL;

  char *name, *count = NULL;

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
      printf("%-28s %s\n", name, count);
    }
  }
}

void listPorts(xmlNode *r_node)
{
  xmlNode *cur_node = NULL;
  xmlNode *child_node = NULL;

  char *name, *repo, *command = NULL;

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
      printf("%-28s %-14s %s\n", name, repo, command);
    }
  }
}

void printUsage()
{
  printf("%s\n", VERSION_STRING);
  printf("Usage: portdbc <command> <arg>\n");
  printf("Where commands are:\n");
  printf("  repos            Overview of available repositories\n");
  printf("  dups             List of duplicated ports\n");
  printf("  search <STRING>  Search for ports by name\n");
  printf("  list <REPO>      Ports in repository REPO\n");
  printf("  getup <REPO>     Print httpup/rsync file for REPO\n");
  printf("  help             Show this help information\n");
}

int main(int argc, char** argv)
{

  xmlDoc *doc = NULL;
  xmlNode *r_node = NULL;

  char *url = "http://crux.nu/portdb/";
  char *filename = "/tmp/portdb.xml";

  int command = 0;

  if (argc > 1)
  {
    if (strcmp("repos", argv[1]) == 0)
    {
      asprintf(&url, "%s?f=xml", url);
      command = CMD_REPOS;
    }
    else if (strcmp("dups", argv[1]) == 0)
    {
      asprintf(&url, "%s?f=xml&a=dups", url);
      command = CMD_DUPS;
    }
    else if (strcmp("search", argv[1]) == 0 && argc > 2)
    {
      asprintf(&url, "%s?f=xml&a=search&q=%s", url, argv[2]);
      command = CMD_SEARCH;
    }
    else if (strcmp("list", argv[1]) == 0 && argc > 2)
    {
      asprintf(&url, "%s?f=xml&a=repo&q=%s", url, argv[2]);
      command = CMD_LIST;
    }
    else if (strcmp("getup", argv[1]) == 0 && argc > 2)
    {
      asprintf(&url, "%s?a=getup&q=%s", url, argv[2]);
      // we should print the output since the output is not xml
      printHttpFile(url);
      // and exit
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

  saveHttpFile(url, filename);
  free(url);

  doc = xmlReadFile(filename, NULL, 0);

  if (doc == NULL)
  {
    fprintf(stderr, "Error, could not parse file %s\n", filename);
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
        fprintf(stdout, "Error, temporary file not compatible\n");
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

  return 0;
}

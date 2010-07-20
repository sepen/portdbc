#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
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
        if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "name") == 0)
        {
          asprintf(&name, "%s", (char *) child_node->children->content);
        }
        else if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "ports") == 0)
        {
          asprintf(&ports, "%s", (char *) child_node->children->content);
        }
        else if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "type") == 0)
        {
          asprintf(&type, "%s", (char *) child_node->children->content);
        }
        else if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "maintainer") == 0)
        {
          asprintf(&maintainer, "%s", (char *) child_node->children->content);
        }
        else if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "url") == 0)
        {
          asprintf(&url, "%s", (char *) child_node->children->content);
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
        if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "name") == 0)
        {
          asprintf(&name, "%s", (char *) child_node->children->content);
        }
        else if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "count") == 0)
        {
          asprintf(&count, "%s", (char *) child_node->children->content);
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
        if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "name") == 0)
        {
          asprintf(&name, "%s", (char *) child_node->children->content);
        }
        else if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "repo") == 0)
        {
          asprintf(&repo, "%s", (char *) child_node->children->content);
        }
        else if (child_node->type == XML_ELEMENT_NODE && xmlStrcmp(child_node->name, (const xmlChar *) "command") == 0)
        {
          asprintf(&command, "%s", (char *) child_node->children->content);
        }
      }
      // print port line
      printf("%-28s %-14s %s\n", name, repo, command);
    }
  }
}

void printUsage()
{
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

  char *url = NULL;
  char *filename = "/tmp/portdb.xml";

  int repos, dups, search, list, getup = 0;

  if (argc > 1)
  {
    if (strcmp("repos", argv[1]) == 0)
    {
      asprintf(&url, "http://crux.nu/portdb/?f=xml");
      repos = 1; dups = 0; search = 0; list = 0; getup = 0;
    }
    else if (strcmp("dups", argv[1]) == 0)
    {
      asprintf(&url, "http://crux.nu/portdb/?f=xml&a=dups");
      repos = 0; dups = 1; search = 0; list = 0; getup = 0;
    }
    else if (strcmp("search", argv[1]) == 0 && argc > 2)
    {
      asprintf(&url, "http://crux.nu/portdb/?f=xml&a=search&q=%s", argv[2]);
      repos = 0; dups = 0; search = 1; list = 0; getup = 0;
    }
    else if (strcmp("list", argv[1]) == 0 && argc > 2)
    {
      asprintf(&url, "http://crux.nu/portdb/?f=xml&a=repo&q=%s", argv[2]);
      repos = 0; dups = 0; search = 0; list = 1; getup = 0;
    }
    else if (strcmp("getup", argv[1]) == 0 && argc > 2)
    {
      asprintf(&url, "http://crux.nu/portdb/?a=getup&q=%s", argv[2]);
      repos = 0; dups = 0; search = 0; list = 0; getup = 1;
      printHttpFile(url);
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
    printf("error: could not parse file %s\n", filename);
    return -1;
  }

  // get the root element node
  r_node = xmlDocGetRootElement(doc);

  if (repos == 1)
  {
    // validate as portdb xml compatible
    if (!r_node || !r_node->name || xmlStrcmp(r_node->name, (const xmlChar *) "repos"))
    {
      printf("name: %s\n", r_node->name);
      xmlFreeDoc(doc);
      printf("error: temporary file not compatible\n");
      return -1;
    }
    // print all repositories
    listRepos(r_node);
  }
  else if (dups == 1)
  {
    listDups(r_node);
  }
  else if (search == 1)
  {
    listPorts(r_node);
  }
  else if (list == 1)
  {
    listPorts(r_node);
  }

  // free the document
  xmlFreeDoc(doc);

  // free global variables allocated by the parser
  xmlCleanupParser();

  return 0;
}

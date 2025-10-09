/*
 * portdbc - CRUX's portdb command line client
 *
 * Copyright 2010-2025 Jose Beneyto
 *
 * Licensed under GPLv3, see file COPYING in this source tree.
 */

#include "defines.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define MAX_URLS 8

int main(int argc, char** argv)
{
    xmlDoc *doc = NULL;
    xmlNode *r_node = NULL;

    char *url = NULL;
    char *tmpfile = NULL;
    char *configfile = "/tmp/.portdbc.conf";

    char *portdb_urls[MAX_URLS];
    int portdb_url_count = 0;

    FILE *file_handle = NULL;

    // use HOME directory when available
    char *home_dir = getenv("HOME");
    if (home_dir != NULL)
    {
        asprintf(&tmpfile, "%s/.portdbc.xml", home_dir);
        asprintf(&configfile, "%s/.portdbc.conf", home_dir);

        // try to read config values
        if ((file_handle = fopen(configfile, "r+")))
        {
            char line[256], option[256], value[256];
            int linenum = 0;
            while (fgets(line, 256, file_handle) != NULL)
            {
                linenum++;
                // ignore comments from config file
                if (line[0] == '#') continue;
                // try to read the pair 'option = value'
                if (sscanf(line, "%s = %s", option, value) != 2)
                {
                    continue;
                }
                if (strcmp(option, "portdb_url") == 0)
                {
                    if (portdb_url_count < MAX_URLS) {
                        portdb_urls[portdb_url_count++] = strdup(value);
                    }
                }
            }
            fclose(file_handle);
        }
    }

    // default URL if none configured
    if (portdb_url_count == 0)
    {
        portdb_urls[0] = strdup("https://crux.nu/portdb/");
        portdb_url_count = 1;
        asprintf(&tmpfile, "/tmp/.portdbc.xml");
    }

    int command = NOCMD;

    if (argc > 1)
    {
        if (strcmp("repos", argv[1]) == 0)
        {
            command = CMD_REPOS;
        }
        else if (strcmp("dups", argv[1]) == 0)
        {
            command = CMD_DUPS;
        }
        else if (strcmp("search", argv[1]) == 0 && argc > 2)
        {
            command = CMD_SEARCH;
        }
        else if (strcmp("list", argv[1]) == 0 && argc > 2)
        {
            command = CMD_LIST;
        }
        else if (strcmp("getup", argv[1]) == 0 && argc > 2)
        {
            for (int i = 0; i < portdb_url_count; i++) {
                asprintf(&url, "%s?a=getup&q=%s", portdb_urls[i], argv[2]);
                printHttpFile(url);
                free(url);
            }
            return 0;
        }
        else if (strcmp("version", argv[1]) == 0)
        {
            printVersion();
            return 0;
        }
        else if (strcmp("help", argv[1]) == 0)
        {
            printUsage();
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

    // loop over all configured URLs
    for (int i = 0; i < portdb_url_count; i++)
    {
        switch (command)
        {
            case CMD_REPOS:
                asprintf(&url, "%s?f=xml", portdb_urls[i]);
                break;
            case CMD_DUPS:
                asprintf(&url, "%s?f=xml&a=dups", portdb_urls[i]);
                break;
            case CMD_SEARCH:
                asprintf(&url, "%s?f=xml&a=search&q=%s", portdb_urls[i], argv[2]);
                break;
            case CMD_LIST:
                asprintf(&url, "%s?f=xml&a=repo&q=%s", portdb_urls[i], argv[2]);
                break;
        }

        saveHttpFile(url, tmpfile);
        free(url);

	FILE *f = fopen(tmpfile, "r");
	if (f) {
	    char buf[6] = {0};
	    fread(buf, 1, 5, f);
	    fclose(f);
	    if (strncmp(buf, "<?xml", 5) != 0) {
		fprintf(stderr, "Warning: %s did not return valid XML, skipping\n", url);
		return -1;
	    }
	}

        doc = xmlReadFile(tmpfile, NULL, 0);
        if (doc == NULL)
        {
            fprintf(stderr, "Error, could not parse file %s from %s\n", tmpfile, portdb_urls[i]);
            continue;
        }

        // get the root element node
        r_node = xmlDocGetRootElement(doc);

        switch (command)
        {
            case CMD_REPOS:
                if (!r_node || !r_node->name || xmlStrcmp(r_node->name, (const xmlChar *) "repos"))
                {
                    xmlFreeDoc(doc);
                    fprintf(stderr, "Error, temporary file not compatible (%s)\n", portdb_urls[i]);
                    continue;
                }
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

        xmlFreeDoc(doc);
        remove(tmpfile);
    }

    xmlCleanupParser();

    // free URLs
    for (int i = 0; i < portdb_url_count; i++) {
        free(portdb_urls[i]);
    }
    free(tmpfile);

    return 0;
}

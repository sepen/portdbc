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
#include <unistd.h>   // for mkstemp
#include <errno.h>

int main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *r_node = NULL;

    char *url = NULL;
    char *portdb_url = strdup("https://crux.nu/portdb/");
    char *configfile = NULL;

    FILE *file_handle = NULL;

    // safer temp file
    char tmpfile_template[] = "/tmp/portdbc-XXXXXX";
    int tmpfd = mkstemp(tmpfile_template);
    if (tmpfd == -1) {
        perror("mkstemp");
        return 1;
    }
    close(tmpfd); // we reopen later

    // use HOME directory when available
    char *home_dir = getenv("HOME");
    if (home_dir != NULL) {
        asprintf(&configfile, "%s/.portdbc.conf", home_dir);
        if ((file_handle = fopen(configfile, "r"))) {
            char line[256];
            while (fgets(line, sizeof(line), file_handle) != NULL) {
                // ignore comments
                if (line[0] == '#') continue;
                // find '='
                char *eq = strchr(line, '=');
                if (!eq) continue;
                *eq = '\0';
                char *option = strtok(line, " \t\r\n");
                char *value  = strtok(eq + 1, " \t\r\n");
                if (!option || !value) continue;
                if (strcmp(option, "portdb_url") == 0) {
                    FREENULL(portdb_url);
                    asprintf(&portdb_url, "%s", value);
                }
            }
            fclose(file_handle);
        }
    }

    int command = NOCMD;

    if (argc > 1) {
        if (strcmp("repos", argv[1]) == 0) {
            asprintf(&url, "%s?f=xml", portdb_url);
            command = CMD_REPOS;
        }
        else if (strcmp("dups", argv[1]) == 0) {
            asprintf(&url, "%s?f=xml&a=dups", portdb_url);
            command = CMD_DUPS;
        }
        else if (strcmp("search", argv[1]) == 0 && argc > 2) {
            asprintf(&url, "%s?f=xml&a=search&q=%s", portdb_url, argv[2]);
            command = CMD_SEARCH;
        }
        else if (strcmp("list", argv[1]) == 0 && argc > 2) {
            asprintf(&url, "%s?f=xml&a=repo&q=%s", portdb_url, argv[2]);
            command = CMD_LIST;
        }
        else if (strcmp("getup", argv[1]) == 0 && argc > 2) {
            asprintf(&url, "%s?a=getup&q=%s", portdb_url, argv[2]);
            command = CMD_GETUP;
            printHttpFile(url);
            goto cleanup;
        }
        else if (strcmp("version", argv[1]) == 0) {
            printVersion();
            goto cleanup;
        }
        else if (strcmp("help", argv[1]) == 0) {
            printUsage();
            goto cleanup;
        }
        else {
            printUsage();
            goto cleanup;
        }
    } else {
        printUsage();
        goto cleanup;
    }

    if (saveHttpFile(url, tmpfile_template) != 0) {
        fprintf(stderr, "Error: failed to download %s\n", url);
        goto cleanup;
    }

    doc = xmlReadFile(tmpfile_template, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Error: Could not parse XML from %s\n", tmpfile_template);
        goto cleanup;
    }

    // get the root element node
    r_node = xmlDocGetRootElement(doc);

    switch (command) {
        case CMD_REPOS:
            if (!r_node || !r_node->name ||
                xmlStrcmp(r_node->name, (const xmlChar *)"repos") != 0) {
                fprintf(stderr, "Error: temporary file not compatible\n");
                goto cleanup;
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

cleanup:
    if (doc) xmlFreeDoc(doc);
    xmlCleanupParser();

    if (url) FREENULL(url);
    if (configfile) FREENULL(configfile);
    if (portdb_url) FREENULL(portdb_url);

    unlink(tmpfile_template); // remove temp file
    return 0;
}

#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *stream) {
    return fwrite(ptr, size, nmemb, (FILE *)stream);
}

int printHttpFile(char *url) {
    CURL *curl_handle = curl_easy_init();
    if (!curl_handle) return -1;

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, stdout);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl_handle);
        return -1;
    }

    curl_easy_cleanup(curl_handle);
    return 0;
}

int saveHttpFile(char *url, char *filename) {
    CURL *curl_handle = curl_easy_init();
    if (!curl_handle) return -1;

    FILE *file_handle = fopen(filename, "w");
    if (!file_handle) {
        perror("fopen");
        curl_easy_cleanup(curl_handle);
        return -1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, file_handle);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
        fclose(file_handle);
        curl_easy_cleanup(curl_handle);
        return -1;
    }

    fclose(file_handle);
    curl_easy_cleanup(curl_handle);
    return 0;
}

void listRepos(xmlNode *r_node) {
    xmlNode *cur_node = NULL;
    xmlNode *child_node = NULL;

    char *name = NULL;
    char *ports = NULL;
    char *type = NULL;
    char *maintainer = NULL;
    char *url = NULL;

    for (cur_node = r_node->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE &&
            xmlStrcmp(cur_node->name, (const xmlChar *)"repo") == 0) {

            for (child_node = cur_node->children; child_node != NULL; child_node = child_node->next) {
                if (child_node->type == XML_ELEMENT_NODE) {
                    if (xmlStrcmp(child_node->name, (const xmlChar *)"name") == 0 &&
                        child_node->children && child_node->children->content) {
                        name = strdup((char *)child_node->children->content);
                    } else if (xmlStrcmp(child_node->name, (const xmlChar *)"ports") == 0 &&
                               child_node->children && child_node->children->content) {
                        ports = strdup((char *)child_node->children->content);
                    } else if (xmlStrcmp(child_node->name, (const xmlChar *)"type") == 0 &&
                               child_node->children && child_node->children->content) {
                        type = strdup((char *)child_node->children->content);
                    } else if (xmlStrcmp(child_node->name, (const xmlChar *)"maintainer") == 0 &&
                               child_node->children && child_node->children->content) {
                        maintainer = strdup((char *)child_node->children->content);
                    } else if (xmlStrcmp(child_node->name, (const xmlChar *)"url") == 0 &&
                               child_node->children && child_node->children->content) {
                        url = strdup((char *)child_node->children->content);
                    }
                }
            }
            printf("%-20s %-4s %-7s %-46s %s\n",
                   NONULL(name), NONULL(ports), NONULL(type),
                   NONULL(maintainer), NONULL(url));

            FREENULL(name);
            FREENULL(ports);
            FREENULL(type);
            FREENULL(maintainer);
            FREENULL(url);
        }
    }
}

void listDups(xmlNode *r_node) {
    xmlNode *cur_node = NULL;
    xmlNode *child_node = NULL;

    char *name = NULL;
    char *count = NULL;

    for (cur_node = r_node->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE &&
            xmlStrcmp(cur_node->name, (const xmlChar *)"duplicate") == 0) {

            for (child_node = cur_node->children; child_node != NULL; child_node = child_node->next) {
                if (child_node->type == XML_ELEMENT_NODE) {
                    if (xmlStrcmp(child_node->name, (const xmlChar *)"name") == 0 &&
                        child_node->children && child_node->children->content) {
                        name = strdup((char *)child_node->children->content);
                    } else if (xmlStrcmp(child_node->name, (const xmlChar *)"count") == 0 &&
                               child_node->children && child_node->children->content) {
                        count = strdup((char *)child_node->children->content);
                    }
                }
            }
            printf("%-28s %s\n", NONULL(name), NONULL(count));

            FREENULL(name);
            FREENULL(count);
        }
    }
}

void listPorts(xmlNode *r_node) {
    xmlNode *cur_node = NULL;
    xmlNode *child_node = NULL;

    char *name = NULL;
    char *repo = NULL;
    char *command = NULL;

    for (cur_node = r_node->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE &&
            xmlStrcmp(cur_node->name, (const xmlChar *)"port") == 0) {

            for (child_node = cur_node->children; child_node != NULL; child_node = child_node->next) {
                if (child_node->type == XML_ELEMENT_NODE) {
                    if (xmlStrcmp(child_node->name, (const xmlChar *)"name") == 0 &&
                        child_node->children && child_node->children->content) {
                        name = strdup((char *)child_node->children->content);
                    } else if (xmlStrcmp(child_node->name, (const xmlChar *)"repo") == 0 &&
                               child_node->children && child_node->children->content) {
                        repo = strdup((char *)child_node->children->content);
                    } else if (xmlStrcmp(child_node->name, (const xmlChar *)"command") == 0 &&
                               child_node->children && child_node->children->content) {
                        command = strdup((char *)child_node->children->content);
                    }
                }
            }
            printf("%-28s %-14s %s\n", NONULL(name), NONULL(repo), NONULL(command));

            FREENULL(name);
            FREENULL(repo);
            FREENULL(command);
        }
    }
}

void printUsage() {
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

void printVersion() {
    printf("%s\n", VERSION_STRING);
}

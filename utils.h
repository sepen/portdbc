#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <libxml/parser.h>

size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *stream);
int printHttpFile(char *url);
int saveHttpFile(char *url, char *filename);
void listRepos(xmlNode *r_node);
void listDups(xmlNode *r_node);
void listPorts(xmlNode *r_node);
void printUsage();
void printVersion();

#endif
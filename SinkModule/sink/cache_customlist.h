#pragma once
#ifndef CACHE_CUSTOMLIST_H
#define CACHE_CUSTOMLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>      

#include "cache_domains.h"

#ifndef WIN32
#include <unistd.h>


#else
#define _Atomic volatile
#include <Windows.h>
void usleep(__int64 usec)
{
	HANDLE timer;
	LARGE_INTEGER ft;

	ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}

#endif

typedef struct
{
	int capacity;
	int index;
	_Atomic int searchers;
	char **identity;
  cache_domain **whitelist;
  cache_domain **blacklist;
} cache_customlist;

typedef struct
{
  char **whitelist;
  char **blacklist;
} customlist;

cache_customlist* cache_customlist_init(int count)
{
	cache_customlist *item = (cache_customlist *)calloc(1, sizeof(cache_customlist));
  if (item == NULL)
  {
    return NULL;
  }
  
	item->capacity = count;
	item->index = 0;
	item->searchers = 0;
	item->identity = (char **)malloc(item->capacity * sizeof(char *));
  item->whitelist = (cache_domain **)malloc(item->capacity * sizeof(cache_domain *));
  item->blacklist = (cache_domain **)malloc(item->capacity * sizeof(cache_domain *));
  if (item->identity == NULL || item->whitelist == NULL || item->blacklist == NULL)
  {
    return NULL;
  }
  

	return item;
}
         
void cache_customlist_destroy(cache_customlist *cache)
{
  while (cache->searchers > 0)
  {
    usleep(50000);
  }
  
  int position = cache->index;
	while (--position >= 0)
	{
    if (cache->identity[position] != NULL)
  	{
  		free(cache->identity[position]);
  	}
    
    if (cache->whitelist[position] != NULL)
  	{
  		cache_domain_destroy(cache->whitelist[position]);
  	}    

    if (cache->blacklist[position] != NULL)
  	{
  		cache_domain_destroy(cache->blacklist[position]);
  	}    
  }
    
  if (cache->identity != NULL)
  {
    free(cache->identity);
  }
  if (cache->whitelist != NULL)
  {
    free(cache->whitelist);
  }
  if (cache->blacklist != NULL)
  {
    free(cache->blacklist);
  }
  if (cache != NULL)
  {
    free(cache);
  } 
}

int cache_customlist_add(cache_customlist* cache, char *identity, cache_domain *whitelist, cache_domain *blacklist)
{
	if (cache->index > cache->capacity)
		return -1;

  char* xidentity = (char *)malloc(strlen(identity));
  if (xidentity == NULL)
  {
    return -1;
  }
  cache_domain *xwhitelist = cache_domain_init(whitelist->capacity);
  cache_domain *xblacklist = cache_domain_init(blacklist->capacity);
  if (xwhitelist == NULL || xblacklist == NULL)
  {
    return -1;
  }

  memcpy(xidentity, identity, strlen(identity));
  memcpy(xwhitelist->base, whitelist->base, whitelist->index * sizeof(unsigned long long));
  memcpy(xblacklist->base, blacklist->base, blacklist->index * sizeof(unsigned long long));
  
  xwhitelist->index = whitelist->index;
  xblacklist->index = blacklist->index;
  
  cache->identity[cache->index] = xidentity;
  cache->whitelist[cache->index] = xwhitelist;
  cache->blacklist[cache->index] = xblacklist;

	cache->index++;

	return 0;
}

int cache_customlist_whitelist_contains(cache_customlist* cache, char *identity, unsigned long long crc)
{
	cache->searchers++;
  int result = 0;
  int position = cache->index;

	while (--position >= 0)
	{
    if (strcmp(cache->identity[position], identity) == 0)
    {
      domain item;
      if ((result = cache_domains_contains(cache->whitelist[position], crc, &item)) == 1)
      {
        break;
      }
    }
  }
  
	cache->searchers--;
	return result;
}

int cache_customlist_blacklist_contains(cache_customlist* cache, char *identity, unsigned long long crc)
{
	cache->searchers++;
  int result = 0;
  int position = cache->index;

	while (--position >= 0)
	{
    if (strcmp(cache->identity[position], identity) == 0)
    {
      domain item;
      if ((result = cache_domains_contains(cache->blacklist[position], crc, &item)) == 1)
      {
        break;
      }
    }
  }
  
	cache->searchers--;
	return result;
}
                 
#endif
#pragma once
#ifndef CACHE_LOADER_H
#define CACHE_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crc64.h"
#include "cache_iprange.h"
#include "cache_policy.h"
#include "cache_customlist.h"
#include "cache_domains.h"

cache_domain* cached_domain = NULL;
cache_iprange* cached_iprange = NULL;
cache_policy* cached_policy = NULL;
cache_customlist* cached_customlist = NULL;

/*
bool hashcontainer_contains(char * value)
{
	if (!hashtable)
		return false;

	unsigned long long crc = crc64(0, (const unsigned char*)value, strlen(value));
	cache1item cache;
	return sink_list_contains(hashtable, crc, &cache);
}
  */
    /*
void hashcontainer_reinit(int count, char *domains, char *accuracy, char *flags)
{
	list *old = hashtable;
	hashtable = sink_list_init_ex(domains, accuracy, flags, count);
	sink_list_destroy(old);
}
*/
         
char **split(char *line, char sep, int fields) {
  char **r = (char **)malloc(fields * sizeof(char*));

  int lptr = 0, fptr = 0;
  r[fptr++] = line;

  while (line[lptr]) {
    if (line[lptr] == sep) {
      line[lptr] = '\0';
      r[fptr] = &(line[lptr+1]);
      fptr++;
    }

    lptr++;
  }

  return r;
}

int parse_addr(struct sockaddr *sa, const char *addr) {
    int family = strchr(addr, ':') ? AF_INET6 : AF_INET;
    if (family == AF_INET6)
    {
    	struct sockaddr* ip6sto;
    	struct sockaddr_in6 ip6to;
    	ip6to.sin6_family = AF_INET6;
    	ip6to.sin6_port = htons(0);
    	inet_pton(AF_INET6, addr, &ip6to.sin6_addr);
    	sa = (struct sockaddr *)&ip6to;
    }
    else
    {
     	struct sockaddr* ip4sfrom; 
    	struct sockaddr_in ip4from;
    	ip4from.sin_family = AF_INET;
    	ip4from.sin_port = htons(0);
    	inet_pton(AF_INET, addr, &ip4from.sin_addr);
    	sa = (struct sockaddr *)&ip4from;
    }
        
    return 0;
}

int countchar(char separator, char *string)
{
  int counter = 0;
  for (int i = 0; i < strlen(string); i++)
  {
    if (string[i] == separator)
      counter ++;
  }
  return counter;  
}

int loader_loaddomains()
{
	FILE* stream = fopen("domains.csv", "r");
	char line[1024];
  
  if (stream == NULL)
  {
    return -1;
  }

	int linecount = 0;
	while (fgets(line, 1024, stream))
	{
		linecount++;
	}
  
	if ((cached_domain = cache_domain_init(linecount)) == NULL)
  {
    puts("not enough memory to create domain cache");
    return -1;
  }
        
	fseek(stream, 0, SEEK_SET);
	while (fgets(line, 1024, stream))
	{
		char **fields = split(line, ',', 3);
    //unsigned long long crc = crc64(0, fields[0], strlen((const char *)fields[0]));
    //short acc = atoi(fields[1]);
    //unsigned long long flags = strtoull(fields[2], (char **)NULL, 10);
		
		//cache_domain_add(cached_domain, crc, 0, 0);
    free(fields);
	}
  /*
	cache_domain_sort(cached_domain);
  
	fseek(stream, 0, SEEK_SET);
	while (fgets(line, 1024, stream))
	{
		char **fields = split(line, ',', 3);
    unsigned long long crc = crc64(0, fields[0], strlen((const char *)fields[0]));
    short acc = atoi(fields[1]);
    unsigned long long flags = strtoull(fields[2], (char **)NULL, 10);
		
		cache_domain_update(cached_domain, crc, acc, flags);
    free(fields);
	}   */
  
  fclose(stream);
  
  return 0;
}

int loader_loadranges()
{
	FILE* stream = fopen("ranges.csv", "r");
	char line[1024];

  if (stream == NULL)
  {
    return -1;
  }

	int linecount = 0;
	while (fgets(line, 1024, stream))
	{
		linecount++;
	}
  
	if ((cached_iprange = cache_iprange_init(linecount)) == NULL)
  {
    puts("not enough memory to create ip range cache");
    return -1;
  }
  
	fseek(stream, 0, SEEK_SET);
	while (fgets(line, 1024, stream))
	{
		char **fields = split(line, ',', 4);
    
    struct sockaddr from;
    struct sockaddr to;
    char *ipfrom = fields[0];
    char *ipto = fields[1];
    parse_addr(&from, ipfrom);
    parse_addr(&to, ipto);
  
    char *identity = fields[2];
    int policy_id = atoi(fields[3]);    
    		
		if (cache_iprange_add(cached_iprange, &from, &to, identity, policy_id) != 0)
    {
      puts("not enough memory to add to ip range cache");
      return -1;      
    }
    
    free(fields);
	}
  
  fclose(stream);
  
  return 0;
}

int loader_loadpolicy()
{
	FILE* stream = fopen("policy.csv", "r");
	char line[1024];
  
  if (stream == NULL)
  {
    return -1;
  }  

	int linecount = 0;
	while (fgets(line, 1024, stream))
	{
		linecount++;
	}
  
	if ((cached_policy = cache_policy_init(linecount)) == NULL)
  {
    puts("not enough memory to create policy cache");
    return -1;
  }
  
	fseek(stream, 0, SEEK_SET);
	while (fgets(line, 1024, stream))
	{
		char **fields = split(line, ',', 4);
    
    int policy_id = atoi(fields[0]);    
    int strategy = atoi(fields[1]);    
    int audit = atoi(fields[2]);    
    int block = atoi(fields[3]);    
    		
		cache_policy_add(cached_policy, policy_id, strategy, audit, block);
    
    free(fields);
	}
  
  fclose(stream);
  
  return 0;
}

int loader_loadcustom()
{
	FILE* stream = fopen("custom.csv", "r");
	char line[1024];
  
  if (stream == NULL)
  {
    return -1;
  }

	int linecount = 0;
	while (fgets(line, 1024, stream))
	{
		linecount++;
	}
  
	if ((cached_customlist = cache_customlist_init(linecount)) == NULL)
  {
    puts("not enough memory to create custom list cache");
    return -1;
  }
  
	fseek(stream, 0, SEEK_SET);
	while (fgets(line, 1024, stream))
	{
		char **fields = split(line, ',', 3);
    
    char *ident = fields[0];    
    int whitenum = countchar(';', fields[1]);
    if (whitenum != strlen(fields[2]) && strlen(fields[1]) > 1)
    {
      whitenum += 1;
    }
    char **whitelist = split(fields[1], ';', whitenum);
    int blacknum = countchar(';', fields[2]);
    if (blacknum != strlen(fields[2]) && strlen(fields[2]) > 1)
    {
      blacknum += 1;
    }
    char **blacklist = split(fields[2], ';', blacknum);
    
    cache_domain *cwhitelist = cache_domain_init(whitenum);
    cache_domain *cblacklist = cache_domain_init(blacknum);
    if (cwhitelist == NULL)
    {
      puts("not enough memory to create custom list whitelist cache");
      return -1;
    }    
    if (cblacklist == NULL)
    {
      puts("not enough memory to create custom list blacklist cache");
      return -1;
    }    
     
    for (int i = 0; i < whitenum; i++)
    { 
      unsigned long long crc = crc64(0, whitelist[i], strlen((const char *)whitelist[i]));
		  cache_domain_add(cwhitelist, crc, 0, 0);
    }
    cache_domain_sort(cwhitelist);
    for (int i = 0; i < blacknum; i++)
    {
      unsigned long long crc = crc64(0, blacklist[i], strlen((const char *)blacklist[i]));
		  cache_domain_add(cblacklist, crc, 0, 0);
    }
    cache_domain_sort(cblacklist);
    if (cache_customlist_add(cached_customlist, ident, cwhitelist, cblacklist) != 0)
    {
      puts("not enough memory to add lists to custom list");
      return -1;
    }
    
    cache_domain_destroy(cwhitelist);
    cache_domain_destroy(cblacklist);
    
    free(fields);
    free(whitelist);
    free(blacklist);
	}
  
  fclose(stream);
  
  return 0;
}
    
       
int loader_init()
{
  int err_success = 0;
	if (cached_domain)
	{
      cache_domain *old_domains = cached_domain;
		  if ((err_success = loader_loaddomains()) != 0)
      {
        puts("error re-reading domains");
        return err_success;
      }
      cache_domain_destroy(old_domains);
	} 
  else
  {
    if ((err_success = loader_loaddomains()) != 0)
    {
      puts("error reading domians");
      return err_success;
    } 
  }
  
	if (cached_iprange)
	{
      cache_iprange *old_iprange = cached_iprange;
		  if ((err_success = loader_loadranges()) != 0)
      {
        puts("error re-reading ranges");
        return err_success;
      }
      cache_iprange_destroy(old_iprange);
	}
  else
  {
    if ((err_success = loader_loadranges()) != 0)
    {
      puts("error reading ip ranges");
      return err_success;
    }   
  }
  
	if (cached_policy)
	{
      cache_policy *old_policy = cached_policy;
		  if ((err_success = loader_loadpolicy()) != 0)
      {
        puts("error re-reading policies");
        return err_success;
      }
      cache_policy_destroy(old_policy);
	}
  else
  {
    if ((err_success = loader_loadpolicy()) != 0)
    {
      puts("error reading policy");
      return err_success;
    }   
  }
  
	if (cached_customlist)
	{
      cache_customlist *old_customlist = cached_customlist;
		  if ((err_success = loader_loadcustom()) != 0)
      {
        puts("error re-reading custom list");
        return err_success;
      }
      cache_customlist_destroy(old_customlist);
	}
  else
  {
    if ((err_success = loader_loadcustom()) != 0)
    {
      puts("error reading custom list");
      return err_success;
    }
  }

  return err_success;  
}

#endif
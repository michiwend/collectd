/**
 * collectd - src/luci_splash.c
 * Copyright (C) 2005-2008  Florian octo Forster
 * Copyright (C) 2009       Manuel Sanmartin
 * Copyright (C) 2013		Freifunk Augsburg / Michael Wendland
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; only version 2 of the License is applicable.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * Authors:
 *   Florian octo Forster <octo at verplant.org>
 *   Manuel Sanmartin
 * 	 Michael Wendland <michael@michiwend.com>
 **/

#include "collectd.h"
#include "common.h"
#include "plugin.h"


#define LEASED 0
#define WHITELISTED 1
#define BLACKLISTED 2

int parse_values (char lines[][256], value_t *values)
{
	
	char *value_ptr;
	
	int i;
		
	for(i=0; i<6; i++)
	{
		j = 0;
		
		value_ptr = (strrchr(lines[i], '=')+1);
				
		if(strstr(lines[i], "luci_splash_leases.stats.leases")) {
			values[LEASED].counter = (counter_t)atoi(value_ptr);
		}
		else if(strstr(lines[i], "luci_splash_leases.stats.whitelistonline")) {
			values[WHITELISTED].counter = (counter_t)atoi(value_ptr);
		}
		else if(strstr(lines[i], "luci_splash_leases.stats.blacklistonline")) {
			values[BLACKLISTED].counter = (counter_t)atoi(value_ptr);
		}			
	}
		
	return 1;
}

static void luci_splash_submit (value_t *values)
{
	value_list_t vl = VALUE_LIST_INIT;

	vl.values = values;
	vl.values_len = STATIC_ARRAY_SIZE (values);
	sstrncpy (vl.host, hostname_g, sizeof (vl.host));
	sstrncpy (vl.plugin, "luci_splash", sizeof (vl.plugin));
	sstrncpy (vl.type, "luci_splash", sizeof (vl.type));

  	plugin_dispatch_values (&vl); 
}


// Simple read callback function
static int luci_splash_read (void)
{
	gauge_t num_leases, num_whitelisted, num_blacklisted;
	
	FILE *leases_file;
	char lines[6][256];
	value_t values[3];
	
	if ((leases_file = fopen ("/var/state/luci_splash_leases", "r")) == NULL)
	{
		char errbuf[1024];
		WARNING ("luci_splash: fopen: %s",
				sstrerror (errno, errbuf, sizeof (errbuf)));
		return (-1);
	}

	if (fgets (buffer, 16, loadavg) == NULL)
	{
		char errbuf[1024];
		WARNING ("luci_splash: fgets: %s",
				sstrerror (errno, errbuf, sizeof (errbuf)));
		fclose (leases_file);
		return (-1);
	}

	if (fclose (leases_file))
	{
		char errbuf[1024];
		WARNING ("luci_splash: fclose: %s",
				sstrerror (errno, errbuf, sizeof (errbuf)));
	}
	
	
	parse_values(lines, values);

	luci_splash_submit(values);
	
	return (0);
}

void module_register (void)
{
	plugin_register_read ("luci_splash", luci_splash_read);
} /* void module_register */

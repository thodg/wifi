/*
 * Copyright (c) 2012 Thomas de Grivel <billitch@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <net80211/ieee80211.h>
#include <net80211/ieee80211_ioctl.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <wifi.h>

int usage (const char *argv0)
{
	fprintf(stderr, "Usage: %s INTERFACE COMMAND\n", argv0);
	return 1;
}

void print_nodes (struct wifi_interface *wi)
{
	int i = wi->node_count;
	struct ieee80211_nodereq *n = &wi->node[0];
	while (i-- > 0) {
		printf("  %s %d\n", n->nr_nwid, (int) n->nr_rssi);
		n++;
	}
}

int main (int argc, char *argv[])
{
	struct wifi_interface wi;
	if (argc != 3)
		return usage(argv[0]);
	if (wifi_interface(&wi, argv[1]))
		return 2;
	if (!strcmp("scan", argv[2])) {
	  int err = 0;
	  err |= wifi_scan(&wi);
	  print_nodes(&wi);
	  err |= wifi_interface_close(&wi);
	  return err;
	}
	return 0;
}

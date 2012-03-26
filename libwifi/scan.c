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

#include "wifi.h"

int wifi_get_flags (struct wifi_interface *wi)
{
	struct ifreq r;
	if (!wi)
		return EINVAL;
	strlcpy(r.ifr_name, wi->name, IFNAMSIZ);
	if (ioctl(wi->socket, SIOCGIFFLAGS, (caddr_t)&r) < 0) {
		warn("%s: get flags", r.ifr_name);
		return -1;
	}
	wi->flags = r.ifr_flags & 0xffff;
	return 0;
}

int wifi_set_flags (struct wifi_interface *wi)
{
	struct ifreq r;
	if (!wi)
		return EINVAL;
	strlcpy(r.ifr_name, wi->name, IFNAMSIZ);
	r.ifr_flags = wi->flags;
	if (ioctl(wi->socket, SIOCSIFFLAGS, (caddr_t)&r) < 0) {
		warn("%s: set flags", r.ifr_name);
		return -1;
	}
	return 0;
}

int wifi_interface (struct wifi_interface *wi, const char *name)
{
	if (!wi)
		return EINVAL;
	bzero(wi, sizeof(struct wifi_interface));
	if (strlcpy(wi->name, name, IFNAMSIZ) >= IFNAMSIZ) {
		warnx("interface name too long: %s", name);
		return EINVAL;
	}
	wi->socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (wi->socket < 0) {
		warn("%s: socket", wi->name);
		return EBADF;
	}
	return 0;
}

int wifi_interface_close (struct wifi_interface *wi)
{
	if (!wi)
		return EINVAL;
	if (wi->socket >= 0) {
		if (close(wi->socket) != 0) {
			warn("%s: close(%d)", wi->name, wi->socket);
			return -1;
		}
		wi->socket = -1;
	}
	return 0;
}

int wifi_up (struct wifi_interface *wi)
{
	int up;
	if (!wi)
		return EINVAL;
	wifi_get_flags(wi);
	up = wi->flags & IFF_UP;
	if (up)
		return 0;
	wi->flags &= IFF_UP;
	return wifi_set_flags(wi);
}

int wifi_ioctl_scan (struct wifi_interface *wi)
{
	struct ifreq r;
	if (!wi)
		return EINVAL;
	bzero(&r, sizeof(r));
	strlcpy(r.ifr_name, wi->name, IFNAMSIZ);
	if (ioctl(wi->socket, SIOCS80211SCAN, (caddr_t)&r) != 0) {
		warn("%s: scan", wi->name);
		return -1;
	}
	return 0;
}

int wifi_ioctl_allnodes (struct wifi_interface *wi)
{
	struct ieee80211_nodereq_all nra;
	if (!wi)
		return EINVAL;
	bzero(&nra, sizeof(nra));
	bzero(&wi->node[0], sizeof(wi->node));
	nra.na_node = &wi->node[0];
	nra.na_size = sizeof(wi->node);
	strlcpy(nra.na_ifname, wi->name, IFNAMSIZ);
	if (ioctl(wi->socket, SIOCG80211ALLNODES, &nra) != 0) {
		warn("%s: allnodes", wi->name);
		return -1;
	}
	wi->node_count = nra.na_nodes;
	return 0;
}

int wifi_scan (struct wifi_interface *wi)
{
	if (!wi)
		return EINVAL;
	return wifi_ioctl_scan(wi) || wifi_ioctl_allnodes(wi);
}

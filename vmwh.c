/*
 * Copyright (c) 2010 joshua stein <jcs@jcs.org>
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

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>

#include "vmwh.h"

int debug = 0;

int
main(int argc, char *argv[])
{
	int done_init = 0;
	int was_grabbed = 0;

	debug = 1;

	vmware_check_version();
	vmware_get_mouse_position();
	x11_init();

	for (;;) {
		was_grabbed = mouse_grabbed;
		vmware_get_mouse_position();

		if (mouse_grabbed && (!was_grabbed || !done_init)) {
			/* transitioned from host -> guest */
			char *clip;

			if (debug)
				printf("transitioned from host -> guest\n");

			if (vmware_get_clipboard(&clip)) {
				x11_set_clipboard(clip);
				free(clip);
			}

			x11_set_cursor(host_mouse_x, host_mouse_y);
		}

		else if (!mouse_grabbed && (was_grabbed || !done_init)) {
			/* transitioned from guest -> host */
			char *clip;

			if (debug)
				printf("transitioned from guest -> host\n");

			if (x11_get_clipboard(&clip)) {
				vmware_set_clipboard(clip);
				free(clip);
			}

			// vmware_set_cursor(host_mouse_x, host_mouse_y);
		}

		if (!done_init)
			done_init = 1;

		usleep(500);
	}

	return (0);
}
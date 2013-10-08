/*
 *  clevo-wmi.c
 *
 *  Copyright (C) 2013  Ash Hughes <ashley.hughes@blueyonder.co.uk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ---------------------------------------------------------------------
 *
 *  Based on code by:
 *
 *  * Michael WJ Wang @ Clevo Co, <weijoe.wang AT gmail.com>
 *  * Peter Wu, <lekensteyn@gmail.com>
 *  * Steven David Seeger, <steven.seeger@flightsystems.net>
 *  * Christoph Jaeger, <christophjaeger@linux.com>
 */

#define CLEVO_WMI_VER "0.1"
#include <linux/module.h>

MODULE_AUTHOR("Ash Hughes <ashley.hughes@blueyonder.co.uk>");
MODULE_DESCRIPTION("Clevo WMI Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(CLEVO_WMI_VER);

static int __init clevo_wmi_init(void)
{
	return 0;
}

static void __exit clevo_wmi_exit(void)
{

}

module_init(clevo_wmi_init);
module_exit(clevo_wmi_exit);

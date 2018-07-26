/*
 * Copyright (C) 2017-2018 Konsulko Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _GNU_SOURCE

#include <string.h>

#define AFB_BINDING_VERSION 2
#include <afb/afb-binding.h>

#include "ofono_manager.h"
#include "ofono_manager_interface.h"

struct ofono_manager_modem
{
	const gchar *address;
	const gchar *path;
	const gchar *name;
	const gchar *type;
	gboolean powered;
	gboolean online;
	gboolean valid;
};

static OrgOfonoManager *manager;
static struct ofono_manager_modem default_modem;

void ofono_manager_invalidate_default_modem()
{
	default_modem.valid = FALSE;
}

int ofono_manager_set_default_modem(const char *address)
{
	GVariant *out_arg = NULL, *next, *value;
	GError *error = NULL;
	gchar *path, *key;
	const gchar *name = NULL, *type = NULL, *serial = NULL;
	gboolean powered = FALSE, online = FALSE;
	GVariantIter *iter, *iter2 = NULL;
	int ret = 0;

	/* Fetch all visible modems */
	org_ofono_manager_call_get_modems_sync(manager, &out_arg, NULL, &error);
	if (error == NULL) {
		g_variant_get(out_arg, "a(oa{sv})", &iter);
		/* Iterate over each modem */
		while ((next = g_variant_iter_next_value(iter))) {
			g_variant_get(next, "(oa{sv})", &path, &iter2);
			while (g_variant_iter_loop(iter2, "{sv}", &key, &value)) {
				if (!strcmp(key, "Name"))
					name = g_variant_get_string(value, NULL);
				else if (!strcmp(key, "Online"))
					online = g_variant_get_boolean(value);
				else if (!strcmp(key, "Powered"))
					powered = g_variant_get_boolean(value);
				else if (!strcmp(key, "Serial"))
					serial = g_variant_get_string(value, NULL);
				else if (!strcmp(key, "Type"))
					type = g_variant_get_string(value, NULL);
			}
			/* If the HFP modem matches the BT address, is powered, and online then set as default */
			if (!strcmp(type, "hfp") && !strcmp(address, serial) && powered && online) {
				default_modem.address = serial;
				default_modem.path = path;
				default_modem.name = name;
				default_modem.type = type;
				default_modem.powered = powered;
				default_modem.online = online;
				default_modem.valid = TRUE;
				AFB_NOTICE("New modem: %s (%s)", name, address);
				break;
			}
		}
	} else {
		ret = -1;
	}

	return ret;
}

int ofono_manager_init()
{
	int ret = 0;

	if (manager) {
		AFB_ERROR("Ofono Manager already initialized\n");
		return -1;
	}

	manager = org_ofono_manager_proxy_new_for_bus_sync(
		G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE,
		"org.ofono", "/", NULL, NULL);

	if (!manager) {
		AFB_ERROR("Ofono Manager not initialized\n");
		return -1;
	}


	return ret;
}

const gchar *ofono_manager_get_default_modem_address(void)
{
	if (!manager) {
		AFB_ERROR("Ofono Manager not initialized\n");
	}

	return default_modem.address;
}

const gchar *ofono_manager_get_default_modem_path(void)
{
	if (!manager) {
		AFB_ERROR("Ofono Manager not initialized\n");
	}

	return default_modem.path;
}

const gchar *ofono_manager_get_default_modem_name(void)
{
	if (!manager) {
		AFB_ERROR("Ofono Manager not initialized\n");
	}

	return default_modem.name;
}

const gchar *ofono_manager_get_default_modem_type(void)
{
	if (!manager) {
		AFB_ERROR("Ofono Manager not initialized\n");
	}

	return default_modem.type;
}

gboolean ofono_manager_get_default_modem_powered(void)
{
	if (!manager) {
		AFB_ERROR("Ofono Manager not initialized\n");
	}

	return default_modem.powered;
}

gboolean ofono_manager_get_default_modem_online(void)
{
	if (!manager) {
		AFB_ERROR("Ofono Manager not initialized\n");
	}

	return default_modem.online;
}

gboolean ofono_manager_get_default_modem_valid(void)
{
	if (!manager) {
		AFB_ERROR("Ofono Manager not initialized\n");
	}

	return default_modem.valid;
}

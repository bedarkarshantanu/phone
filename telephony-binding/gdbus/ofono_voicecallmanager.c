/*
 * Copyright (C) 2017 Konsulko Group
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

#include "ofono_voicecallmanager_interface.h"

static void call_added(OrgOfonoVoiceCallManager *manager,
		       gchar *op,
		       GVariant *properties)
{
	GVariantIter *iter;
	gchar *key;
	GVariant *value;
	const gchar *state = NULL, *cl = NULL;

	g_variant_get(properties, "a{sv}", &iter);
	while (g_variant_iter_loop(iter, "{sv}", &key, &value)) {
		if (!strcmp(key, "State")) {
			state = g_variant_get_string(value, NULL);
		} else if (!strcmp(key, "LineIdentification")) {
			cl = g_variant_get_string(value, NULL);
		}
	}

	if (!strcmp(state, "incoming")) {
		g_signal_emit_by_name(manager, "incoming-call", op, cl ? cl : "");
	} else if (!strcmp(state, "dialing")) {
		g_signal_emit_by_name(manager, "dialing-call", op, cl ? cl : "");
	}
}

static void call_removed(OrgOfonoVoiceCallManager *manager,
			 gchar *op,
			 GVariant *properties)
{
	g_signal_emit_by_name(manager, "terminated-call", op);
}

const OrgOfonoVoiceCallManager
*ofono_voicecallmanager_init(const gchar *op,
			     void (*incoming_call)(OrgOfonoVoiceCallManager *manager,gchar *,gchar *),
			     void (*dialing_call)(OrgOfonoVoiceCallManager *manager,gchar *,gchar *),
			     void (*terminated_call)(OrgOfonoVoiceCallManager *manager,gchar *))
{
	OrgOfonoVoiceCallManager *manager = org_ofono_voice_call_manager_proxy_new_for_bus_sync(
			G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE,
			"org.ofono", op, NULL, NULL);

	g_signal_new("incoming-call",
		     G_TYPE_OBJECT,
		     G_SIGNAL_RUN_LAST,
		     0,
		     NULL,
		     NULL,
		     g_cclosure_marshal_generic,
		     G_TYPE_NONE,
		     2,
		     G_TYPE_STRING,
		     G_TYPE_STRING);

	g_signal_new("dialing-call",
		     G_TYPE_OBJECT,
		     G_SIGNAL_RUN_LAST,
		     0,
		     NULL,
		     NULL,
		     g_cclosure_marshal_generic,
		     G_TYPE_NONE,
		     2,
		     G_TYPE_STRING,
		     G_TYPE_STRING);

	g_signal_new("terminated-call",
		     G_TYPE_OBJECT,
		     G_SIGNAL_RUN_LAST,
		     0,
		     NULL,
		     NULL,
		     g_cclosure_marshal_generic,
		     G_TYPE_NONE,
		     1,
		     G_TYPE_STRING);

	if (g_signal_connect(G_OBJECT(manager), "incoming-call", G_CALLBACK(incoming_call), NULL) <= 0) {
		AFB_ERROR("Failed to connect to signal incoming-call\n");
	}

	if (g_signal_connect(G_OBJECT(manager), "dialing-call", G_CALLBACK(dialing_call), NULL) <= 0) {
		AFB_ERROR("Failed to connect to signal dialing-call\n");
	}

	if (g_signal_connect(G_OBJECT(manager), "terminated-call", G_CALLBACK(terminated_call), NULL) <= 0) {
		AFB_ERROR("Failed to connect to signal terminated-call\n");
	}

	if (g_signal_connect(manager, "call-added", G_CALLBACK(call_added), NULL) <= 0) {
		AFB_ERROR("Failed to connect to signal call-added\n");
	}

	if (g_signal_connect(manager, "call-removed", G_CALLBACK(call_removed), NULL) <= 0) {
		AFB_ERROR("Failed to connect to signal call-removed\n");
	}

	return manager;
}

void ofono_voicecallmanager_free(OrgOfonoVoiceCallManager *vcm)
{
	g_object_unref(G_OBJECT(vcm));
}

gchar *ofono_voicecallmanager_dial(OrgOfonoVoiceCallManager *manager,
				   const gchar *number,
				   const gchar *cid)
{
	gchar *out = NULL;
	GError *error = NULL;

	if (!manager) {
		AFB_ERROR("Ofono VoiceCallmanager uninitialized\n");
		return NULL;
	}

	org_ofono_voice_call_manager_call_dial_sync(manager, number, cid, &out, NULL, &error);
	if (error != NULL)
		out = NULL;

	return out;
}

void ofono_voicecallmanager_hangup_all(OrgOfonoVoiceCallManager *manager)
{
	GError *error = NULL;

	if (!manager) {
		AFB_ERROR("Ofono VoiceCallmanager uninitialized\n");
		return;
	}

	org_ofono_voice_call_manager_call_hangup_all_sync(manager, NULL, &error);
}

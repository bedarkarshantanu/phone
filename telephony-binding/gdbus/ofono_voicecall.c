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
#include <unistd.h>

#define AFB_BINDING_VERSION 2
#include <afb/afb-binding.h>

#include "ofono_voicecall_interface.h"

static void property_changed(OrgOfonoVoiceCall *voice_call,
			     gchar *property,
			     GVariant *value)
{
	GVariant *vvalue;
	const gchar *state;

	if (!strcmp(property, "State")) {
		vvalue = g_variant_get_variant(value);
		state = g_variant_get_string(vvalue, NULL);
		g_signal_emit_by_name(voice_call, "call-state-changed", state);
	}
}

OrgOfonoVoiceCall *ofono_voicecall_new(gchar *op,
				       void (*call_state_changed)(OrgOfonoVoiceCall *,gchar *))
{
	OrgOfonoVoiceCall *voice_call;

	voice_call = org_ofono_voice_call_proxy_new_for_bus_sync(
			G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE,
			"org.ofono", op, NULL, NULL);

	if (voice_call) {
		g_signal_new("call-state-changed",
			     G_TYPE_OBJECT,
			     G_SIGNAL_RUN_LAST,
			     0,
			     NULL,
			     NULL,
			     g_cclosure_marshal_generic,
			     G_TYPE_NONE,
			     1,
			     G_TYPE_STRING);

		if (g_signal_connect(G_OBJECT(voice_call), "call-state-changed", G_CALLBACK(call_state_changed), NULL) <= 0) {
	                AFB_ERROR("Failed to connect to signal call-state-changed\n");
					        }

		if (g_signal_connect(voice_call, "property-changed", G_CALLBACK(property_changed), NULL) <= 0) {
			AFB_ERROR("Failed to connect to signal call-added\n");
		}
	}

	return voice_call;
}

void ofono_voicecall_free(OrgOfonoVoiceCall *voice_call)
{
	g_object_unref(G_OBJECT(voice_call));
}

void ofono_voicecall_hangup(OrgOfonoVoiceCall *voice_call)
{
	GError *error = NULL;

	org_ofono_voice_call_call_hangup_sync(voice_call, NULL, &error);
}

void ofono_voicecall_answer(OrgOfonoVoiceCall *voice_call)
{
	GError *error = NULL;

	org_ofono_voice_call_call_answer_sync(voice_call, NULL, &error);
}

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

#include <glib.h>

#include "ofono_voicecallmanager_interface.h"

OrgOfonoVoiceCallManager
*ofono_voicecallmanager_init(const gchar *,
			     void(*)(OrgOfonoVoiceCallManager *, gchar *, gchar *),
			     void(*)(OrgOfonoVoiceCallManager *, gchar *, gchar *),
			     void(*)(OrgOfonoVoiceCallManager *, gchar *));
void ofono_voicecallmanager_free(OrgOfonoVoiceCallManager *);
gchar *ofono_voicecallmanager_dial(OrgOfonoVoiceCallManager *, gchar *, gchar *);
void ofono_hangup_all(OrgOfonoVoiceCallManager *);

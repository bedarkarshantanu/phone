#!/bin/sh

CG=gdbus-codegen
API=api

$CG \
	--interface-prefix Ofono.Manager. \
	--generate-c-code ofono_manager_interface \
	--c-generate-object-manager \
	$API/ofono_manager.xml

$CG \
	--interface-prefix Ofono.VoiceCallManager. \
	--generate-c-code ofono_voicecallmanager_interface \
	$API/ofono_voicecallmanager.xml

$CG \
	--interface-prefix Ofono.VoiceCall. \
	--generate-c-code ofono_voicecall_interface \
	$API/ofono_voice_call.xml

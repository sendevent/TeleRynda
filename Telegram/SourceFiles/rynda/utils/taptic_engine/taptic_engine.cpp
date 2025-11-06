// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "taptic_engine.h"

#if defined Q_OS_MAC
#include "rynda/utils/taptic_engine/platform/taptic_engine_mac.h"
#else
#include "rynda/utils/taptic_engine/platform/taptic_engine_dummy.h"
#endif

namespace TapticEngine {

void generateGeneric() {
	Impl::generateGeneric();
}

void generateAlignment() {
	Impl::generateAlignment();
}

void generateLevelChange() {
	Impl::generateLevelChange();
}

}

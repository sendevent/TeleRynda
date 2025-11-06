// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "rynda_infra.h"

#include "rynda/rynda_lang.h"
#include "rynda/rynda_settings.h"
#include "rynda/rynda_ui_settings.h"
#include "rynda/rynda_worker.h"
#include "rynda/data/rynda_database.h"
#include "features/translator/rynda_translator.h"
#include "features/filters/shadow_ban_utils.h"
#include "lang/lang_instance.h"
#include "utils/rc_manager.h"

namespace RyndaInfra {

void initLang() {
	QString id = Lang::GetInstance().id();
	QString baseId = Lang::GetInstance().baseId();
	if (id.isEmpty()) {
		LOG(("Language is not loaded"));
		return;
	}
	RyndaLanguage::init();
	RyndaLanguage::currentInstance()->fetchLanguage(id, baseId);
}

void initUiSettings() {
	const auto &settings = RyndaSettings::getInstance();

	RyndaUiSettings::setMonoFont(settings.monoFont);
	RyndaUiSettings::setWideMultiplier(settings.wideMultiplier);
	RyndaUiSettings::setMaterialSwitches(settings.materialSwitches);
}

void initDatabase() {
	RyndaDatabase::initialize();
}

void initWorker() {
	RyndaWorker::initialize();
}

void initRCManager() {
	RCManager::getInstance().start();
}

void initTranslator() {
	Rynda::Translator::TranslateManager::init();
}

void init() {
	initLang();
	initDatabase();
	initUiSettings();
	initWorker();
	initRCManager();
	initTranslator();
}

}

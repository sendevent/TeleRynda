// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#pragma once

#include "settings/settings_common.h"
#include "settings/settings_common_session.h"

class BoxContent;

namespace Window {
class Controller;
class SessionController;
}

namespace Settings {

class RyndaMain : public Section<RyndaMain>
{
public:
	RyndaMain(QWidget *parent, not_null<Window::SessionController*> controller);

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent(not_null<Window::SessionController*> controller);
};

}
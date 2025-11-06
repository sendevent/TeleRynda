// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "settings_appearance.h"

#include "lang_auto.h"
#include "rynda/rynda_settings.h"
#include "rynda/ui/boxes/font_selector.h"
#include "rynda/ui/components/icon_picker.h"
#include "inline_bots/bot_attach_web_view.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "styles/style_rynda_icons.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

namespace Settings {

namespace {

bool HasDrawerBots(not_null<Window::SessionController*> controller) {
	// todo: maybe iterate through all accounts
	const auto bots = &controller->session().attachWebView();
	for (const auto &bot : bots->attachBots()) {
		if (!bot.inMainMenu || !bot.media) {
			continue;
		}

		return true;
	}

	return false;
}

}

rpl::producer<QString> RyndaAppearance::title() {
	return tr::rynda_CategoryAppearance();
}

RyndaAppearance::RyndaAppearance(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
	: Section(parent) {
	setupContent(controller);
}

void SetupAppIcon(not_null<Ui::VerticalLayout*> container) {
	AddSubsectionTitle(container, tr::rynda_AppIconHeader());
	container->add(
		object_ptr<IconPicker>(container),
		st::settingsCheckboxPadding);

#ifdef Q_OS_WIN
	auto *settings = &RyndaSettings::getInstance();

	AddDivider(container);
	AddSkip(container);
	AddButtonWithIcon(
		container,
		tr::rynda_HideNotificationBadge(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->hideNotificationBadge)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->hideNotificationBadge);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_hideNotificationBadge(enabled);
			RyndaSettings::save();
		},
		container->lifetime());
	AddSkip(container);
	AddDividerText(container, tr::rynda_HideNotificationBadgeDescription());
	AddSkip(container);
#endif
}

void SetupAppearance(not_null<Ui::VerticalLayout*> container, not_null<Window::SessionController*> controller) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_CategoryAppearance());

	AddButtonWithIcon(
		container,
		tr::rynda_MaterialSwitches(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->materialSwitches)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->materialSwitches);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_materialSwitches(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_RemoveMessageTail(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->removeMessageTail)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->removeMessageTail);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_removeMessageTail(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_DisableCustomBackgrounds(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->disableCustomBackgrounds)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->disableCustomBackgrounds);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_disableCustomBackgrounds(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	const auto monoButton = AddButtonWithLabel(
		container,
		tr::rynda_MonospaceFont(),
		rpl::single(
			settings->monoFont.isEmpty() ? tr::rynda_FontDefault(tr::now) : settings->monoFont
		),
		st::settingsButtonNoIcon);
	const auto monoGuard = Ui::CreateChild<base::binary_guard>(monoButton.get());

	monoButton->addClickHandler(
		[=]
		{
			*monoGuard = RyndaUi::FontSelectorBox::Show(
				controller,
				[=](QString font)
				{
					RyndaSettings::set_monoFont(std::move(font));
					RyndaSettings::save();
				});
		});

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);
}

void SetupChatFolders(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_ChatFoldersHeader());

	AddButtonWithIcon(
		container,
		tr::rynda_HideNotificationCounters(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->hideNotificationCounters)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->hideNotificationCounters);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_hideNotificationCounters(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_HideAllChats(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->hideAllChatsFolder)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->hideAllChatsFolder);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_hideAllChatsFolder(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);
}

void SetupDrawerElements(not_null<Ui::VerticalLayout*> container, not_null<Window::SessionController*> controller) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_DrawerElementsHeader());

	AddButtonWithIcon(
		container,
		tr::lng_menu_my_profile(),
		st::settingsButton,
		{&st::menuIconProfile}
	)->toggleOn(
		rpl::single(settings->showMyProfileInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showMyProfileInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showMyProfileInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	if (HasDrawerBots(controller)) {
		AddButtonWithIcon(
			container,
			tr::lng_filters_type_bots(),
			st::settingsButton,
			{&st::menuIconBot}
		)->toggleOn(
			rpl::single(settings->showBotsInDrawer)
		)->toggledValue(
		) | rpl::filter(
			[=](bool enabled)
			{
				return (enabled != settings->showBotsInDrawer);
			}) | rpl::start_with_next(
			[=](bool enabled)
			{
				RyndaSettings::set_showBotsInDrawer(enabled);
				RyndaSettings::save();
			},
			container->lifetime());
	}

	AddButtonWithIcon(
		container,
		tr::lng_create_group_title(),
		st::settingsButton,
		{&st::menuIconGroups}
	)->toggleOn(
		rpl::single(settings->showNewGroupInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showNewGroupInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showNewGroupInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::lng_create_channel_title(),
		st::settingsButton,
		{&st::menuIconChannel}
	)->toggleOn(
		rpl::single(settings->showNewChannelInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showNewChannelInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showNewChannelInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::lng_menu_contacts(),
		st::settingsButton,
		{&st::menuIconUserShow}
	)->toggleOn(
		rpl::single(settings->showContactsInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showContactsInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showContactsInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::lng_menu_calls(),
		st::settingsButton,
		{&st::menuIconPhone}
	)->toggleOn(
		rpl::single(settings->showCallsInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showCallsInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showCallsInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::lng_saved_messages(),
		st::settingsButton,
		{&st::menuIconSavedMessages}
	)->toggleOn(
		rpl::single(settings->showSavedMessagesInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showSavedMessagesInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showSavedMessagesInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_LReadMessages(),
		st::settingsButton,
		{&st::ryndaLReadMenuIcon}
	)->toggleOn(
		rpl::single(settings->showLReadToggleInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showLReadToggleInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showLReadToggleInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_SReadMessages(),
		st::settingsButton,
		{&st::ryndaSReadMenuIcon}
	)->toggleOn(
		rpl::single(settings->showSReadToggleInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showSReadToggleInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showSReadToggleInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::lng_menu_night_mode(),
		st::settingsButton,
		{&st::menuIconNightMode}
	)->toggleOn(
		rpl::single(settings->showNightModeToggleInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showNightModeToggleInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showNightModeToggleInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_GhostModeToggle(),
		st::settingsButton,
		{&st::ryndaGhostIcon}
	)->toggleOn(
		rpl::single(settings->showGhostToggleInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showGhostToggleInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showGhostToggleInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

#ifdef WIN32
	AddButtonWithIcon(
		container,
		tr::rynda_StreamerModeToggle(),
		st::settingsButton,
		{&st::ryndaStreamerModeMenuIcon}
	)->toggleOn(
		rpl::single(settings->showStreamerToggleInDrawer)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showStreamerToggleInDrawer);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showStreamerToggleInDrawer(enabled);
			RyndaSettings::save();
		},
		container->lifetime());
#endif

	AddSkip(container);
}

void SetupTrayElements(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_TrayElementsHeader());

	AddButtonWithIcon(
		container,
		tr::rynda_EnableGhostModeTray(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->showGhostToggleInTray)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showGhostToggleInTray);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showGhostToggleInTray(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

#ifdef WIN32
	AddButtonWithIcon(
		container,
		tr::rynda_EnableStreamerModeTray(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->showStreamerToggleInTray)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showStreamerToggleInTray);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showStreamerToggleInTray(enabled);
			RyndaSettings::save();
		},
		container->lifetime());
#endif

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);
}

void RyndaAppearance::setupContent(not_null<Window::SessionController*> controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	AddSkip(content);

	SetupAppIcon(content);
	SetupAppearance(content, controller);
	SetupChatFolders(content);
	SetupTrayElements(content);
	SetupDrawerElements(content, controller);
	AddSkip(content);

	ResizeFitChild(this, content);
}

} // namespace Settings
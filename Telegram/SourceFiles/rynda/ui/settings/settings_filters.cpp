// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "settings_filters.h"

#include "lang_auto.h"
#include "rynda/rynda_settings.h"
#include "rynda/data/rynda_database.h"
#include "rynda/features/filters/filters_cache_controller.h"
#include "rynda/ui/boxes/import_filters_box.h"
#include "rynda/utils/telegram_helpers.h"
#include "boxes/abstract_box.h"
#include "boxes/peer_list_box.h"
#include "core/application.h"
#include "filters/per_dialog_filter.h"
#include "filters/settings_filters_list.h"
#include "inline_bots/bot_attach_web_view.h"
#include "settings/settings_common.h"
#include "styles/style_boxes.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/vertical_list.h"
#include "ui/boxes/confirm_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_controller.h"
#include "window/window_peer_menu.h"
#include "window/window_session_controller.h"

namespace Settings {

rpl::producer<QString> RyndaFilters::title() {
	return tr::rynda_CategoryFilters();
}

void RyndaFilters::fillTopBarMenu(const Ui::Menu::MenuCallback &addAction) {
	addAction(
		tr::rynda_FiltersMenuSelectChat(tr::now),
		[=]
		{
			if (const auto window = Core::App().activeWindow()) {
				if (const auto controller = window->sessionController()) {
					auto types = InlineBots::PeerTypes();
					types |= InlineBots::PeerType::Bot;
					types |= InlineBots::PeerType::Group;
					types |= InlineBots::PeerType::Broadcast;

					Window::ShowChooseRecipientBox(
						controller,
						[=](not_null<Data::Thread*> thread)
						{
							const auto peer = thread->peer();
							controller->dialogId = getDialogIdFromPeer(peer);
							controller->showExclude = true;
							controller->showSettings(RyndaFiltersList::Id());
							return true;
						},
						tr::rynda_FiltersMenuSelectChat(),
						nullptr,
						types
					);
				}
			}
		},
		&st::menuIconSearch);
	addAction({
		.isSeparator = true
	});
	addAction(
		tr::rynda_FiltersMenuImport(tr::now),
		[=]
		{
			auto box = Box(Ui::FillImportFiltersBox, true);
			Ui::show(std::move(box));
		},
		&st::menuIconArchive);
	if (RyndaDatabase::hasFilters()) {
		addAction(
			tr::rynda_FiltersMenuExport(tr::now),
			[=]
			{
				auto box = Box(Ui::FillImportFiltersBox, false);
				Ui::show(std::move(box));
			},
			&st::menuIconUnarchive);
	}
	addAction({
		.isSeparator = true
	});
	addAction(
		tr::rynda_FiltersMenuClear(tr::now),
		[=]
		{
			auto callback = [=](Fn<void()> &&close)
			{
				RyndaDatabase::deleteAllFilters();
				RyndaDatabase::deleteAllExclusions();
				FiltersCacheController::rebuildCache();
				RyndaSettings::fire_filtersUpdate();
				close();
			};

			auto box = Ui::MakeConfirmBox({
				.text = tr::rynda_FiltersClearPopupText(),
				.confirmed = callback,
				.confirmText = tr::rynda_FiltersClearPopupActionText()
			});
			Ui::show(std::move(box));
		},
		&st::menuIconClear);
}

RyndaFilters::RyndaFilters(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
	: Section(parent) {
	setupContent(controller);
}

void SetupFiltersSettings(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSkip(container);
	AddSubsectionTitle(container, tr::rynda_RegexFilters());

	AddButtonWithIcon(
		container,
		tr::rynda_RegexFiltersEnable(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->filtersEnabled)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->filtersEnabled);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_filtersEnabled(enabled);
			RyndaSettings::save();

			FiltersCacheController::rebuildCache();
			RyndaSettings::fire_filtersUpdate();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_RegexFiltersEnableSharedInChats(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->filtersEnabledInChats)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->filtersEnabledInChats);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_filtersEnabledInChats(enabled);
			RyndaSettings::save();

			FiltersCacheController::rebuildCache();
			RyndaSettings::fire_filtersUpdate();
		},
		container->lifetime());


	AddButtonWithIcon(
		container,
		tr::rynda_FiltersHideFromBlocked(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->hideFromBlocked)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->hideFromBlocked);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_hideFromBlocked(enabled);
			RyndaSettings::save();

			FiltersCacheController::rebuildCache();
			RyndaSettings::fire_filtersUpdate();
		},
		container->lifetime());
	AddSkip(container);
}

void SetupShared(not_null<Window::SessionController*> controller,
				 Ui::VerticalLayout *container) {
	Ui::AddSkip(container);

	auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		tr::rynda_RegexFiltersShared()
	));
	button->addClickHandler([=]
	{
		controller->dialogId = std::nullopt; // ensure we're handling shared filters
		controller->showExclude = false;
		controller->showSettings(RyndaFiltersList::Id());
	});
}

void SetupShadowBan(not_null<Window::SessionController*> controller,
					Ui::VerticalLayout *container) {
	auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		tr::rynda_FiltersShadowBan()
	));
	button->addClickHandler([=]
	{
		controller->dialogId = std::nullopt;
		controller->showExclude = false;
		controller->shadowBan = true;
		controller->showSettings(RyndaFiltersList::Id());
	});
}

void SetupPerDialog(
	not_null<Window::SessionController*> controller,
	not_null<Ui::VerticalLayout*> container
) {
	auto ctrl = container->lifetime().make_state<PerDialogFiltersListController>(
		&controller->session(),
		controller
	);

	auto list = object_ptr<Ui::PaddingWrap<PeerListContent>>(
		container,
		object_ptr<PeerListContent>(
			container,
			ctrl),
		QMargins(0, -st::peerListBox.padding.top(), 0, -st::peerListBox.padding.bottom()));
	AddSkip(container);
	const auto content = container->add(std::move(list));
	AddSkip(container);
	auto delegate = container->lifetime().make_state<PeerListContentDelegateSimple>();
	delegate->setContent(content->entity());
	ctrl->setDelegate(delegate);
}

void SetupMessageFilters(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_RegexFilters());

	AddButtonWithIcon(
		container,
		tr::rynda_FiltersHideFromBlocked(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->hideFromBlocked)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->hideFromBlocked);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_hideFromBlocked(enabled);
			RyndaSettings::save();
		},
		container->lifetime());
}

void RyndaFilters::setupContent(not_null<Window::SessionController*> controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	SetupFiltersSettings(content);

	AddDivider(content);

	SetupShared(controller, content);
	SetupShadowBan(controller, content);

	if (RyndaDatabase::hasPerDialogFilters()) {
		AddSkip(content);
		AddDivider(content);
		SetupPerDialog(controller, content);
	}

	ResizeFitChild(this, content);
}

} // namespace Settings

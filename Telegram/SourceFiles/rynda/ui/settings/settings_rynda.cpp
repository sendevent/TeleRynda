// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "settings_rynda.h"

#include "lang_auto.h"
#include "settings_rynda_utils.h"
#include "rynda/rynda_settings.h"
#include "boxes/peer_list_box.h"
#include "filters/settings_filters_list.h"
#include "settings/settings_common.h"
#include "styles/style_settings.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

namespace Settings {

rpl::producer<QString> RyndaGhost::title() {
	return rpl::single(QString("TeleRynda"));
}

RyndaGhost::RyndaGhost(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
	: Section(parent) {
	setupContent(controller);
}

void SetupGhostModeToggle(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_GhostEssentialsHeader());

	std::vector checkboxes{
		NestedEntry{
			tr::rynda_DontReadMessages(tr::now), !settings->sendReadMessages, [=](bool enabled)
			{
				RyndaSettings::set_sendReadMessages(!enabled);
				RyndaSettings::save();
			}
		},
		NestedEntry{
			tr::rynda_DontReadStories(tr::now), !settings->sendReadStories, [=](bool enabled)
			{
				RyndaSettings::set_sendReadStories(!enabled);
				RyndaSettings::save();
			}
		},
		NestedEntry{
			tr::rynda_DontSendOnlinePackets(tr::now), !settings->sendOnlinePackets, [=](bool enabled)
			{
				RyndaSettings::set_sendOnlinePackets(!enabled);
				RyndaSettings::save();
			}
		},
		NestedEntry{
			tr::rynda_DontSendUploadProgress(tr::now), !settings->sendUploadProgress, [=](bool enabled)
			{
				RyndaSettings::set_sendUploadProgress(!enabled);
				RyndaSettings::save();
			}
		},
		NestedEntry{
			tr::rynda_SendOfflinePacketAfterOnline(tr::now), settings->sendOfflinePacketAfterOnline, [=](bool enabled)
			{
				RyndaSettings::set_sendOfflinePacketAfterOnline(enabled);
				RyndaSettings::save();
			}
		},
	};

	AddCollapsibleToggle(container, tr::rynda_GhostModeToggle(), checkboxes, true);
}

void SetupGhostEssentials(
	not_null<Ui::VerticalLayout*> container,
	not_null<RyndaSettings::TeleRyndaSettings*> settings,
	not_null<rpl::variable<bool>*> markReadAfterActionVal,
	not_null<rpl::variable<bool>*> useScheduledMessagesVal) {
	SetupGhostModeToggle(container);


	AddButtonWithIcon(
		container,
		tr::rynda_MarkReadAfterAction(),
		st::settingsButtonNoIcon
	)->toggleOn(
		markReadAfterActionVal->value()
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->markReadAfterAction);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_markReadAfterAction(enabled);
			if (enabled) {
				RyndaSettings::set_useScheduledMessages(false);
				useScheduledMessagesVal->force_assign(false);
			}

			RyndaSettings::save();
		},
		container->lifetime());
	AddSkip(container);
	AddDividerText(container, tr::rynda_MarkReadAfterActionDescription());
}

void SetupScheduleMessages(
	not_null<Ui::VerticalLayout*> container,
	not_null<RyndaSettings::TeleRyndaSettings*> settings,
	not_null<rpl::variable<bool>*> markReadAfterActionVal,
	not_null<rpl::variable<bool>*> useScheduledMessagesVal) {
	AddSkip(container);
	AddButtonWithIcon(
		container,
		tr::rynda_UseScheduledMessages(),
		st::settingsButtonNoIcon
	)->toggleOn(
		useScheduledMessagesVal->value()
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->useScheduledMessages);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_useScheduledMessages(enabled);
			if (enabled) {
				RyndaSettings::set_markReadAfterAction(false);
				markReadAfterActionVal->force_assign(false);
			}

			RyndaSettings::save();
		},
		container->lifetime());
	AddSkip(container);
	AddDividerText(container, tr::rynda_UseScheduledMessagesDescription());
}

void SetupSendWithoutSound(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSkip(container);
	AddButtonWithIcon(
		container,
		tr::rynda_SendWithoutSoundByDefault(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->sendWithoutSound)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->sendWithoutSound);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_sendWithoutSound(enabled);
			RyndaSettings::save();
		},
		container->lifetime());
	AddSkip(container);
	AddDividerText(container, tr::rynda_SendWithoutSoundByDefaultDescription());
}

void SetupSpyEssentials(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_SpyEssentialsHeader());

	AddButtonWithIcon(
		container,
		tr::rynda_SaveDeletedMessages(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->saveDeletedMessages)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->saveDeletedMessages);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_saveDeletedMessages(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_SaveMessagesHistory(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->saveMessagesHistory)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->saveMessagesHistory);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_saveMessagesHistory(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);

	AddButtonWithIcon(
		container,
		tr::rynda_MessageSavingSaveForBots(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->saveForBots)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->saveForBots);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_saveForBots(enabled);
			RyndaSettings::save();
		},
		container->lifetime());
}

void SetupOther(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_MessageSavingOtherHeader());

	AddButtonWithIcon(
		container,
		tr::rynda_LocalPremium(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->localPremium)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->localPremium);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_localPremium(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_DisableAds(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->disableAds)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->disableAds);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_disableAds(enabled);
			RyndaSettings::save();
		},
		container->lifetime());
}

void RyndaGhost::setupContent(not_null<Window::SessionController*> controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	const auto settings = &RyndaSettings::getInstance();
	const auto markReadAfterActionVal = content->lifetime().make_state<rpl::variable<bool>>(
		settings->markReadAfterAction);
	const auto useScheduledMessagesVal = content->lifetime().make_state<rpl::variable<bool>>(
		settings->useScheduledMessages);

	AddSkip(content);

	SetupGhostEssentials(content, settings, markReadAfterActionVal, useScheduledMessagesVal);
	SetupScheduleMessages(content, settings, markReadAfterActionVal, useScheduledMessagesVal);
	SetupSendWithoutSound(content);

	AddSkip(content);
	SetupSpyEssentials(content);

	AddSkip(content);
	AddDivider(content);
	AddSkip(content);

	SetupOther(content);
	AddSkip(content);

	ResizeFitChild(this, content);
}

} // namespace Settings

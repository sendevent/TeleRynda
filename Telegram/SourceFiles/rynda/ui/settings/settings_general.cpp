// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "settings_general.h"

#include "lang_auto.h"
#include "settings_rynda_utils.h"
#include "rynda/rynda_settings.h"
#include "settings/settings_common.h"
#include "styles/style_settings.h"
#include "ui/vertical_list.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

namespace Settings {

rpl::producer<QString> RyndaGeneral::title() {
	return tr::rynda_CategoryGeneral();
}

RyndaGeneral::RyndaGeneral(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
	: Section(parent) {
	setupContent(controller);
}

void SetupTranslator(not_null<Ui::VerticalLayout*> container,
					 not_null<Window::SessionController*> controller) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::lng_translate_settings_subtitle()/*rpl::single(QString("Translate Messages"))*/);

	const auto options = std::vector{
		QString("Telegram"),
		QString("Google"),
		QString("Yandex")
	};

	const auto getIndex = [=](const QString &val)
	{
		if (val == "telegram") {
			return 0;
		}
		if (val == "google") {
			return 1;
		}
		if (val == "yandex") {
			return 2;
		}
		return 0;
	};

	auto currentVal = RyndaSettings::get_translationProviderReactive() | rpl::map(getIndex) | rpl::map(
		[=](int val)
		{
			return options[val];
		});

	const auto button = AddButtonWithLabel(
		container,
		asBeta(tr::rynda_TranslationProvider()),
		currentVal,
		st::settingsButtonNoIcon);
	button->addClickHandler(
		[=]
		{
			controller->show(Box(
				[=](not_null<Ui::GenericBox*> box)
				{
					const auto save = [=](int index)
					{
						const auto provider = (index == 0) ? "telegram" : (index == 1) ? "google" : "yandex";

						RyndaSettings::set_translationProvider(provider);
						RyndaSettings::save();
					};
					SingleChoiceBox(box,
									{
										.title = tr::rynda_TranslationProvider(),
										.options = options,
										.initialSelection = getIndex(settings->translationProvider),
										.callback = save,
									});
				}));
		});
}

void SetupShowPeerId(not_null<Ui::VerticalLayout*> container,
					 not_null<Window::SessionController*> controller) {
	auto *settings = &RyndaSettings::getInstance();

	const auto options = std::vector{
		QString(tr::rynda_SettingsShowID_Hide(tr::now)),
		QString("Telegram API"),
		QString("Bot API")
	};

	auto currentVal = RyndaSettings::get_showPeerIdReactive() | rpl::map(
		[=](int val)
		{
			return options[val];
		});

	const auto button = AddButtonWithLabel(
		container,
		tr::rynda_SettingsShowID(),
		currentVal,
		st::settingsButtonNoIcon);
	button->addClickHandler(
		[=]
		{
			controller->show(Box(
				[=](not_null<Ui::GenericBox*> box)
				{
					const auto save = [=](int index)
					{
						RyndaSettings::set_showPeerId(index);
						RyndaSettings::save();
					};
					SingleChoiceBox(box,
									{
										.title = tr::rynda_SettingsShowID(),
										.options = options,
										.initialSelection = settings->showPeerId,
										.callback = save,
									});
				}));
		});
}

void SetupQoLToggles(not_null<Ui::VerticalLayout*> container, not_null<Window::SessionController*> controller) {
	auto *settings = &RyndaSettings::getInstance();

	SetupTranslator(container, controller);
	AddSkip(container);
	AddDivider(container);
	AddSkip(container);

	AddSubsectionTitle(container, tr::rynda_CategoryGeneral());

	AddButtonWithIcon(
		container,
		tr::rynda_DisableStories(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->disableStories)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->disableStories);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_disableStories(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	std::vector checkboxes = {
		NestedEntry{
			tr::rynda_CollapseSimilarChannels(tr::now), settings->collapseSimilarChannels, [=](bool enabled)
			{
				RyndaSettings::set_collapseSimilarChannels(enabled);
				RyndaSettings::save();
			}
		},
		NestedEntry{
			tr::rynda_HideSimilarChannelsTab(tr::now), settings->hideSimilarChannels, [=](bool enabled)
			{
				RyndaSettings::set_hideSimilarChannels(enabled);
				RyndaSettings::save();
			}
		}
	};

	AddCollapsibleToggle(container, tr::rynda_DisableSimilarChannels(), checkboxes, true);


	AddButtonWithIcon(
		container,
		tr::rynda_DisableNotificationsDelay(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->disableNotificationsDelay)
	)->toggledValue(
	) | rpl::filter([=](bool enabled)
	{
		return (enabled != settings->disableNotificationsDelay);
	}) | start_with_next([=](bool enabled)
						 {
							 RyndaSettings::set_disableNotificationsDelay(enabled);
							 RyndaSettings::save();
						 },
						 container->lifetime());

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);

	AddButtonWithIcon(
		container,
		tr::rynda_SettingsShowMessageSeconds(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->showMessageSeconds)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showMessageSeconds);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showMessageSeconds(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	SetupShowPeerId(container, controller);

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);

	AddSubsectionTitle(container, rpl::single(QString("Webview")));

	AddButtonWithIcon(
		container,
		tr::rynda_SettingsSpoofWebviewAsAndroid(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->spoofWebviewAsAndroid)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->spoofWebviewAsAndroid);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_spoofWebviewAsAndroid(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	std::vector webviewCheckboxes = {
		NestedEntry{
			tr::rynda_SettingsIncreaseWebviewHeight(tr::now), settings->increaseWebviewHeight, [=](bool enabled)
			{
				RyndaSettings::set_increaseWebviewHeight(enabled);
				RyndaSettings::save();
			}
		},
		NestedEntry{
			tr::rynda_SettingsIncreaseWebviewWidth(tr::now), settings->increaseWebviewWidth, [=](bool enabled)
			{
				RyndaSettings::set_increaseWebviewWidth(enabled);
				RyndaSettings::save();
			}
		}
	};

	AddCollapsibleToggle(container, tr::rynda_SettingsBiggerWindow(), webviewCheckboxes, false);

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);

	// todo: move into a single checkbox with dropdown
	AddSubsectionTitle(container, tr::rynda_ConfirmationsTitle());

	AddButtonWithIcon(
		container,
		tr::rynda_StickerConfirmation(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->stickerConfirmation)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->stickerConfirmation);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_stickerConfirmation(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_GIFConfirmation(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->gifConfirmation)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->gifConfirmation);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_gifConfirmation(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_VoiceConfirmation(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->voiceConfirmation)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->voiceConfirmation);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_voiceConfirmation(enabled);
			RyndaSettings::save();
		},
		container->lifetime());
}

void RyndaGeneral::setupContent(not_null<Window::SessionController*> controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	AddSkip(content);
	SetupQoLToggles(content, controller);
	AddSkip(content);

	ResizeFitChild(this, content);
}

} // namespace Settings
// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "settings_chats.h"

#include <styles/style_rynda_icons.h>

#include "lang_auto.h"
#include "rynda/rynda_settings.h"
#include "rynda/ui/boxes/edit_mark_box.h"
#include "rynda/ui/settings/settings_rynda_utils.h"
#include "core/application.h"
#include "settings/settings_common.h"
#include "styles/style_rynda_styles.h"
#include "styles/style_boxes.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/vertical_list.h"
#include "ui/boxes/confirm_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/continuous_sliders.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

namespace Settings {

rpl::producer<QString> RyndaChats::title() {
	return tr::rynda_CategoryChats();
}

RyndaChats::RyndaChats(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
	: Section(parent) {
	setupContent(controller);
}

void SetupStickersAndEmojiSettings(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::lng_settings_stickers_emoji()/*rpl::single(QString("Stickers and Emoji"))*/);

	AddButtonWithIcon(
		container,
		tr::rynda_ShowOnlyAddedEmojisAndStickers(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->showOnlyAddedEmojisAndStickers)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showOnlyAddedEmojisAndStickers);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showOnlyAddedEmojisAndStickers(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	std::vector checkboxes = {
		NestedEntry{
			tr::rynda_HideReactionsInChannels(tr::now), !settings->showChannelReactions, [=](bool enabled)
			{
				RyndaSettings::set_hideChannelReactions(!enabled);
				RyndaSettings::save();
			}
		},
		NestedEntry{
			tr::rynda_HideReactionsInGroups(tr::now), !settings->showGroupReactions, [=](bool enabled)
			{
				RyndaSettings::set_hideGroupReactions(!enabled);
				RyndaSettings::save();
			}
		}
	};

	AddCollapsibleToggle(container, tr::rynda_HideReactions(), checkboxes, false);

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);
}

void SetupRecentStickersLimit(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	container->add(
		object_ptr<Button>(container,
						   tr::rynda_SettingsRecentStickersCount(),
						   st::settingsButtonNoIcon)
	)->setAttribute(Qt::WA_TransparentForMouseEvents);

	auto recentStickersLimitSlider = MakeSliderWithLabel(
		container,
		st::autoDownloadLimitSlider,
		st::settingsScaleLabel,
		0,
		st::settingsScaleLabel.style.font->width("8%%"));
	container->add(std::move(recentStickersLimitSlider.widget), st::recentStickersLimitPadding);
	const auto slider = recentStickersLimitSlider.slider;
	const auto label = recentStickersLimitSlider.label;

	const auto updateLabel = [=](int amount)
	{
		label->setText(QString::number(amount));
	};
	updateLabel(settings->recentStickersCount);

	slider->setPseudoDiscrete(
		200 + 1,
		[=](int amount)
		{
			return amount;
		},
		settings->recentStickersCount,
		[=](int amount)
		{
			updateLabel(amount);
		},
		[=](int amount)
		{
			updateLabel(amount);
			RyndaSettings::set_recentStickersCount(amount);
			RyndaSettings::save();
		});

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);
}

void SetupGroupsAndChannels(not_null<Ui::VerticalLayout*> container, not_null<Window::SessionController*> controller) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container,
					   tr::lng_premium_double_limits_subtitle_channels()
					   /*rpl::single(QString("Groups and Channels"))*/);

	const auto options = std::vector{
		tr::rynda_ChannelBottomButtonHide(tr::now),
		tr::rynda_ChannelBottomButtonMute(tr::now),
		tr::rynda_ChannelBottomButtonDiscuss(tr::now),
	};

	AddChooseButtonWithIconAndRightText(
		container,
		controller,
		settings->channelBottomButton,
		options,
		tr::rynda_ChannelBottomButton(),
		tr::rynda_ChannelBottomButton(),
		[=](int index)
		{
			RyndaSettings::set_channelBottomButton(index);
			RyndaSettings::save();
		});

	AddButtonWithIcon(
		container,
		tr::rynda_QuickAdminShortcuts(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->quickAdminShortcuts)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->quickAdminShortcuts);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_quickAdminShortcuts(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_SettingsShowMessageShot(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->showMessageShot)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showMessageShot);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showMessageShot(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddSkip(container);
	AddDividerText(container, tr::rynda_SettingsShowMessageShotDescription());
	AddSkip(container);
}

void SetupMarks(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::lng_settings_messages()/*rpl::single(QString("Messages"))*/);

	AddButtonWithLabel(
		container,
		tr::rynda_DeletedMarkText(),
		RyndaSettings::get_deletedMarkReactive(),
		st::settingsButtonNoIcon
	)->addClickHandler(
		[=]()
		{
			auto box = Box<EditMarkBox>(
				tr::rynda_DeletedMarkText(),
				settings->deletedMark,
				QString("🧹"),
				[=](const QString &value)
				{
					RyndaSettings::set_deletedMark(value);
					RyndaSettings::save();
				}
			);
			Ui::show(std::move(box));
		});

	AddButtonWithLabel(
		container,
		tr::rynda_EditedMarkText(),
		RyndaSettings::get_editedMarkReactive(),
		st::settingsButtonNoIcon
	)->addClickHandler(
		[=]()
		{
			auto box = Box<EditMarkBox>(
				tr::rynda_EditedMarkText(),
				settings->editedMark,
				tr::lng_edited(tr::now),
				[=](const QString &value)
				{
					RyndaSettings::set_editedMark(value);
					RyndaSettings::save();
				}
			);
			Ui::show(std::move(box));
		});

	AddButtonWithIcon(
		container,
		tr::rynda_ReplaceMarksWithIcons(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->replaceBottomInfoWithIcons)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->replaceBottomInfoWithIcons);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_replaceBottomInfoWithIcons(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);

	AddButtonWithIcon(
		container,
		tr::rynda_HideShareButton(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->hideFastShare)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->hideFastShare);
		}) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_hideFastShare(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_SimpleQuotesAndReplies(),
		st::settingsButtonNoIcon
	)->toggleOn(
		rpl::single(settings->simpleQuotesAndReplies)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->simpleQuotesAndReplies);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_simpleQuotesAndReplies(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);
}

void SetupWideMessagesMultiplier(not_null<Ui::VerticalLayout*> container,
								 not_null<Window::SessionController*> controller) {
	auto *settings = &RyndaSettings::getInstance();

	container->add(
		object_ptr<Button>(container,
						   tr::rynda_SettingsWideMultiplier(),
						   st::settingsButtonNoIcon)
	)->setAttribute(Qt::WA_TransparentForMouseEvents);

	auto wideMultiplierSlider = MakeSliderWithLabel(
		container,
		st::autoDownloadLimitSlider,
		st::settingsScaleLabel,
		0,
		st::settingsScaleLabel.style.font->width("8%%%"));
	container->add(std::move(wideMultiplierSlider.widget), st::recentStickersLimitPadding);
	const auto slider = wideMultiplierSlider.slider;
	const auto label = wideMultiplierSlider.label;

	const auto updateLabel = [=](double val)
	{
		label->setText(QString::number(val, 'f', 2));
	};

	constexpr auto kSizeAmount = 61; // (4.00 - 1.00) / 0.05 + 1
	constexpr auto kMinSize = 1.00;
	constexpr auto kStep = 0.05;

	const auto valueToIndex = [=](double value)
	{
		return static_cast<int>(std::round((value - kMinSize) / kStep));
	};
	const auto indexToValue = [=](int index)
	{
		return kMinSize + index * kStep;
	};

	updateLabel(settings->wideMultiplier);

	slider->setPseudoDiscrete(
		kSizeAmount,
		[=](int index) { return index; },
		valueToIndex(settings->wideMultiplier),
		[=](int index)
		{
			updateLabel(indexToValue(index));
		},
		[=](int index)
		{
			updateLabel(indexToValue(index));
			RyndaSettings::set_wideMultiplier(indexToValue(index));
			RyndaSettings::save();

			// fix slider
			crl::on_main([=]
			{
				controller->show(Ui::MakeConfirmBox({
					.text = tr::lng_settings_need_restart(),
					.confirmed = []
					{
						Core::Restart();
					},
					.confirmText = tr::lng_settings_restart_now(),
					.cancelText = tr::lng_settings_restart_later(),
				}));
			});
		});

	AddSkip(container);
	AddDividerText(container, tr::rynda_SettingsWideMultiplierDescription());
	AddSkip(container);
}

void SetupContextMenuElements(not_null<Ui::VerticalLayout*> container,
							  not_null<Window::SessionController*> controller) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_ContextMenuElementsHeader());

	const auto options = std::vector{
		tr::rynda_SettingsContextMenuItemHidden(tr::now),
		tr::rynda_SettingsContextMenuItemShown(tr::now),
		tr::rynda_SettingsContextMenuItemExtended(tr::now),
	};

	AddChooseButtonWithIconAndRightText(
		container,
		controller,
		settings->showReactionsPanelInContextMenu,
		options,
		tr::rynda_SettingsContextMenuReactionsPanel(),
		tr::rynda_SettingsContextMenuTitle(),
		st::menuIconReactions,
		[=](int index)
		{
			RyndaSettings::set_showReactionsPanelInContextMenu(index);
			RyndaSettings::save();
		});
	AddChooseButtonWithIconAndRightText(
		container,
		controller,
		settings->showViewsPanelInContextMenu,
		options,
		tr::rynda_SettingsContextMenuViewsPanel(),
		tr::rynda_SettingsContextMenuTitle(),
		st::menuIconShowInChat,
		[=](int index)
		{
			RyndaSettings::set_showViewsPanelInContextMenu(index);
			RyndaSettings::save();
		});

	AddChooseButtonWithIconAndRightText(
		container,
		controller,
		settings->showHideMessageInContextMenu,
		options,
		tr::rynda_ContextHideMessage(),
		tr::rynda_SettingsContextMenuTitle(),
		st::menuIconClear,
		[=](int index)
		{
			RyndaSettings::set_showHideMessageInContextMenu(index);
			RyndaSettings::save();
		});
	AddChooseButtonWithIconAndRightText(
		container,
		controller,
		settings->showUserMessagesInContextMenu,
		options,
		tr::rynda_UserMessagesMenuText(),
		tr::rynda_SettingsContextMenuTitle(),
		st::menuIconTTL,
		[=](int index)
		{
			RyndaSettings::set_showUserMessagesInContextMenu(index);
			RyndaSettings::save();
		});
	AddChooseButtonWithIconAndRightText(
		container,
		controller,
		settings->showMessageDetailsInContextMenu,
		options,
		tr::rynda_MessageDetailsPC(),
		tr::rynda_SettingsContextMenuTitle(),
		st::menuIconInfo,
		[=](int index)
		{
			RyndaSettings::set_showMessageDetailsInContextMenu(index);
			RyndaSettings::save();
		});
	AddChooseButtonWithIconAndRightText(
		container,
		controller,
		settings->showRepeatMessageInContextMenu,
		options,
		tr::rynda_RepeatMessage(),
		tr::rynda_SettingsContextMenuTitle(),
		st::menuIconRestore,
		[=](int index)
		{
			RyndaSettings::set_showRepeatMessageInContextMenu(index);
			RyndaSettings::save();
		});
	if (settings->filtersEnabled) {
		AddChooseButtonWithIconAndRightText(
			container,
			controller,
			settings->showAddFilterInContextMenu,
			options,
			tr::rynda_RegexFilterQuickAdd(),
			tr::rynda_SettingsContextMenuTitle(),
			st::menuIconAddToFolder,
			[=](int index)
			{
				RyndaSettings::set_showAddFilterInContextMenu(index);
				RyndaSettings::save();
			});
	}

	AddSkip(container);
	AddDividerText(container, tr::rynda_SettingsContextMenuDescription());
	AddSkip(container);
}

void SetupMessageFieldElements(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_MessageFieldElementsHeader());

	AddButtonWithIcon(
		container,
		tr::rynda_MessageFieldElementAttach(),
		st::settingsButton,
		{&st::messageFieldAttachIcon}
	)->toggleOn(
		rpl::single(settings->showAttachButtonInMessageField)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showAttachButtonInMessageField);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showAttachButtonInMessageField(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_MessageFieldElementCommands(),
		st::settingsButton,
		{&st::messageFieldCommandsIcon}
	)->toggleOn(
		rpl::single(settings->showCommandsButtonInMessageField)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showCommandsButtonInMessageField);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showCommandsButtonInMessageField(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_MessageFieldElementTTL(),
		st::settingsButton,
		{&st::messageFieldTTLIcon}
	)->toggleOn(
		rpl::single(settings->showAutoDeleteButtonInMessageField)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showAutoDeleteButtonInMessageField);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showAutoDeleteButtonInMessageField(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_MessageFieldElementEmoji(),
		st::settingsButton,
		{&st::messageFieldEmojiIcon}
	)->toggleOn(
		rpl::single(settings->showEmojiButtonInMessageField)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showEmojiButtonInMessageField);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showEmojiButtonInMessageField(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_MessageFieldElementVoice(),
		st::settingsButton,
		{&st::messageFieldVoiceIcon}
	)->toggleOn(
		rpl::single(settings->showMicrophoneButtonInMessageField)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showMicrophoneButtonInMessageField);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showMicrophoneButtonInMessageField(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddSkip(container);
	AddDivider(container);
	AddSkip(container);
}

void SetupMessageFieldPopups(not_null<Ui::VerticalLayout*> container) {
	auto *settings = &RyndaSettings::getInstance();

	AddSubsectionTitle(container, tr::rynda_MessageFieldPopupsHeader());

	AddButtonWithIcon(
		container,
		tr::rynda_MessageFieldElementAttach(),
		st::settingsButton,
		{&st::messageFieldAttachIcon}
	)->toggleOn(
		rpl::single(settings->showAttachPopup)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showAttachPopup);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showAttachPopup(enabled);
			RyndaSettings::save();
		},
		container->lifetime());

	AddButtonWithIcon(
		container,
		tr::rynda_MessageFieldElementEmoji(),
		st::settingsButton,
		{&st::messageFieldEmojiIcon}
	)->toggleOn(
		rpl::single(settings->showEmojiPopup)
	)->toggledValue(
	) | rpl::filter(
		[=](bool enabled)
		{
			return (enabled != settings->showEmojiPopup);
		}) | rpl::start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_showEmojiPopup(enabled);
			RyndaSettings::save();
		},
		container->lifetime());
}

void RyndaChats::setupContent(not_null<Window::SessionController*> controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	AddSkip(content);
	SetupStickersAndEmojiSettings(content);
	SetupRecentStickersLimit(content);

	SetupGroupsAndChannels(content, controller);

	SetupMarks(content);
	SetupWideMessagesMultiplier(content, controller);

	SetupContextMenuElements(content, controller);
	SetupMessageFieldElements(content);

	SetupMessageFieldPopups(content);
	AddSkip(content);

	ResizeFitChild(this, content);
}

} // namespace Settings

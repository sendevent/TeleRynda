// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "settings_main.h"

#include <QDesktopServices>
#include <styles/style_rynda_icons.h>

#include "lang_auto.h"
#include "settings_appearance.h"
#include "settings_rynda.h"
#include "settings_rynda_utils.h"
#include "settings_chats.h"
#include "settings_filters.h"
#include "settings_general.h"
#include "settings_other.h"

#include "rynda/rynda_settings.h"
#include "rynda/ui/rynda_logo.h"
#include "core/version.h"
#include "settings/settings_common.h"
#include "styles/style_rynda_settings.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/painter.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "window/window_session_controller_link_info.h"

namespace Settings {

rpl::producer<QString> RyndaMain::title() {
	return rpl::single(QString(""));
}

RyndaMain::RyndaMain(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
	: Section(parent) {
	setupContent(controller);
}

void SetupAppLogo(not_null<Ui::VerticalLayout*> container) {
	const auto logo = container->add(
		object_ptr<Ui::RpWidget>(container),
		style::al_top);

	logo->resize(QSize(st::settingsCloudPasswordIconSize, st::settingsCloudPasswordIconSize));
	logo->setNaturalWidth(st::settingsCloudPasswordIconSize);

	logo->paintRequest(
	) | rpl::start_with_next([=](QRect clip)
							 {
								 auto p = QPainter(logo);
								 const auto image = RyndaAssets::currentAppLogoPad();
								 if (!image.isNull()) {
									 const auto size = st::settingsCloudPasswordIconSize;
									 const auto scaled = image.scaled(
										 size * style::DevicePixelRatio(),
										 size * style::DevicePixelRatio(),
										 Qt::KeepAspectRatio,
										 Qt::SmoothTransformation);
									 p.drawImage(
										 QRect(0, 0, size, size),
										 scaled);
								 }
							 },
							 logo->lifetime());
}

void SetupCategories(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller,
	Fn<void(Type)> showOther) {
	struct CategoryInfo
	{
		QString name;
		const style::icon *icon;
		std::function<void()> handler;
	};

	const auto categories = std::vector<CategoryInfo>{
		{QString("TeleRynda"), &st::menuIconGroupReactions, [=] { showOther(RyndaGhost::Id()); }},
		{asBeta(tr::rynda_CategoryFilters(tr::now)), &st::menuIconTagFilter, [=] { showOther(RyndaFilters::Id()); }},
		{tr::rynda_CategoryGeneral(tr::now), &st::menuIconShowAll, [=] { showOther(RyndaGeneral::Id()); }},
		{tr::rynda_CategoryAppearance(tr::now), &st::menuIconPalette, [=] { showOther(RyndaAppearance::Id()); }},
		{tr::rynda_CategoryChats(tr::now), &st::menuIconChatBubble, [=] { showOther(RyndaChats::Id()); }},
		{tr::rynda_CategoryOther(tr::now), &st::menuIconFave, [=] { showOther(RyndaOther::Id()); }},
	};

	for (const auto &category : categories) {
		AddButtonWithIcon(
			container,
			rpl::single(category.name),
			st::settingsButton,
			{category.icon}
		)->setClickedCallback([=]
		{
			if (category.handler) {
				category.handler();
			}
		});
	}
}

void SetupLinks(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller) {
	struct LinkInfo
	{
		QString name;
		QString value;
		const style::icon *icon;
		std::function<void()> handler;
	};

	const auto links = std::vector<LinkInfo>{
		{
			tr::rynda_LinksChannel(tr::now),
			QString("@telerynda"),
			&st::menuIconChannel,
			[=]
			{
				controller->showPeerByLink(Window::PeerByLinkInfo{
					.usernameOrId = QString("telerynda"),
				});
			}
		},
		{
			tr::rynda_LinksChats(tr::now),
			QString("@teleryndachat"),
			&st::menuIconChats,
			[=]
			{
				controller->showPeerByLink(Window::PeerByLinkInfo{
					.usernameOrId = QString("teleryndachat"),
				});
			}
		},
		{
			tr::rynda_LinksTranslate(tr::now),
			QString("Crowdin"),
			&st::menuIconTranslate,
			[=]
			{
				QDesktopServices::openUrl(QString("https://translate.telerynda.one"));
			}
		},
		{
			tr::rynda_LinksDocumentation(tr::now),
			QString("docs.telerynda.one"),
			&st::menuIconIpAddress,
			[=]
			{
				QDesktopServices::openUrl(QString("https://docs.telerynda.one"));
			}
		},
	};

	for (const auto &link : links) {
		AddButtonWithLabel(
			container,
			rpl::single(link.name),
			rpl::single(link.value),
			st::settingsButton,
			{link.icon}
		)->setClickedCallback(link.handler);
	}
}

void RyndaMain::setupContent(not_null<Window::SessionController*> controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	SetupAppLogo(content);

	AddSkip(content);

	content->add(
		object_ptr<Ui::FlatLabel>(
			content,
			rpl::single(QString("TeleRynda Desktop v") + QString::fromLatin1(AppVersionStr)),
			st::boxTitle),
		style::al_top);

	AddSkip(content);

	content->add(
		object_ptr<Ui::FlatLabel>(
			content,
			tr::rynda_SettingsDescription(),
			st::centeredBoxLabel),
		style::al_top);

	AddSkip(content);
	AddSkip(content);
	AddSkip(content);
	AddSkip(content);
	AddDivider(content);
	AddSkip(content);

	AddSubsectionTitle(content, tr::rynda_CategoriesHeader());
	SetupCategories(content, controller, showOtherMethod());

	AddSkip(content);
	AddDivider(content);
	AddSkip(content);

	AddSubsectionTitle(content, tr::rynda_LinksHeader());
	SetupLinks(content, controller);

	AddSkip(content);

	ResizeFitChild(this, content);
}

} // namespace Settings

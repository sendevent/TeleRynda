// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "message_shot_box.h"

#include <QFileDialog>
#include <QGuiApplication>
#include "styles/style_rynda_styles.h"

#include "lang_auto.h"
#include "theme_selector_box.h"
#include "rynda/rynda_settings.h"
#include "rynda/ui/components/image_view.h"
#include "boxes/abstract_box.h"
#include "core/core_settings.h"
#include "data/data_session.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

MessageShotBox::MessageShotBox(
	QWidget *parent,
	RyndaFeatures::MessageShot::ShotConfig config)
	: _config(std::move(config)) {
}

void MessageShotBox::prepare() {
	setupContent();
}

void MessageShotBox::setupContent() {
	_selectedPalette = std::make_shared<style::palette>();

	const auto &settings = RyndaSettings::getInstance();
	const auto savedShowColorfulReplies = !settings.simpleQuotesAndReplies;

	using namespace Settings;

	RyndaFeatures::MessageShot::setShotConfig(_config);

	setTitle(rpl::single(QString("Message Shot")));

	auto wrap = object_ptr<Ui::VerticalLayout>(this);
	const auto content = wrap.data();
	setInnerWidget(object_ptr<Ui::OverrideMargins>(this, std::move(wrap)));

	AddSubsectionTitle(content, tr::rynda_MessageShotPreview());

	const auto imageView = content->add(object_ptr<ImageView>(content), st::imageViewPadding);

	AddSkip(content);
	AddDivider(content);
	AddSkip(content);
	AddSubsectionTitle(content, tr::rynda_MessageShotPreferences());

	const auto updatePreview = [=]
	{
		const auto image = RyndaFeatures::MessageShot::Make(this, _config);
		imageView->setImage(image);
	};

	auto selectedTheme =
		content->lifetime().make_state<rpl::variable<QString>>(tr::rynda_MessageShotThemeDefault(tr::now));

	AddButtonWithLabel(
		content,
		tr::rynda_MessageShotTheme(),
		selectedTheme->value(),
		st::settingsButtonNoIcon
	)->addClickHandler(
		[=]
		{
			RyndaFeatures::MessageShot::setChoosingTheme(true);

			auto box = Box<ThemeSelectorBox>(_config.controller);
			box->paletteSelected() | rpl::start_with_next(
				[=](const style::palette &palette) mutable
				{
					_selectedPalette->reset();
					_selectedPalette->load(palette.save());

					_config.st = std::make_shared<Ui::ChatStyle>(
						_selectedPalette.get());

					updatePreview();
				},
				content->lifetime());

			box->themeNameChanged() | rpl::start_with_next(
				[=](const QString &name)
				{
					selectedTheme->force_assign(name);
				},
				content->lifetime());

			box->boxClosing() | rpl::start_with_next(
				[=]
				{
					RyndaFeatures::MessageShot::setChoosingTheme(false);
				},
				content->lifetime());

			Ui::show(std::move(box), Ui::LayerOption::KeepOther);
		});
	AddButtonWithIcon(
		content,
		tr::rynda_MessageShotShowBackground(),
		st::settingsButtonNoIcon
	)->toggleOn(rpl::single(_config.showBackground)
	)->toggledValue(
	) | start_with_next(
		[=](bool enabled)
		{
			_config.showBackground = enabled;

			updatePreview();
		},
		content->lifetime());

	AddButtonWithIcon(
		content,
		tr::rynda_MessageShotShowDate(),
		st::settingsButtonNoIcon
	)->toggleOn(rpl::single(_config.showDate)
	)->toggledValue(
	) | start_with_next(
		[=](bool enabled)
		{
			_config.showDate = enabled;

			updatePreview();
		},
		content->lifetime());

	AddButtonWithIcon(
		content,
		tr::rynda_MessageShotShowReactions(),
		st::settingsButtonNoIcon
	)->toggleOn(rpl::single(_config.showReactions)
	)->toggledValue(
	) | start_with_next(
		[=](bool enabled)
		{
			_config.showReactions = enabled;

			updatePreview();
		},
		content->lifetime());

	const auto latestToggle = AddButtonWithIcon(
		content,
		tr::rynda_MessageShotShowColorfulReplies(),
		st::settingsButtonNoIcon
	);
	latestToggle->toggleOn(rpl::single(savedShowColorfulReplies)
	)->toggledValue(
	) | start_with_next(
		[=](bool enabled)
		{
			RyndaSettings::set_simpleQuotesAndReplies(!enabled);

			_config.st = std::make_shared<Ui::ChatStyle>(_config.st.get());
			updatePreview();
		},
		content->lifetime());

	AddSkip(content);

	addButton(tr::rynda_MessageShotSave(),
			  [=]
			  {
				  const auto image = imageView->getImage();
				  const auto path = QFileDialog::getSaveFileName(
					  this,
					  tr::lng_save_file(tr::now),
					  QString(),
					  "*.png");

				  if (!path.isEmpty()) {
					  image.save(path);
				  }

				  closeBox();
			  });
	addButton(tr::rynda_MessageShotCopy(),
			  [=]
			  {
				  QGuiApplication::clipboard()->setImage(imageView->getImage());

				  closeBox();
			  });

	updatePreview();

	const auto boxWidth = imageView->getImage().width() / style::DevicePixelRatio() + (st::boxPadding.left() + st::boxPadding.right()) * 4;

	boxClosing() | rpl::start_with_next(
		[=]
		{
			RyndaFeatures::MessageShot::resetCustomSelected();
			RyndaFeatures::MessageShot::resetDefaultSelected();
			RyndaFeatures::MessageShot::resetShotConfig();

			RyndaSettings::set_simpleQuotesAndReplies(!savedShowColorfulReplies);
		},
		content->lifetime());

	setDimensionsToContent(boxWidth, content);

	// scroll to the end
	scrollToWidget(latestToggle);
}

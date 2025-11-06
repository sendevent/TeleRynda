// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#pragma once

#define ICON(name, value) const auto name##_ICON = QStringLiteral(value)

namespace AyuAssets {

// Define your two custom icons here
// The string in quotes is the folder name in Resources/art/ayu/
ICON(APP_WHITE, "app_white");
ICON(APP_BLACK, "app_black");

void loadAppIco();

QImage loadPreview(const QString& name);

QString currentAppLogoName();
QImage currentAppLogo();
QImage currentAppLogoPad();

}

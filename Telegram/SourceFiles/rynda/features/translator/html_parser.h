// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#pragma once

#include <QtCore/QString>
#include "ui/text/text_entity.h"

namespace Rynda::Translator::Html {

[[nodiscard]] QString entitiesToHtml(const TextWithEntities &text);
[[nodiscard]] TextWithEntities htmlToEntities(const QString &text);

}
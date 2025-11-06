// This is the source code of TeleRynda for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#pragma once

#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

class RyndaLanguage : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(RyndaLanguage)

public:
	static RyndaLanguage *currentInstance();
	static void init();
	static RyndaLanguage *instance;

	void fetchLanguage(const QString &id, const QString &baseId);
	void applyLanguageJson(QJsonDocument doc);

public Q_SLOTS:
	void fetchFinished();
	void fetchError(QNetworkReply::NetworkError e);

private:
	RyndaLanguage();
	~RyndaLanguage() override = default;

	QNetworkAccessManager networkManager;
	QNetworkReply *_chkReply = nullptr;
	bool needFallback = false;
};

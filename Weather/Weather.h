#pragma once

#include <QtWidgets/QWidget>
#include "ui_Weather.h"
#include "qsettings.h"
#include "qmessagebox.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"
#include "qjsonarray.h"
#include "qbuttongroup.h"
#include "qsslsocket.h"
#include "qsslconfiguration.h"
#include "qfile.h"
#include "qdir.h"
#include "qtimer.h"
#include "qimage.h"
#include "qpixmap.h"
#include "qmutex.h"

class Weather : public QWidget
{
    Q_OBJECT

protected:
	Ui::WeatherClass ui;
	QSettings* conf;
	QNetworkAccessManager* networkAccessMenager;
	QTimer* timer;
	QMutex* mutex;

	const QString defaultConfPath = "./conf.ini";
	void readConf(QString path) {
		conf = new QSettings(path, QSettings::IniFormat);
		if (conf->status() != QSettings::NoError) {
			QMessageBox::warning(this, "Error", "The conf.ini is not available!");
			QApplication::exit(-1);
		}
		if (conf->value("api_key").toString().isEmpty()) {
			QMessageBox::warning(this, "Error", "The 'api_key' in conf.ini is not set!");
			QApplication::exit(-1);
		}
	}

	const QString defaultIconPath = "./WeatherIcons/";
	bool setWeatherIcon(int id) {
		QDir dir = QDir(defaultIconPath);
		QString filename = QString("%1.png").arg(id);
		if (!dir.exists(filename)) return false;


		QImage img = QImage(dir.filePath(filename));
		QPixmap pm = QPixmap::fromImage(img);
		ui.l_image->setPixmap(pm.scaledToHeight(height()-10));
		return true;
	}

public slots:
	void refresh() {
		auto location = conf->value("location_small", "dongcheng").toString() + "," + conf->value("location_large", "beijing").toString();

		QString str_url =
			QString("https://free-api.heweather.com/s6/weather/forecast?location=%1&key=%2")
			.arg(location)
			.arg(conf->value("api_key").toString());

		QNetworkRequest request = QNetworkRequest(QUrl(str_url));

		QSslConfiguration config;
		QSslConfiguration conf = request.sslConfiguration();
		conf.setPeerVerifyMode(QSslSocket::VerifyNone);
		conf.setProtocol(QSsl::TlsV1SslV3);
		request.setSslConfiguration(conf);

		//forcast
		auto reply = networkAccessMenager->get(request);
		reply->waitForReadyRead(10000);


		//the moment
		request.setUrl(QUrl(str_url.replace("forecast", "now")));
		networkAccessMenager->get(request);
		reply->waitForReadyRead(10000);
	}

private slots:
	void httpRequestFinished(QNetworkReply* reply) {

		auto s = QString(reply->readAll());
		if (s.isEmpty()) {
			return;
		}

		auto doc = QJsonDocument::fromJson(s.toUtf8());
		auto root = doc.object();

		if (!root.contains("HeWeather6")) {
			QMessageBox::warning(this, "Web Request Error", "Cannot get data from the server. The return is as follows\n" + s);
			return;
		}

		auto body = root["HeWeather6"].toArray()[0].toObject().toVariantMap();

		if (!body.contains("basic")) {
			QMessageBox::warning(this, "Web Request Error", "Cannot get data from the server. The return is as follows\n" + s);
			return;
		}

		if (body.contains("now")) {
			auto now = body["now"].toMap();
			auto info = body["basic"].toMap();

			ui.l_region->setText(info["location"].toString());
			ui.l_txt_weather->setText(now["cond_txt"].toString());
			ui.l_current_temp->setText(now["tmp"].toString());
			int weather_code = now["cond_code"].toInt();
			setWeatherIcon(weather_code);
		}
		else //forecast
		{
			auto p = body["daily_forecast"];
			auto s = p.typeName();
			auto forecast = body["daily_forecast"].toList()[0].toMap();

			ui.l_highest_temp->setText(forecast["tmp_max"].toString());
			ui.l_lowest_temp->setText(forecast["tmp_min"].toString());
		}
	}

public:
    Weather(QWidget *parent = Q_NULLPTR): QWidget(parent)
	{
		ui.setupUi(this);
		readConf(defaultConfPath);

		//Init Window
		this->setGeometry(
			conf->value("x", 0).toInt(),
			conf->value("y", 0).toInt(),
			conf->value("width", 180).toInt(), 
			conf->value("height", 200).toInt()
		);
		setWindowFlag(Qt::Tool);//For No Icon in Taskbar

		//To set the window fully transparent
		setWindowFlag(Qt::FramelessWindowHint);
		setWindowOpacity(1);
		this->setAttribute(Qt::WA_TranslucentBackground, true);

		QPalette pal = palette();
		pal.setColor(QPalette::Background, Qt::transparent);
		setPalette(pal);

		//init classes
		networkAccessMenager = new QNetworkAccessManager(this);
		timer = new QTimer(this);
		timer->setSingleShot(false);
		timer->setInterval(conf->value("refresh_secs", 1800).toInt()*1000l);
		mutex = new QMutex();

		//init signals
		connect(networkAccessMenager, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpRequestFinished(QNetworkReply*)));
		connect(ui.b_refresh, SIGNAL(clicked()), this, SLOT(refresh()));
		connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
		connect(ui.b_exit, SIGNAL(clicked()),qApp, SLOT(quit()));


		timer->start();
		refresh();
	}


	
};

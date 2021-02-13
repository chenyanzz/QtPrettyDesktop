#pragma once

#include <QtWidgets/QWidget>
#include "ui_Weather.h"

class Weather : public QWidget
{
    Q_OBJECT

public:
    Weather(QWidget *parent = Q_NULLPTR);

private:
    Ui::WeatherClass ui;
};

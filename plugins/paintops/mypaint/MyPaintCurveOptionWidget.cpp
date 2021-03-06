/*
 * Copyright (C) 2008 Boudewijn Rempt <boud@valdyas.org>
 * Copyright (C) 2009 Sven Langkamp   <sven.langkamp@gmail.com>
 * Copyright (C) 2011 Silvio Heinrich <plassy@web.de>
 * Copyright (c) 2020 Ashwin Dhakaita <ashwingpdhakaita@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <QJsonObject>
#include <QJsonDocument>

#include <kis_global.h>
#include <kis_icon_utils.h>
#include <kis_signals_blocker.h>
#include <widgets/kis_curve_widget.h>

#include "ui_wdgmypaintcurveoption.h"
#include "MyPaintPaintOpOption.h"
#include "MyPaintBrushOption.h"
#include "MyPaintCurveOption.h"
#include "MyPaintPaintOpSettingsWidget.h"
#include "MyPaintCurveOptionWidget.h"

KisMyPaintCurveOptionWidget::KisMyPaintCurveOptionWidget(KisMyPaintCurveOption* curveOption, const QString &minLabel, const QString &maxLabel, bool hideSlider, KisMyPaintOpOption *baseOption)
    : KisCurveOptionWidget (curveOption, minLabel, maxLabel, hideSlider)
{
    Q_UNUSED(baseOption);
    setObjectName("KisMyPaintCurveOptionWidget");   

    strengthToCurveOptionValueScale = 1.0;
    hideRangeLabelsAndBoxes(false);

    connect(m_curveOptionWidget->sensorSelector, SIGNAL(highlightedSensorChanged(KisDynamicSensorSP)), SLOT(updateRangeSpinBoxes(KisDynamicSensorSP)));
    connect(m_curveOptionWidget->xMinBox, SIGNAL(valueChanged(double)), SLOT(emitSettingChanged()));
    connect(m_curveOptionWidget->xMaxBox, SIGNAL(valueChanged(double)), SLOT(emitSettingChanged()));
    connect(m_curveOptionWidget->yMinBox, SIGNAL(valueChanged(double)), SLOT(emitSettingChanged()));
    connect(m_curveOptionWidget->yMaxBox, SIGNAL(valueChanged(double)), SLOT(emitSettingChanged()));

    m_curveOptionWidget->strengthSlider->setRange(curveOption->minValue(), curveOption->maxValue(), 2);
    m_curveOptionWidget->strengthSlider->setValue(curveOption->value());
    m_curveOptionWidget->strengthSlider->setPrefix(i18n("Base Value: "));
    m_curveOptionWidget->strengthSlider->setSuffix("");

    if (hideSlider) {
        m_curveOptionWidget->strengthSlider->hide();
    }

    connect(m_curveOption, SIGNAL(unCheckUseCurve()), SLOT(slotUnCheckUseCurve()));
}

KisMyPaintCurveOptionWidget::~KisMyPaintCurveOptionWidget()
{
}

void KisMyPaintCurveOptionWidget::writeOptionSetting(KisPropertiesConfigurationSP setting) const
{    
    checkRanges();
    KisMyPaintBrushOption* currentSensor = dynamic_cast<KisMyPaintBrushOption*>(m_curveOptionWidget->sensorSelector->currentHighlighted().data());
    setting->setProperty(m_curveOption->name() + currentSensor->identifier() + "XMIN", m_curveOptionWidget->xMinBox->value());
    setting->setProperty(m_curveOption->name() + currentSensor->identifier() + "XMAX", m_curveOptionWidget->xMaxBox->value());
    setting->setProperty(m_curveOption->name() + currentSensor->identifier() + "YMIN", m_curveOptionWidget->yMinBox->value());
    setting->setProperty(m_curveOption->name() + currentSensor->identifier() + "YMAX", m_curveOptionWidget->yMaxBox->value());

    currentSensor->setXRangeMin(m_curveOptionWidget->xMinBox->value());
    currentSensor->setXRangeMax(m_curveOptionWidget->xMaxBox->value());
    currentSensor->setYRangeMin(m_curveOptionWidget->yMinBox->value());
    currentSensor->setYRangeMax(m_curveOptionWidget->yMaxBox->value());

    updateSensorCurveLabels(currentSensor);
    setBaseValue(setting, m_curveOptionWidget->strengthSlider->value());
    m_curveOption->writeOptionSetting(setting);    
}

void KisMyPaintCurveOptionWidget::readOptionSetting(const KisPropertiesConfigurationSP setting)
{    
    KisCurveOptionWidget::readOptionSetting(setting);

    m_curveOptionWidget->checkBoxUseCurve->setChecked(m_curveOption->isCurveUsed());
    m_curveOptionWidget->strengthSlider->setValue(getBaseValue(setting));
    updateRangeSpinBoxes(m_curveOptionWidget->sensorSelector->currentHighlighted());
}

void KisMyPaintCurveOptionWidget::slotUnCheckUseCurve() {

    m_curveOptionWidget->checkBoxUseCurve->setChecked(false);
    updateValues();
}

void KisMyPaintCurveOptionWidget::updateSensorCurveLabels(KisDynamicSensorSP sensor) const
{    
    KisCurveOptionWidget::updateSensorCurveLabels(sensor);
    KisMyPaintBrushOption* mySensor = dynamic_cast<KisMyPaintBrushOption*>(sensor.data());
    if (mySensor) {

        m_curveOptionWidget->label_xmin->setText(mySensor->minimumXLabel());
        m_curveOptionWidget->label_xmax->setText(mySensor->maximumXLabel());
        m_curveOptionWidget->label_ymin->setText(mySensor->minimumYLabel());
        m_curveOptionWidget->label_ymax->setText(mySensor->maximumYLabel());
    }
}

void KisMyPaintCurveOptionWidget::updateRangeSpinBoxes(KisDynamicSensorSP sensor) const {

    KisMyPaintBrushOption* mySensor = dynamic_cast<KisMyPaintBrushOption*>(sensor.data());
    m_curveOptionWidget->xMinBox->blockSignals(true);
    m_curveOptionWidget->xMaxBox->blockSignals(true);
    m_curveOptionWidget->yMinBox->blockSignals(true);
    m_curveOptionWidget->yMaxBox->blockSignals(true);

    m_curveOptionWidget->xMinBox->setValue( mySensor->getXRangeMin());
    m_curveOptionWidget->xMaxBox->setValue( mySensor->getXRangeMax());
    m_curveOptionWidget->yMinBox->setValue( mySensor->getYRangeMin());
    m_curveOptionWidget->yMaxBox->setValue( mySensor->getYRangeMax());

    m_curveOptionWidget->xMinBox->blockSignals(false);
    m_curveOptionWidget->xMaxBox->blockSignals(false);
    m_curveOptionWidget->yMinBox->blockSignals(false);
    m_curveOptionWidget->yMaxBox->blockSignals(false);
}

float KisMyPaintCurveOptionWidget::getBaseValue(KisPropertiesConfigurationSP setting) {

    KisMyPaintCurveOption *curveOpt = dynamic_cast<KisMyPaintCurveOption*>(m_curveOption);
    if(curveOpt->currentSetting() == MYPAINT_BRUSH_SETTING_RADIUS_LOGARITHMIC)
        return log(setting->getFloat(MYPAINT_DIAMETER)/2);

    if(curveOpt->currentSetting() == MYPAINT_BRUSH_SETTING_OPAQUE)
        return setting->getFloat(MYPAINT_OPACITY);

    if(curveOpt->currentSetting() == MYPAINT_BRUSH_SETTING_HARDNESS)
        return setting->getFloat(MYPAINT_HARDNESS);

    MyPaintBrush *brush = mypaint_brush_new();
    mypaint_brush_from_string(brush, setting->getProperty(MYPAINT_JSON).toByteArray());

    float ret = mypaint_brush_get_base_value(brush, curveOpt->currentSetting());

    mypaint_brush_unref(brush);
    return ret;
}

void KisMyPaintCurveOptionWidget::setBaseValue(KisPropertiesConfigurationSP setting, float val) const {

    QJsonDocument doc = QJsonDocument::fromJson(setting->getProperty(MYPAINT_JSON).toByteArray());
    QJsonObject brush_json = doc.object();
    QVariantMap map = brush_json.toVariantMap();
    QVariantMap settings_map = map["settings"].toMap();
    QVariantMap name_map = settings_map[m_curveOption->name()].toMap();
    double base_val = name_map["base_value"].toDouble();
    Q_UNUSED(base_val);

    name_map["base_value"] = val;
    settings_map[m_curveOption->name()] = name_map;
    map["settings"] = settings_map;

    QJsonObject json_obj = QJsonObject::fromVariantMap(map);
    QJsonDocument doc2(json_obj);

    setting->setProperty(MYPAINT_JSON, doc2.toJson());    

    KisMyPaintCurveOption *curveOpt = dynamic_cast<KisMyPaintCurveOption*>(m_curveOption);
    if(curveOpt->currentSetting() == MYPAINT_BRUSH_SETTING_RADIUS_LOGARITHMIC)
        setting->setProperty(MYPAINT_DIAMETER, exp(val)*2);

    if(curveOpt->currentSetting() == MYPAINT_BRUSH_SETTING_HARDNESS)
        setting->setProperty(MYPAINT_HARDNESS, val);

    if(curveOpt->currentSetting() == MYPAINT_BRUSH_SETTING_OPAQUE)
        setting->setProperty(MYPAINT_OPACITY, val);

    if(curveOpt->currentSetting() == MYPAINT_BRUSH_SETTING_OFFSET_BY_RANDOM)
        setting->setProperty(MYPAINT_OFFSET_BY_RANDOM, val);
}

void KisMyPaintCurveOptionWidget::checkRanges() const
{
    if(m_curveOptionWidget->xMinBox->value() >= m_curveOptionWidget->xMaxBox->value()) {

        m_curveOptionWidget->xMinBox->blockSignals(true);
        m_curveOptionWidget->xMinBox->setValue(m_curveOptionWidget->xMaxBox->value() - 1);
        m_curveOptionWidget->xMinBox->blockSignals(false);
    }

    if(m_curveOptionWidget->yMinBox->value() >= m_curveOptionWidget->yMaxBox->value()) {

        m_curveOptionWidget->yMinBox->blockSignals(true);
        m_curveOptionWidget->yMinBox->setValue(m_curveOptionWidget->yMaxBox->value() - 1);
        m_curveOptionWidget->yMinBox->blockSignals(false);
    }
}

KisDoubleSliderSpinBox* KisMyPaintCurveOptionWidget::slider() {

    return m_curveOptionWidget->strengthSlider;
}

void KisMyPaintCurveOptionWidget::refresh() {

    emitSettingChanged();
}

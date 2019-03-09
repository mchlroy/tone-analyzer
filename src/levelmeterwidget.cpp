#include "levelmeter.h"
#include "levelmeterwidget.h"

#include <QtPlugin>

LevelMeterWidget::LevelMeterWidget(QObject *parent)
    : QObject(parent)
{
}

void LevelMeterWidget::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool LevelMeterWidget::isInitialized() const
{
    return initialized;
}

QWidget *LevelMeterWidget::createWidget(QWidget *parent)
{
    return new LevelMeter(parent);
}

QString LevelMeterWidget::name() const
{
    return QStringLiteral("LevelMeter");
}

QString LevelMeterWidget::group() const
{
    return QStringLiteral("Audio Widgets");
}

QIcon LevelMeterWidget::icon() const
{
    return QIcon();
}

QString LevelMeterWidget::toolTip() const
{
    return QString();
}

QString LevelMeterWidget::whatsThis() const
{
    return QString();
}

bool LevelMeterWidget::isContainer() const
{
    return false;
}

QString LevelMeterWidget::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"LevelMeter\" name=\"levelMeter\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>100</width>\n"
           "    <height>800</height>\n"
           "   </rect>\n"
           "  </property>\n"
           " </widget>\n"
           "</ui>";
}

QString LevelMeterWidget::includeFile() const
{
    return QStringLiteral("levelmeter.h");
}

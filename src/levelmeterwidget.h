#ifndef LEVELMETERWIDGET_H
#define LEVELMETERWIDGET_H

#include <QDesignerCustomWidgetInterface>

class LevelMeterWidget : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
//! [1]
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")
//! [1]
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    explicit LevelMeterWidget(QObject *parent = nullptr);

    bool isContainer() const override;
    bool isInitialized() const override;
    QIcon icon() const override;
    QString domXml() const override;
    QString group() const override;
    QString includeFile() const override;
    QString name() const override;
    QString toolTip() const override;
    QString whatsThis() const override;
    QWidget *createWidget(QWidget *parent) override;
    void initialize(QDesignerFormEditorInterface *core) override;

private:
    bool initialized = false;
};

#endif // LEVELMETERWIDGET_H

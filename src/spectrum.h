ifndef SPECTRUM_H
#define SPECTRUM_H

#include <QWidget>

class Spectrum  : public QWidget
{
    Q_OBJECT

public:
    Spectrum(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};


#endif // SPECTRUM_H

#pragma once

#include <QWidget>
#include <QColor>
#include <QLineEdit>
#include <QSpinBox>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>

class ColorPreviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit ColorPreviewWidget(QWidget *parent = nullptr);
    void setColor(const QColor &color);
    QColor color() const;

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QColor m_color;
};

class ColorPickerWidget : public QWidget {
    Q_OBJECT
public:
    explicit ColorPickerWidget(const QString &title, const QString &colorKey,
                               const QColor &defaultColor, QWidget *parent = nullptr);

    QString colorKey() const { return m_colorKey; }
    QColor currentColor() const { return m_color; }
    void setColor(const QColor &color);
    void setColorWithoutSignal(const QColor &color);

signals:
    void colorChanged(const QString &key, const QColor &color);

private slots:
    void onHexChanged();
    void onRgbChanged();
    void onHsvChanged();
    void onAlphaChanged();
    void onPreviewClicked();
    void updatePreviews();

private:
    void setupUI();
    void updateHexFromColor();
    void updateRgbFromColor();
    void updateHsvFromColor();
    void updateAlphaFromColor();
    void blockSignalsAll(bool block);

    QString m_title;
    QString m_colorKey;
    QColor m_color;

    ColorPreviewWidget *m_preview;
    QLineEdit *m_hexInput;
    QSpinBox *m_rSpinBox;
    QSpinBox *m_gSpinBox;
    QSpinBox *m_bSpinBox;
    QDoubleSpinBox *m_hSpinBox;
    QDoubleSpinBox *m_sSpinBox;
    QDoubleSpinBox *m_vSpinBox;
    QSlider *m_alphaSlider;
    QLabel *m_alphaLabel;
    QPushButton *m_pickButton;
};

#include "ColorPickerWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QApplication>

/* ---- ColorPreviewWidget ---- */

ColorPreviewWidget::ColorPreviewWidget(QWidget *parent)
    : QWidget(parent), m_color(Qt::black) {
    setFixedSize(48, 48);
    setCursor(Qt::PointingHandCursor);
    setToolTip("Click to open color picker dialog");
}

void ColorPreviewWidget::setColor(const QColor &color) {
    m_color = color;
    update();
}

QColor ColorPreviewWidget::color() const { return m_color; }

void ColorPreviewWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    QRect r = rect().adjusted(1, 1, -1, -1);

    p.setPen(Qt::NoPen);

    int checkSize = 8;
    for (int y = 0; y < r.height(); y += checkSize) {
        for (int x = 0; x < r.width(); x += checkSize) {
            bool even = ((x / checkSize) + (y / checkSize)) % 2 == 0;
            p.fillRect(r.x() + x, r.y() + y, checkSize, checkSize,
                       even ? Qt::lightGray : Qt::darkGray);
        }
    }

    p.fillRect(r, m_color);

    p.setPen(palette().color(QPalette::Mid));
    p.drawRect(r);
}

void ColorPreviewWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

/* ---- ColorPickerWidget ---- */

ColorPickerWidget::ColorPickerWidget(const QString &title, const QString &colorKey,
                                     const QColor &defaultColor, QWidget *parent)
    : QWidget(parent), m_title(title), m_colorKey(colorKey), m_color(defaultColor) {
    setupUI();
}

void ColorPickerWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(6);

    QGroupBox *group = new QGroupBox(m_title, this);
    QVBoxLayout *groupLayout = new QVBoxLayout(group);
    groupLayout->setSpacing(6);
    groupLayout->setContentsMargins(8, 12, 8, 8);

    QHBoxLayout *topRow = new QHBoxLayout();
    topRow->setSpacing(8);

    m_preview = new ColorPreviewWidget(this);
    m_preview->setColor(m_color);
    connect(m_preview, &ColorPreviewWidget::clicked, this, &ColorPickerWidget::onPreviewClicked);

    QVBoxLayout *hexLayout = new QVBoxLayout();
    QLabel *hexLabel = new QLabel("Hex:", this);
    hexLabel->setStyleSheet("font-size: 10px; color: #888;");
    m_hexInput = new QLineEdit(this);
    m_hexInput->setPlaceholderText("#RRGGBB");
    m_hexInput->setMaxLength(9);
    m_hexInput->setFixedWidth(90);
    connect(m_hexInput, &QLineEdit::editingFinished, this, &ColorPickerWidget::onHexChanged);
    hexLayout->addWidget(hexLabel);
    hexLayout->addWidget(m_hexInput);

    topRow->addWidget(m_preview);
    topRow->addLayout(hexLayout);
    topRow->addStretch();

    m_pickButton = new QPushButton("Pick...", this);
    m_pickButton->setFixedWidth(60);
    connect(m_pickButton, &QPushButton::clicked, this, &ColorPickerWidget::onPreviewClicked);
    topRow->addWidget(m_pickButton);

    groupLayout->addLayout(topRow);

    QHBoxLayout *rgbaLayout = new QHBoxLayout();
    rgbaLayout->setSpacing(8);

    auto makeRgbSpinBox = [this, &rgbaLayout](const QString &label, int initial, QSpinBox *&spin) {
        QVBoxLayout *vl = new QVBoxLayout();
        QLabel *lbl = new QLabel(label, this);
        lbl->setStyleSheet("font-size: 10px; color: #888;");
        spin = new QSpinBox(this);
        spin->setRange(0, 255);
        spin->setValue(initial);
        spin->setFixedWidth(60);
        connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ColorPickerWidget::onRgbChanged);
        vl->addWidget(lbl);
        vl->addWidget(spin);
        rgbaLayout->addLayout(vl);
    };

    makeRgbSpinBox("R", m_color.red(), m_rSpinBox);
    makeRgbSpinBox("G", m_color.green(), m_gSpinBox);
    makeRgbSpinBox("B", m_color.blue(), m_bSpinBox);

    QVBoxLayout *alphaVl = new QVBoxLayout();
    QLabel *alphaLbl = new QLabel("A", this);
    alphaLbl->setStyleSheet("font-size: 10px; color: #888;");
    m_alphaSlider = new QSlider(Qt::Horizontal, this);
    m_alphaSlider->setRange(0, 255);
    m_alphaSlider->setValue(m_color.alpha());
    m_alphaSlider->setFixedWidth(80);
    connect(m_alphaSlider, &QSlider::valueChanged, this, &ColorPickerWidget::onAlphaChanged);
    m_alphaLabel = new QLabel(QString::number(m_color.alpha()), this);
    m_alphaLabel->setFixedWidth(24);
    m_alphaLabel->setStyleSheet("font-size: 10px; color: #888;");
    alphaVl->addWidget(alphaLbl);
    QHBoxLayout *alphaInner = new QHBoxLayout();
    alphaInner->addWidget(m_alphaSlider);
    alphaInner->addWidget(m_alphaLabel);
    alphaVl->addLayout(alphaInner);
    rgbaLayout->addLayout(alphaVl);

    groupLayout->addLayout(rgbaLayout);

    QHBoxLayout *hsvLayout = new QHBoxLayout();
    hsvLayout->setSpacing(8);

    auto makeHsvSpinBox = [this, &hsvLayout](const QString &label, double initial,
                                 QDoubleSpinBox *&spin, double max, int decimals) {
        QVBoxLayout *vl = new QVBoxLayout();
        QLabel *lbl = new QLabel(label, this);
        lbl->setStyleSheet("font-size: 10px; color: #888;");
        spin = new QDoubleSpinBox(this);
        spin->setRange(0.0, max);
        spin->setDecimals(decimals);
        spin->setValue(initial);
        spin->setFixedWidth(70);
        connect(spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorPickerWidget::onHsvChanged);
        vl->addWidget(lbl);
        vl->addWidget(spin);
        hsvLayout->addLayout(vl);
    };

    makeHsvSpinBox("H", m_color.hueF() * 360.0, m_hSpinBox, 360.0, 1);
    makeHsvSpinBox("S", m_color.saturationF() * 100.0, m_sSpinBox, 100.0, 1);
    makeHsvSpinBox("V", m_color.valueF() * 100.0, m_vSpinBox, 100.0, 1);

    groupLayout->addLayout(hsvLayout);
    mainLayout->addWidget(group);

    updateHexFromColor();
}

void ColorPickerWidget::blockSignalsAll(bool block) {
    if (m_hexInput) m_hexInput->blockSignals(block);
    if (m_rSpinBox) m_rSpinBox->blockSignals(block);
    if (m_gSpinBox) m_gSpinBox->blockSignals(block);
    if (m_bSpinBox) m_bSpinBox->blockSignals(block);
    if (m_hSpinBox) m_hSpinBox->blockSignals(block);
    if (m_sSpinBox) m_sSpinBox->blockSignals(block);
    if (m_vSpinBox) m_vSpinBox->blockSignals(block);
    if (m_alphaSlider) m_alphaSlider->blockSignals(block);
}

void ColorPickerWidget::setColor(const QColor &color) {
    if (m_color == color && color.alpha() == m_color.alpha()) return;
    m_color = color;
    updatePreviews();
    emit colorChanged(m_colorKey, m_color);
}

void ColorPickerWidget::setColorWithoutSignal(const QColor &color) {
    m_color = color;
    updatePreviews();
}

void ColorPickerWidget::updatePreviews() {
    blockSignalsAll(true);
    updateHexFromColor();
    updateRgbFromColor();
    updateHsvFromColor();
    updateAlphaFromColor();
    m_preview->setColor(m_color);
    blockSignalsAll(false);
}

void ColorPickerWidget::onHexChanged() {
    QString hex = m_hexInput->text().trimmed();
    if (!hex.startsWith('#')) hex = "#" + hex;
    QColor c(hex);
    if (c.isValid()) {
        c.setAlpha(m_color.alpha());
        m_color = c;
        updatePreviews();
        m_preview->setColor(m_color);
        emit colorChanged(m_colorKey, m_color);
    }
}

void ColorPickerWidget::onRgbChanged() {
    m_color.setRgb(m_rSpinBox->value(), m_gSpinBox->value(), m_bSpinBox->value(), m_color.alpha());
    updatePreviews();
    m_preview->setColor(m_color);
    emit colorChanged(m_colorKey, m_color);
}

void ColorPickerWidget::onHsvChanged() {
    qreal h = m_hSpinBox->value() / 360.0;
    qreal s = m_sSpinBox->value() / 100.0;
    qreal v = m_vSpinBox->value() / 100.0;
    m_color.setHsvF(h, s, v, m_color.alphaF());
    updatePreviews();
    m_preview->setColor(m_color);
    emit colorChanged(m_colorKey, m_color);
}

void ColorPickerWidget::onAlphaChanged() {
    m_color.setAlpha(m_alphaSlider->value());
    m_alphaLabel->setText(QString::number(m_color.alpha()));
    updateHexFromColor();
    m_preview->setColor(m_color);
    emit colorChanged(m_colorKey, m_color);
}

void ColorPickerWidget::onPreviewClicked() {
    QColor newColor = QColorDialog::getColor(m_color, this, "Pick " + m_title);
    if (newColor.isValid()) {
        newColor.setAlpha(m_color.alpha());
        setColor(newColor);
    }
}

void ColorPickerWidget::updateHexFromColor() {
    QString hex;
    if (m_color.alpha() < 255) {
        hex = m_color.name(QColor::HexArgb);
    } else {
        hex = m_color.name(QColor::HexRgb);
    }
    m_hexInput->setText(hex);
}

void ColorPickerWidget::updateRgbFromColor() {
    m_rSpinBox->setValue(m_color.red());
    m_gSpinBox->setValue(m_color.green());
    m_bSpinBox->setValue(m_color.blue());
}

void ColorPickerWidget::updateHsvFromColor() {
    m_hSpinBox->setValue(m_color.hueF() * 360.0);
    m_sSpinBox->setValue(m_color.saturationF() * 100.0);
    m_vSpinBox->setValue(m_color.valueF() * 100.0);
}

void ColorPickerWidget::updateAlphaFromColor() {
    m_alphaSlider->setValue(m_color.alpha());
    m_alphaLabel->setText(QString::number(m_color.alpha()));
}

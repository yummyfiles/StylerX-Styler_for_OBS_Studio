#include "QSSEditorWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QTextBlock>
#include <QShortcut>
#include <QScrollBar>

/* ---- QSSHighlighter ---- */

QSSHighlighter::QSSHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {

    m_selectorFormat.setForeground(QColor("#cba6f7"));
    m_selectorFormat.setFontWeight(QFont::Bold);

    m_propertyFormat.setForeground(QColor("#89b4fa"));

    m_valueFormat.setForeground(QColor("#a6e3a1"));

    m_commentFormat.setForeground(QColor("#6c7086"));
    m_commentFormat.setFontItalic(true);

    m_punctuationFormat.setForeground(QColor("#f5e0dc"));

    m_numberFormat.setForeground(QColor("#fab387"));

    m_hexColorFormat.setForeground(QColor("#f9e2af"));

    m_commentStart = QRegularExpression("/\\*");
    m_commentEnd = QRegularExpression("\\*/");

    m_rules.append({QRegularExpression("\\b[\\w-]+(?=\\s*\\{)"), m_selectorFormat});
    m_rules.append({QRegularExpression("\\b[\\w-]+(?=\\s*:)"), m_propertyFormat});
    m_rules.append({QRegularExpression("#[0-9a-fA-F]{3,8}\\b"), m_hexColorFormat});
    m_rules.append({QRegularExpression("\\brgb[a]?\\([^)]+\\)"), m_hexColorFormat});
    m_rules.append({QRegularExpression("\\b\\d+\\.?\\d*(px|pt|em|%)?"), m_numberFormat});
    m_rules.append({QRegularExpression("[{};:]"), m_punctuationFormat});
}

void QSSHighlighter::highlightBlock(const QString &text) {
    if (text.trimmed().startsWith("/*") && text.trimmed().endsWith("*/")) {
        setFormat(0, text.length(), m_commentFormat);
        return;
    }

    int commentIdx = text.indexOf(m_commentStart);
    if (commentIdx >= 0) {
        int endIdx = text.indexOf(m_commentEnd, commentIdx + 2);
        if (endIdx >= 0) {
            setFormat(commentIdx, endIdx - commentIdx + 2, m_commentFormat);
            QString remaining = text.mid(endIdx + 2);
            for (const auto &rule : m_rules) {
                QRegularExpressionMatchIterator it = rule.pattern.globalMatch(remaining);
                while (it.hasNext()) {
                    QRegularExpressionMatch match = it.next();
                    setFormat(endIdx + 2 + match.capturedStart(), match.capturedLength(), rule.format);
                }
            }
        } else {
            setFormat(commentIdx, text.length() - commentIdx, m_commentFormat);
        }
        return;
    }

    for (const auto &rule : m_rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

/* ---- LineNumberArea ---- */

LineNumberArea::LineNumberArea(EditorHelper *editor)
    : QWidget(editor), m_editor(editor) {}

QSize LineNumberArea::sizeHint() const {
    int digits = qMax(2, (int)m_editor->blockCount());
    int width = m_editor->fontMetrics().horizontalAdvance('9') * (digits + 1) + 10;
    return QSize(width, 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), QColor("#1e1e1e"));

    QTextBlock block = m_editor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(m_editor->blockBoundingGeometry(block).translated(m_editor->contentOffset()).top());
    int bottom = top + qRound(m_editor->blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor("#6c7086"));
            painter.drawText(0, top, width() - 4, m_editor->fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(m_editor->blockBoundingRect(block).height());
        ++blockNumber;
    }
}

/* ---- QSSEditorWidget ---- */

QSSEditorWidget::QSSEditorWidget(QWidget *parent)
    : QWidget(parent) {
    setupUI();
    setupShortcuts();
}

void QSSEditorWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(4);

    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search...");
    connect(m_searchInput, &QLineEdit::returnPressed, this, &QSSEditorWidget::onSearch);

    m_replaceInput = new QLineEdit(this);
    m_replaceInput->setPlaceholderText("Replace...");
    connect(m_replaceInput, &QLineEdit::returnPressed, this, &QSSEditorWidget::onReplace);

    m_searchBtn = new QPushButton("Find", this);
    m_searchBtn->setFixedWidth(50);
    connect(m_searchBtn, &QPushButton::clicked, this, &QSSEditorWidget::onSearch);

    m_replaceBtn = new QPushButton("Rpl", this);
    m_replaceBtn->setFixedWidth(40);
    connect(m_replaceBtn, &QPushButton::clicked, this, &QSSEditorWidget::onReplace);

    m_replaceAllBtn = new QPushButton("All", this);
    m_replaceAllBtn->setFixedWidth(40);
    connect(m_replaceAllBtn, &QPushButton::clicked, [this]() {
        QString text = m_editor->toPlainText();
        QString search = m_searchInput->text();
        QString replace = m_replaceInput->text();
        if (!search.isEmpty()) {
            text.replace(search, replace);
            m_editor->setPlainText(text);
            m_statusLabel->setText("Replace all done");
        }
    });

    searchLayout->addWidget(m_searchInput);
    searchLayout->addWidget(m_replaceInput);
    searchLayout->addWidget(m_searchBtn);
    searchLayout->addWidget(m_replaceBtn);
    searchLayout->addWidget(m_replaceAllBtn);

    layout->addLayout(searchLayout);

    m_editor = new EditorHelper(this);
    m_editor->setTabStopDistance(m_editor->fontMetrics().horizontalAdvance(' ') * 4);
    m_editor->setStyleSheet(
        "QPlainTextEdit { background-color: #1e1e1e; color: #d4d4d4; "
        "border: 1px solid #3c3c3c; font-family: 'Consolas', 'Courier New', monospace; "
        "font-size: 12px; }");
    m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    m_highlighter = new QSSHighlighter(m_editor->document());

    m_lineNumberArea = new LineNumberArea(m_editor);

    connect(m_editor, &QPlainTextEdit::blockCountChanged, this, &QSSEditorWidget::updateLineNumberAreaWidth);
    connect(m_editor, &QPlainTextEdit::updateRequest, this, &QSSEditorWidget::updateLineNumberArea);
    connect(m_editor, &QPlainTextEdit::cursorPositionChanged, [this]() {
        QTextBlock block = m_editor->textCursor().block();
        int line = block.blockNumber() + 1;
        int col = m_editor->textCursor().columnNumber() + 1;
        m_statusLabel->setText(QString("Line: %1  Col: %2").arg(line).arg(col));
    });

    updateLineNumberAreaWidth(0);
    layout->addWidget(m_editor);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    m_applyBtn = new QPushButton("Apply (Ctrl+S)", this);
    m_applyBtn->setStyleSheet(
        "QPushButton { background-color: #0078d4; color: white; "
        "border: none; padding: 6px 16px; border-radius: 3px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1a8ad4; }");
    connect(m_applyBtn, &QPushButton::clicked, this, &QSSEditorWidget::onApply);

    m_statusLabel = new QLabel("Line: 1  Col: 1", this);
    m_statusLabel->setStyleSheet("color: #6c7086; font-size: 11px;");

    bottomLayout->addWidget(m_statusLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_applyBtn);
    layout->addLayout(bottomLayout);
}

void QSSEditorWidget::setupShortcuts() {
    QShortcut *applyShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(applyShortcut, &QShortcut::activated, this, &QSSEditorWidget::onApply);

    QShortcut *findShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(findShortcut, &QShortcut::activated, [this]() {
        m_searchInput->setFocus();
        m_searchInput->selectAll();
    });

    QShortcut *replaceShortcut = new QShortcut(QKeySequence("Ctrl+H"), this);
    connect(replaceShortcut, &QShortcut::activated, [this]() {
        m_replaceInput->setFocus();
        m_replaceInput->selectAll();
    });
}

void QSSEditorWidget::updateLineNumberAreaWidth(int) {
    int digits = qMax(2, (int)m_editor->blockCount());
    int width = m_editor->fontMetrics().horizontalAdvance('9') * (digits + 1) + 10;
    m_editor->setViewportMargins(width, 0, 0, 0);
}

void QSSEditorWidget::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(m_editor->viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void QSSEditorWidget::setStyleSheetText(const QString &text) {
    m_editor->setPlainText(text);
}

QString QSSEditorWidget::styleSheetText() const {
    return m_editor->toPlainText();
}

void QSSEditorWidget::clear() {
    m_editor->clear();
}

void QSSEditorWidget::onApply() {
    QString qss = m_editor->toPlainText();
    m_statusLabel->setText("Applied " + QString::number(m_editor->blockCount()) + " lines");
    emit applyRequested(qss);
}

void QSSEditorWidget::onSearch() {
    QString search = m_searchInput->text();
    if (search.isEmpty()) return;

    bool found = m_editor->find(search);
    if (!found) {
        m_editor->moveCursor(QTextCursor::Start);
        found = m_editor->find(search);
        if (!found) {
            m_statusLabel->setText("Not found");
            return;
        }
    }
    m_statusLabel->setText("Found: " + search);
}

void QSSEditorWidget::onReplace() {
    QString replace = m_replaceInput->text();
    if (m_editor->textCursor().hasSelection()) {
        m_editor->textCursor().insertText(replace);
        m_editor->find(m_searchInput->text());
        m_statusLabel->setText("Replaced");
    } else {
        onSearch();
    }
}

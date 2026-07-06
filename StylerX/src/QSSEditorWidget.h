#pragma once

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class QSSHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit QSSHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightRule> m_rules;

    QRegularExpression m_commentStart;
    QRegularExpression m_commentEnd;
    QTextCharFormat m_selectorFormat;
    QTextCharFormat m_propertyFormat;
    QTextCharFormat m_valueFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_punctuationFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_hexColorFormat;
};

class EditorHelper : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit EditorHelper(QWidget *parent = nullptr) : QPlainTextEdit(parent) {}
    using QPlainTextEdit::firstVisibleBlock;
    using QPlainTextEdit::blockBoundingGeometry;
    using QPlainTextEdit::contentOffset;
    using QPlainTextEdit::blockBoundingRect;
    using QAbstractScrollArea::setViewportMargins;
};

class LineNumberArea : public QWidget {
    Q_OBJECT
public:
    explicit LineNumberArea(EditorHelper *editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    EditorHelper *m_editor;
};

class QSSEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit QSSEditorWidget(QWidget *parent = nullptr);

    void setStyleSheetText(const QString &text);
    QString styleSheetText() const;
    void clear();

signals:
    void applyRequested(const QString &styleSheet);

private slots:
    void onApply();
    void onSearch();
    void onReplace();
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    void setupUI();
    void setupShortcuts();

    EditorHelper *m_editor;
    LineNumberArea *m_lineNumberArea;
    QLineEdit *m_searchInput;
    QLineEdit *m_replaceInput;
    QPushButton *m_searchBtn;
    QPushButton *m_replaceBtn;
    QPushButton *m_replaceAllBtn;
    QPushButton *m_applyBtn;
    QLabel *m_statusLabel;
    QSSHighlighter *m_highlighter;
};

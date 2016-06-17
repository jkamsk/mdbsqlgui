#ifndef SYNTAX_H
#define SYNTAX_H

#include <QSyntaxHighlighter>
#include <QTextDocument>

class Syntax : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Syntax(QTextDocument *parent = 0);
protected:
    void highlightBlock(const QString &text);
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
    QRegExp braceStart; QRegExp braceEnd;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat braceFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat singleQuotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat braceFormater;
    QTextCharFormat numberFormat;
    QTextCharFormat showFieldFormat;
    QTextCharFormat htmlNumberFormat;
    QTextCharFormat dollarNumberFormat;
    QTextCharFormat stringFormat;
};

#endif // SYNTAX_H

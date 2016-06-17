#include "syntax.h"

#define keywordFormatColorString            "000080"
#define classFormatColorString              "FF0000"
#define singleLineCommentFormatColorString  "C0C0C0"
#define quotationFormatColorString          "008000"
#define functionFormatColorString           "0000FF"
#define showFieldFormatColorString          "808000"
#define htmlNumberFormatColorString         "0000FF"
#define dollarNumberFormatColorString       "800000"

Syntax::Syntax(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QByteArray tempByteArray;
    quint8 r,g,b;

    HighlightingRule rule;

    tempByteArray.clear();
    tempByteArray.append(keywordFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor keywordFormatColor(r,g,b);
    keywordFormat.setForeground(keywordFormatColor);
    keywordFormat.setFontWeight(QFont::Bold);
    keywordFormat.setFontCapitalization(QFont::AllUppercase);
    QStringList keywordPatterns;
    keywordPatterns
                    << "\\bbigint\\b"         << "\\bbinary\\b"          << "\\bbit\\b"
                    << "\\bblob\\b"           << "\\bbool\\b"            << "\\bboolean\\b"       << "\\bchar\\b"
                    << "\\bdate\\b"           << "\\bdatetime\\b"        << "\\bdecimal\\b"       << "\\bdouble\\b"
                    << "\\benum\\b"           << "\\bfloat\\b"           << "\\bint\\b"           << "\\blongblob\\b"
                    << "\\bfloat\\b"          << "\\bint\\b"             << "\\blongblob\\b"      << "\\blongtext\\b"
                    << "\\bmediumblob\\b"     << "\\bmediumint\\b"       << "\\bmediumtext\\b"
                    << "\\bnumeric\\b"        << "\\breal\\b"            << "\\bset\\b"           << "\\bsmallint\\b"
                    << "\\btext\\b"           << "\\btime\\b"            << "\\btimestamp\\b"     << "\\btinyblob\\b"
                    << "\\btinyint\\b"        << "\\btinytext\\b"        << "\\bvarbinary\\b"
                    << "\\bselect\\b"         << "\\bfrom\\b"            << "\\bwhere\\b"         << "\\border\\b"
                    << "\\bvarchar\\b"        << "\\byear\\b"
    << "\\bBIGINT\\b"         << "\\bbinary\\b"          << "\\bbit\\b"
    << "\\bBLOB\\b"           << "\\bbool\\b"            << "\\bboolean\\b"       << "\\bchar\\b"
    << "\\bDATE\\b"           << "\\bdatetime\\b"        << "\\bdecimal\\b"       << "\\bdouble\\b"
    << "\\bENUM\\b"           << "\\bfloat\\b"           << "\\bint\\b"           << "\\blongblob\\b"
    << "\\bFLOAT\\b"          << "\\bint\\b"             << "\\blongblob\\b"      << "\\blongtext\\b"
    << "\\bMEDIUMBLOB\\b"     << "\\bmediumint\\b"       << "\\bmediumtext\\b"
    << "\\bNUMERIC\\b"        << "\\breal\\b"            << "\\bset\\b"           << "\\bsmallint\\b"
    << "\\bTEXT\\b"           << "\\btime\\b"            << "\\btimestamp\\b"     << "\\btinyblob\\b"
    << "\\bTINYINT\\b"        << "\\btinytext\\b"        << "\\bvarbinary\\b"
    << "\\bSELECT\\b"         << "\\bFROM\\b"            << "\\bWHERE\\b"         << "\\bORDER\\b" << "\\bINSERT\\b"<< "\\bUPDATE\\b"<< "\\bDELETE\\b"
    << "\\bVARCHAR\\b"        << "\\byear\\b"<< "\\bVALUES\\b"<< "\\bINTO\\b"<< "\\bSET\\b"
       << "\\bDROP\\b" << "\\bTABLE\\b" << "\\bIF\\b" << "\\bEXISTS\\b" << "\\bCREATE\\b" << "\\bSCHEMA\\b" << "\\bDATABASE\\b" << "\\bUSE\\b"
       << "\\bNOT\\b"<< "\\bNULL\\b"<< "\\bDEFAULT\\b"  << "\\bENGINE\\b"<< "\\bCHARSET\\b" ;

    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        rule.format.setFontCapitalization(QFont::AllUppercase);
        highlightingRules.append(rule);
    }

    tempByteArray.clear();
    tempByteArray.append(classFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor classFormatColor(r,g,b);
    classFormat.setForeground(classFormatColor);
    classFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("^\\$\\$[^\n]*");
    rule.format = classFormat;
    highlightingRules.append(rule);

    tempByteArray.clear();
    tempByteArray.append(singleLineCommentFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor singleLineCommentFormatColor(r,g,b);
    singleLineCommentFormat.setForeground(singleLineCommentFormatColor);
    rule.pattern = QRegExp("^#[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    tempByteArray.clear();
    tempByteArray.append(quotationFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor quotationFormatColor(r,g,b);
    quotationFormat.setForeground(quotationFormatColor);
    rule.pattern = QRegExp("<[A-Za-z0-9\\s/='%]+>");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    tempByteArray.clear();
    tempByteArray.append(functionFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor functionFormatColor(r,g,b);
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(functionFormatColor);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    tempByteArray.clear();
    tempByteArray.append(showFieldFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor showFieldFormatColor(r,g,b);
    showFieldFormat.setForeground(showFieldFormatColor);
    rule.pattern = QRegExp("\\{[A-Za-z0-9\\s/='']+\\}");
    rule.format = showFieldFormat;
    highlightingRules.append(rule);

    tempByteArray.clear();
    tempByteArray.append(showFieldFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor ashowFieldFormatColor(r,g,b);
    showFieldFormat.setForeground(ashowFieldFormatColor);
    rule.pattern = QRegExp("\\%[A-Za-z0-9_\\s/='']+\\%");
    rule.format = showFieldFormat;
    highlightingRules.append(rule);

    tempByteArray.clear();
    tempByteArray.append(showFieldFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor ashow1FieldFormatColor(r,g,b);
    showFieldFormat.setForeground(ashow1FieldFormatColor);
    rule.pattern = QRegExp("\\`[A-Za-z0-9_\\s/='']+\\`");
    rule.format = showFieldFormat;
    highlightingRules.append(rule);

    tempByteArray.clear();
    tempByteArray.append(htmlNumberFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor htmlNumberFormatColor(r,g,b);
    htmlNumberFormat.setForeground(htmlNumberFormatColor);
    rule.pattern = QRegExp("&[A-Za-z0-9\\s/=''#]+;");
    rule.format = htmlNumberFormat;
    highlightingRules.append(rule);

    tempByteArray.clear();
    tempByteArray.append(dollarNumberFormatColorString);
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor dollarNumberFormatColor(r,g,b);
    dollarNumberFormat.setForeground(dollarNumberFormatColor);
    rule.pattern = QRegExp("\\s\\$[A-Za-z]");
    rule.format = dollarNumberFormat;
    highlightingRules.append(rule);

    tempByteArray.clear();
    tempByteArray.append("008000");
    tempByteArray = QByteArray::fromHex(tempByteArray);
    r = tempByteArray.at(0);
    g = tempByteArray.at(1);
    b = tempByteArray.at(2);
    QColor stringFormatColor(r,g,b);
    stringFormat.setForeground(stringFormatColor);
    rule.pattern = QRegExp("'[A-Za-z0-9%\\s\\\\]+'");
    rule.format = stringFormat;
    highlightingRules.append(rule);
}
void Syntax::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}




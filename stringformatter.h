#ifndef STRINGFORMATTER_H
#define STRINGFORMATTER_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QProgressBar>
#include "apimanager.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
class QPushButton;
class QLabel;
class QProgressBar;
class QSplitter;
class QGroupBox;
QT_END_NAMESPACE

class StringFormatter : public QWidget
{
    Q_OBJECT

public:
    explicit StringFormatter(QWidget *parent = nullptr);

private slots:
    // WHERE条件格式化按钮点击事件
    void onWhereFormatClicked();
    
    // VALUES插入格式化按钮点击事件
    void onValuesFormatClicked();
    
    // 清空按钮点击事件
    void onClearClicked();
    
    // 复制结果按钮点击事件
    void onCopyResultClicked();

private:
    // UI组件
    QTextEdit *m_inputTextEdit;
    QTextEdit *m_outputTextEdit;
    QPushButton *m_whereFormatButton;
    QPushButton *m_valuesFormatButton;
    QPushButton *m_clearButton;
    QPushButton *m_copyResultButton;
    QLabel *m_statusLabel;
    QSplitter *m_splitter;
    QGroupBox *m_inputGroup;
    QGroupBox *m_outputGroup;
    QWidget *m_controlWidget;
    
    // 初始化UI
    void setupUI();
    
    // 设置样式
    void setupStyles();
    
    // 验证输入
    bool validateInput();
    
    // 格式化为WHERE条件
    QString formatAsWhereCondition(const QStringList &values);
    
    // 格式化为VALUES插入语句
    QString formatAsValuesInsert(const QStringList &values);
    
    // 解析输入文本为字符串列表
    QStringList parseInputText(const QString &input);
    
    // 更新状态信息
    void updateStatus(const QString &message, const QString &color = "#2c3e50");
};

#endif // STRINGFORMATTER_H
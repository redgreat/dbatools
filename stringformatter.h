#ifndef STRINGFORMATTER_H
#define STRINGFORMATTER_H

#include <QWidget>
#include <QTextEdit>
#include <QComboBox>
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
class QComboBox;
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
    explicit StringFormatter(ApiManager *apiManager, QWidget *parent = nullptr);

private slots:
    // 格式化按钮点击事件
    void onFormatClicked();
    
    // 清空按钮点击事件
    void onClearClicked();
    
    // 复制结果按钮点击事件
    void onCopyResultClicked();
    
    // 格式化类型改变事件
    void onFormatTypeChanged();
    
    // 处理格式化结果
    void onFormatResult(bool success, const QString &result, const QString &error);
    
    // 处理网络错误
    void onNetworkError(const QString &error);

private:
    // UI组件
    QTextEdit *m_inputTextEdit;
    QTextEdit *m_outputTextEdit;
    QComboBox *m_formatTypeCombo;
    QPushButton *m_formatButton;
    QPushButton *m_clearButton;
    QPushButton *m_copyResultButton;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QSplitter *m_splitter;
    QGroupBox *m_inputGroup;
    QGroupBox *m_outputGroup;
    QGroupBox *m_controlGroup;
    
    // API管理器
    ApiManager *m_apiManager;
    
    // 初始化UI
    void setupUI();
    
    // 设置样式
    void setupStyles();
    
    // 初始化格式化类型
    void initializeFormatTypes();
    
    // 验证输入
    bool validateInput();
    
    // 设置处理状态
    void setProcessingState(bool isProcessing);
    
    // 更新状态信息
    void updateStatus(const QString &message, const QString &color = "#2c3e50");
};

#endif // STRINGFORMATTER_H
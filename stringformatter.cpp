#include "stringformatter.h"
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QTextCursor>
#include <QScrollBar>

/**
 * 字符串格式化工具构造函数
 * 初始化UI组件和API连接
 */
StringFormatter::StringFormatter(ApiManager *apiManager, QWidget *parent)
    : QWidget(parent)
    , m_inputTextEdit(nullptr)
    , m_outputTextEdit(nullptr)
    , m_formatTypeCombo(nullptr)
    , m_formatButton(nullptr)
    , m_clearButton(nullptr)
    , m_copyResultButton(nullptr)
    , m_statusLabel(nullptr)
    , m_progressBar(nullptr)
    , m_splitter(nullptr)
    , m_inputGroup(nullptr)
    , m_outputGroup(nullptr)
    , m_controlGroup(nullptr)
    , m_apiManager(apiManager)
{
    setupUI();
    setupStyles();
    initializeFormatTypes();
    
    // 连接API管理器信号
    connect(m_apiManager, &ApiManager::formatStringResult,
            this, &StringFormatter::onFormatResult);
    connect(m_apiManager, &ApiManager::networkError,
            this, &StringFormatter::onNetworkError);
}

/**
 * 初始化UI界面
 */
void StringFormatter::setupUI()
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建控制面板组
    m_controlGroup = new QGroupBox("格式化控制", this);
    QHBoxLayout *controlLayout = new QHBoxLayout(m_controlGroup);
    
    // 格式化类型选择
    QLabel *typeLabel = new QLabel("格式化类型:", this);
    m_formatTypeCombo = new QComboBox(this);
    m_formatTypeCombo->setMinimumWidth(150);
    
    // 按钮
    m_formatButton = new QPushButton("格式化", this);
    m_clearButton = new QPushButton("清空", this);
    m_copyResultButton = new QPushButton("复制结果", this);
    
    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 0);
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumHeight(20);
    
    controlLayout->addWidget(typeLabel);
    controlLayout->addWidget(m_formatTypeCombo);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(m_formatButton);
    controlLayout->addWidget(m_clearButton);
    controlLayout->addWidget(m_copyResultButton);
    controlLayout->addStretch();
    controlLayout->addWidget(m_progressBar);
    
    mainLayout->addWidget(m_controlGroup);
    
    // 创建分割器
    m_splitter = new QSplitter(Qt::Horizontal, this);
    
    // 输入区域
    m_inputGroup = new QGroupBox("输入文本", this);
    QVBoxLayout *inputLayout = new QVBoxLayout(m_inputGroup);
    m_inputTextEdit = new QTextEdit(this);
    m_inputTextEdit->setPlaceholderText("请输入需要格式化的文本...");
    inputLayout->addWidget(m_inputTextEdit);
    
    // 输出区域
    m_outputGroup = new QGroupBox("格式化结果", this);
    QVBoxLayout *outputLayout = new QVBoxLayout(m_outputGroup);
    m_outputTextEdit = new QTextEdit(this);
    m_outputTextEdit->setPlaceholderText("格式化结果将显示在这里...");
    m_outputTextEdit->setReadOnly(true);
    outputLayout->addWidget(m_outputTextEdit);
    
    // 添加到分割器
    m_splitter->addWidget(m_inputGroup);
    m_splitter->addWidget(m_outputGroup);
    m_splitter->setSizes({400, 400});
    
    mainLayout->addWidget(m_splitter);
    
    // 状态标签
    m_statusLabel = new QLabel("就绪", this);
    mainLayout->addWidget(m_statusLabel);
    
    // 连接信号槽
    connect(m_formatButton, &QPushButton::clicked,
            this, &StringFormatter::onFormatClicked);
    connect(m_clearButton, &QPushButton::clicked,
            this, &StringFormatter::onClearClicked);
    connect(m_copyResultButton, &QPushButton::clicked,
            this, &StringFormatter::onCopyResultClicked);
    connect(m_formatTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StringFormatter::onFormatTypeChanged);
}

/**
 * 设置界面样式
 */
void StringFormatter::setupStyles()
{
    // 设置组框样式
    QString groupBoxStyle = "QGroupBox { "
                           "font-weight: bold; "
                           "border: 2px solid #cccccc; "
                           "border-radius: 5px; "
                           "margin-top: 10px; "
                           "padding-top: 10px; "
                           "} "
                           "QGroupBox::title { "
                           "subcontrol-origin: margin; "
                           "left: 10px; "
                           "padding: 0 5px 0 5px; "
                           "}";
    
    m_controlGroup->setStyleSheet(groupBoxStyle);
    m_inputGroup->setStyleSheet(groupBoxStyle);
    m_outputGroup->setStyleSheet(groupBoxStyle);
    
    // 设置文本编辑器样式
    QString textEditStyle = "QTextEdit { "
                           "border: 1px solid #cccccc; "
                           "border-radius: 4px; "
                           "padding: 8px; "
                           "font-family: 'Consolas', 'Monaco', monospace; "
                           "font-size: 12px; "
                           "line-height: 1.4; "
                           "} "
                           "QTextEdit:focus { "
                           "border-color: #3498db; "
                           "}";
    
    m_inputTextEdit->setStyleSheet(textEditStyle);
    m_outputTextEdit->setStyleSheet(textEditStyle + 
                                   "QTextEdit { background-color: #f8f9fa; }");
    
    // 设置按钮样式
    QString primaryButtonStyle = "QPushButton { "
                                "background-color: #3498db; "
                                "color: white; "
                                "border: none; "
                                "padding: 8px 16px; "
                                "border-radius: 4px; "
                                "font-weight: bold; "
                                "} "
                                "QPushButton:hover { "
                                "background-color: #2980b9; "
                                "} "
                                "QPushButton:pressed { "
                                "background-color: #21618c; "
                                "} "
                                "QPushButton:disabled { "
                                "background-color: #bdc3c7; "
                                "}";
    
    QString secondaryButtonStyle = "QPushButton { "
                                  "background-color: #95a5a6; "
                                  "color: white; "
                                  "border: none; "
                                  "padding: 8px 16px; "
                                  "border-radius: 4px; "
                                  "} "
                                  "QPushButton:hover { "
                                  "background-color: #7f8c8d; "
                                  "}";
    
    m_formatButton->setStyleSheet(primaryButtonStyle);
    m_clearButton->setStyleSheet(secondaryButtonStyle);
    m_copyResultButton->setStyleSheet(secondaryButtonStyle);
    
    // 设置下拉框样式
    QString comboBoxStyle = "QComboBox { "
                           "border: 1px solid #cccccc; "
                           "border-radius: 4px; "
                           "padding: 6px 12px; "
                           "background-color: white; "
                           "} "
                           "QComboBox:focus { "
                           "border-color: #3498db; "
                           "} "
                           "QComboBox::drop-down { "
                           "border: none; "
                           "} "
                           "QComboBox::down-arrow { "
                           "image: none; "
                           "border-left: 5px solid transparent; "
                           "border-right: 5px solid transparent; "
                           "border-top: 5px solid #666; "
                           "}";
    
    m_formatTypeCombo->setStyleSheet(comboBoxStyle);
    
    // 设置状态标签样式
    m_statusLabel->setStyleSheet("QLabel { "
                                "color: #2c3e50; "
                                "font-size: 12px; "
                                "padding: 4px; "
                                "}");
}

/**
 * 初始化格式化类型
 */
void StringFormatter::initializeFormatTypes()
{
    m_formatTypeCombo->addItem("JSON格式化", "json");
    m_formatTypeCombo->addItem("XML格式化", "xml");
    m_formatTypeCombo->addItem("SQL格式化", "sql");
    m_formatTypeCombo->addItem("HTML格式化", "html");
    m_formatTypeCombo->addItem("CSS格式化", "css");
    m_formatTypeCombo->addItem("JavaScript格式化", "javascript");
    m_formatTypeCombo->addItem("Base64编码", "base64_encode");
    m_formatTypeCombo->addItem("Base64解码", "base64_decode");
    m_formatTypeCombo->addItem("URL编码", "url_encode");
    m_formatTypeCombo->addItem("URL解码", "url_decode");
    
    // 设置默认选择
    m_formatTypeCombo->setCurrentIndex(0);
    onFormatTypeChanged();
}

/**
 * 格式化按钮点击事件处理
 */
void StringFormatter::onFormatClicked()
{
    if (!validateInput()) {
        return;
    }
    
    setProcessingState(true);
    
    QString input = m_inputTextEdit->toPlainText();
    QString formatType = m_formatTypeCombo->currentData().toString();
    
    updateStatus("正在格式化...", "#3498db");
    m_apiManager->formatString(input, formatType);
}

/**
 * 清空按钮点击事件处理
 */
void StringFormatter::onClearClicked()
{
    m_inputTextEdit->clear();
    m_outputTextEdit->clear();
    updateStatus("已清空", "#27ae60");
    m_inputTextEdit->setFocus();
}

/**
 * 复制结果按钮点击事件处理
 */
void StringFormatter::onCopyResultClicked()
{
    QString result = m_outputTextEdit->toPlainText();
    if (result.isEmpty()) {
        updateStatus("没有可复制的内容", "#e74c3c");
        return;
    }
    
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(result);
    updateStatus("结果已复制到剪贴板", "#27ae60");
}

/**
 * 格式化类型改变事件处理
 */
void StringFormatter::onFormatTypeChanged()
{
    QString formatType = m_formatTypeCombo->currentData().toString();
    QString placeholder;
    
    if (formatType == "json") {
        placeholder = "请输入JSON字符串，例如: {\"name\":\"value\"}";
    } else if (formatType == "xml") {
        placeholder = "请输入XML字符串，例如: <root><item>value</item></root>";
    } else if (formatType == "sql") {
        placeholder = "请输入SQL语句，例如: SELECT * FROM table WHERE id = 1";
    } else if (formatType == "base64_encode") {
        placeholder = "请输入要编码的文本";
    } else if (formatType == "base64_decode") {
        placeholder = "请输入要解码的Base64字符串";
    } else if (formatType == "url_encode") {
        placeholder = "请输入要编码的URL";
    } else if (formatType == "url_decode") {
        placeholder = "请输入要解码的URL";
    } else {
        placeholder = "请输入需要格式化的文本...";
    }
    
    m_inputTextEdit->setPlaceholderText(placeholder);
    updateStatus("格式化类型已切换到: " + m_formatTypeCombo->currentText());
}

/**
 * 处理格式化结果
 */
void StringFormatter::onFormatResult(bool success, const QString &result, const QString &error)
{
    setProcessingState(false);
    
    if (success) {
        m_outputTextEdit->setPlainText(result);
        updateStatus("格式化完成", "#27ae60");
        
        // 滚动到顶部
        QTextCursor cursor = m_outputTextEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        m_outputTextEdit->setTextCursor(cursor);
    } else {
        m_outputTextEdit->setPlainText("格式化失败: " + error);
        updateStatus("格式化失败: " + error, "#e74c3c");
    }
}

/**
 * 处理网络错误
 */
void StringFormatter::onNetworkError(const QString &error)
{
    setProcessingState(false);
    m_outputTextEdit->setPlainText("网络错误: " + error);
    updateStatus("网络错误: " + error, "#e74c3c");
}

/**
 * 验证用户输入
 */
bool StringFormatter::validateInput()
{
    if (m_inputTextEdit->toPlainText().trimmed().isEmpty()) {
        updateStatus("请输入要格式化的文本", "#e74c3c");
        m_inputTextEdit->setFocus();
        return false;
    }
    
    return true;
}

/**
 * 设置处理状态
 */
void StringFormatter::setProcessingState(bool isProcessing)
{
    m_formatButton->setEnabled(!isProcessing);
    m_formatTypeCombo->setEnabled(!isProcessing);
    m_progressBar->setVisible(isProcessing);
}

/**
 * 更新状态信息
 */
void StringFormatter::updateStatus(const QString &message, const QString &color)
{
    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet(QString("QLabel { "
                                        "color: %1; "
                                        "font-size: 12px; "
                                        "padding: 4px; "
                                        "}").arg(color));
}
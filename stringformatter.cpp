#include "stringformatter.h"
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QTextCursor>
#include <QScrollBar>

/**
 * 字符串格式化工具构造函数
 * 初始化UI组件和API连接
 */
StringFormatter::StringFormatter(QWidget *parent)
    : QWidget(parent)
    , m_inputTextEdit(nullptr)
    , m_outputTextEdit(nullptr)
    , m_whereFormatButton(nullptr)
    , m_valuesFormatButton(nullptr)
    , m_clearButton(nullptr)
    , m_copyResultButton(nullptr)
    , m_statusLabel(nullptr)
    , m_splitter(nullptr)
    , m_inputGroup(nullptr)
    , m_outputGroup(nullptr)
    , m_controlWidget(nullptr)
{
    setupUI();
    setupStyles();
}

/**
 * 初始化UI界面
 */
void StringFormatter::setupUI()
{
    setWindowTitle("查询条件格式化工具");
    resize(1000, 700);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 10);
    mainLayout->setSpacing(5);
    
    // 创建控制面板
    m_controlWidget = new QWidget(this);
    QHBoxLayout *controlLayout = new QHBoxLayout(m_controlWidget);
    controlLayout->setContentsMargins(5, 5, 5, 5);
    m_controlWidget->setMaximumHeight(50);
    
    // 按钮
    m_whereFormatButton = new QPushButton("WHERE条件格式化", this);
    m_valuesFormatButton = new QPushButton("VALUES插入格式化", this);
    m_clearButton = new QPushButton("清空", this);
    m_copyResultButton = new QPushButton("复制结果", this);
    
    // 状态标签
    m_statusLabel = new QLabel("就绪", this);
    
    controlLayout->addWidget(m_whereFormatButton);
    controlLayout->addWidget(m_valuesFormatButton);
    controlLayout->addStretch();
    controlLayout->addWidget(m_clearButton);
    controlLayout->addWidget(m_copyResultButton);
    controlLayout->addWidget(m_statusLabel);
    
    // 创建分割器
    m_splitter = new QSplitter(Qt::Horizontal, this);
    
    // 输入区域
    m_inputGroup = new QGroupBox("输入文本", this);
    QVBoxLayout *inputLayout = new QVBoxLayout(m_inputGroup);
    m_inputTextEdit = new QTextEdit(this);
    m_inputTextEdit->setPlaceholderText("请输入需要格式化的字符串数据，每行一个...");
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
    m_splitter->setSizes({500, 500});
    
    // 添加到主布局
    mainLayout->addWidget(m_controlWidget);
    mainLayout->addWidget(m_splitter);
    
    // 连接信号槽
    connect(m_whereFormatButton, &QPushButton::clicked, this, &StringFormatter::onWhereFormatClicked);
    connect(m_valuesFormatButton, &QPushButton::clicked, this, &StringFormatter::onValuesFormatClicked);
    connect(m_clearButton, &QPushButton::clicked,
            this, &StringFormatter::onClearClicked);
    connect(m_copyResultButton, &QPushButton::clicked,
            this, &StringFormatter::onCopyResultClicked);
}

/**
 * 设置界面样式
 */
void StringFormatter::setupStyles()
{
    // 设置组框样式 - 暗黑主题
    QString groupBoxStyle = "QGroupBox { "
                           "font-weight: bold; "
                           "color: #ffffff; "
                           "border: 2px solid #555555; "
                           "border-radius: 5px; "
                           "margin-top: 10px; "
                           "padding-top: 10px; "
                           "background-color: #2b2b2b; "
                           "} "
                           "QGroupBox::title { "
                           "subcontrol-origin: margin; "
                           "left: 10px; "
                           "padding: 0 5px 0 5px; "
                           "color: #ffffff; "
                           "}";
    
    // 控制面板不需要特殊样式，使用默认样式
    m_inputGroup->setStyleSheet(groupBoxStyle);
    m_outputGroup->setStyleSheet(groupBoxStyle);
    
    // 设置文本编辑器样式 - 暗黑主题
    QString textEditStyle = "QTextEdit { "
                           "background-color: #1e1e1e; "
                           "color: #ffffff; "
                           "border: 1px solid #555555; "
                           "border-radius: 4px; "
                           "padding: 8px; "
                           "font-family: 'Consolas', 'Monaco', monospace; "
                           "font-size: 12px; "
                           "line-height: 1.4; "
                           "selection-background-color: #3399ff; "
                           "} "
                           "QTextEdit:focus { "
                           "border-color: #0078d4; "
                           "} "
                           "QScrollBar:vertical { "
                           "background-color: #2b2b2b; "
                           "width: 12px; "
                           "border-radius: 6px; "
                           "} "
                           "QScrollBar::handle:vertical { "
                           "background-color: #555555; "
                           "border-radius: 6px; "
                           "min-height: 20px; "
                           "} "
                           "QScrollBar::handle:vertical:hover { "
                           "background-color: #666666; "
                           "}";
    
    m_inputTextEdit->setStyleSheet(textEditStyle);
    m_outputTextEdit->setStyleSheet(textEditStyle);
    
    // 设置按钮样式 - 暗黑主题
    QString primaryButtonStyle = "QPushButton { "
                                "background-color: #0078d4; "
                                "color: white; "
                                "border: none; "
                                "padding: 8px 16px; "
                                "border-radius: 4px; "
                                "font-weight: bold; "
                                "} "
                                "QPushButton:hover { "
                                "background-color: #106ebe; "
                                "} "
                                "QPushButton:pressed { "
                                "background-color: #005a9e; "
                                "} "
                                "QPushButton:disabled { "
                                "background-color: #555555; "
                                "color: #888888; "
                                "}";
    
    QString secondaryButtonStyle = "QPushButton { "
                                  "background-color: #404040; "
                                  "color: white; "
                                  "border: 1px solid #555555; "
                                  "padding: 8px 16px; "
                                  "border-radius: 4px; "
                                  "} "
                                  "QPushButton:hover { "
                                  "background-color: #505050; "
                                  "border-color: #666666; "
                                  "}";
    
    m_whereFormatButton->setStyleSheet(primaryButtonStyle);
    m_valuesFormatButton->setStyleSheet(primaryButtonStyle);
    m_clearButton->setStyleSheet(secondaryButtonStyle);
    m_copyResultButton->setStyleSheet(secondaryButtonStyle);
    
    // 设置下拉框样式 - 暗黑主题
    QString comboBoxStyle = "QComboBox { "
                           "background-color: #1e1e1e; "
                           "color: #ffffff; "
                           "border: 1px solid #555555; "
                           "border-radius: 4px; "
                           "padding: 6px 12px; "
                           "} "
                           "QComboBox:focus { "
                           "border-color: #0078d4; "
                           "} "
                           "QComboBox::drop-down { "
                           "border: none; "
                           "background-color: #2b2b2b; "
                           "} "
                           "QComboBox::down-arrow { "
                           "image: none; "
                           "border-left: 5px solid transparent; "
                           "border-right: 5px solid transparent; "
                           "border-top: 5px solid #ffffff; "
                           "} "
                           "QComboBox QAbstractItemView { "
                           "background-color: #1e1e1e; "
                           "color: #ffffff; "
                           "border: 1px solid #555555; "
                           "selection-background-color: #0078d4; "
                           "selection-color: #ffffff; "
                           "}";
    

    
    // 设置状态标签样式 - 暗黑主题
    m_statusLabel->setStyleSheet("QLabel { "
                                "color: #ffffff; "
                                "font-size: 12px; "
                                "padding: 4px; "
                                "background-color: transparent; "
                                "}");
}



/**
 * WHERE条件格式化按钮点击事件
 */
void StringFormatter::onWhereFormatClicked()
{
    if (!validateInput()) {
        return;
    }
    
    QString inputText = m_inputTextEdit->toPlainText().trimmed();
    QStringList values = parseInputText(inputText);
    
    if (values.isEmpty()) {
        updateStatus("输入为空或格式不正确", "#e74c3c");
        return;
    }
    
    QString result = formatAsWhereCondition(values);
    m_outputTextEdit->setPlainText(result);
    updateStatus(QString("WHERE条件格式化完成，共处理 %1 个值").arg(values.size()), "#27ae60");
}

/**
 * VALUES插入格式化按钮点击事件
 */
void StringFormatter::onValuesFormatClicked()
{
    if (!validateInput()) {
        return;
    }
    
    QString inputText = m_inputTextEdit->toPlainText().trimmed();
    QStringList values = parseInputText(inputText);
    
    if (values.isEmpty()) {
        updateStatus("输入为空或格式不正确", "#e74c3c");
        return;
    }
    
    QString result = formatAsValuesInsert(values);
    m_outputTextEdit->setPlainText(result);
    updateStatus(QString("VALUES插入格式化完成，共处理 %1 个值").arg(values.size()), "#27ae60");
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
 * 格式化为WHERE条件
 */
QString StringFormatter::formatAsWhereCondition(const QStringList &values)
{
    if (values.isEmpty()) {
        return QString();
    }
    
    QStringList quotedValues;
    for (const QString &value : values) {
        quotedValues << QString("'%1'").arg(value.trimmed());
    }
    
    return QString("(%1)").arg(quotedValues.join(", \n "));
}

/**
 * 格式化为VALUES插入语句
 */
QString StringFormatter::formatAsValuesInsert(const QStringList &values)
{
    if (values.isEmpty()) {
        return QString();
    }
    
    QString currentDate = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    QString tableName = QString("tm_strcode%1").arg(currentDate);
    
    QString result;
    result += QString("DROP TABLE IF EXISTS %1;\n").arg(tableName);
    result += QString("CREATE TABLE %1(\n").arg(tableName);
    result += "    Id BIGINT AUTO_INCREMENT PRIMARY KEY,\n";
    result += "    StrCode varchar(100),\n";
    result += "    KEY `1` (StrCode)\n";
    result += ");\n";
    result += QString("INSERT INTO %1(StrCode)\n").arg(tableName);
    result += "VALUES ";
    
    QStringList valueLines;
    for (const QString &value : values) {
        valueLines << QString("('%1')").arg(value.trimmed());
    }
    
    result += valueLines.join(",\n       ");
    result += ";";
    
    return result;
}

/**
 * 解析输入文本为字符串列表
 */
QStringList StringFormatter::parseInputText(const QString &input)
{
    QStringList lines = input.split('\n', Qt::SkipEmptyParts);
    QStringList result;
    
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (!trimmed.isEmpty()) {
            result << trimmed;
        }
    }
    
    return result;
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
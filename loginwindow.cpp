#include "loginwindow.h"
#include "mainwindow.h"
#include <QApplication>
#include <QInputDialog>
#include <QSettings>
#include <QTimer>

/**
 * 登录窗口构造函数
 * 初始化UI组件和API管理器
 */
LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_usernameEdit(nullptr)
    , m_passwordEdit(nullptr)
    , m_loginButton(nullptr)
    , m_serverSettingsButton(nullptr)
    , m_titleLabel(nullptr)
    , m_usernameLabel(nullptr)
    , m_passwordLabel(nullptr)
    , m_statusLabel(nullptr)
    , m_progressBar(nullptr)
    , m_apiManager(new ApiManager(this))
{
    setupUI();
    setupStyles();
    
    // 连接API管理器信号
    connect(m_apiManager, &ApiManager::loginResult,
            this, &LoginWindow::onLoginResult);
    connect(m_apiManager, &ApiManager::networkError,
            this, &LoginWindow::onNetworkError);
    
    // 从设置中加载服务器URL
    QSettings settings;
    QString serverUrl = settings.value("server/url", "http://localhost:8001/api").toString();
    m_apiManager->setBaseUrl(serverUrl);
}

/**
 * 登录窗口析构函数
 */
LoginWindow::~LoginWindow()
{
}

/**
 * 初始化UI界面
 */
void LoginWindow::setupUI()
{
    setWindowTitle("DBA Tools - 登录");
    setFixedSize(400, 300);
    
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // 创建UI组件
    m_titleLabel = new QLabel("DBA Tools", this);
    m_usernameLabel = new QLabel("用户名:", this);
    m_passwordLabel = new QLabel("密码:", this);
    m_usernameEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_loginButton = new QPushButton("登录", this);
    m_serverSettingsButton = new QPushButton("服务器设置", this);
    m_statusLabel = new QLabel("", this);
    m_progressBar = new QProgressBar(this);
    
    // 设置密码输入框为密码模式
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    
    // 设置进度条为不确定模式并隐藏
    m_progressBar->setRange(0, 0);
    m_progressBar->setVisible(false);
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);
    
    // 标题
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addSpacing(20);
    
    // 表单布局
    QGridLayout *formLayout = new QGridLayout();
    formLayout->addWidget(m_usernameLabel, 0, 0);
    formLayout->addWidget(m_usernameEdit, 0, 1);
    formLayout->addWidget(m_passwordLabel, 1, 0);
    formLayout->addWidget(m_passwordEdit, 1, 1);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(10);
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_serverSettingsButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_loginButton);
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addSpacing(10);
    
    // 状态和进度条
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addWidget(m_progressBar);
    
    mainLayout->addStretch();
    
    // 连接信号槽
    connect(m_loginButton, &QPushButton::clicked,
            this, &LoginWindow::onLoginClicked);
    connect(m_serverSettingsButton, &QPushButton::clicked,
            this, &LoginWindow::onServerSettingsClicked);
    
    // 回车键登录
    connect(m_passwordEdit, &QLineEdit::returnPressed,
            this, &LoginWindow::onLoginClicked);
    connect(m_usernameEdit, &QLineEdit::returnPressed,
            this, &LoginWindow::onLoginClicked);
}

/**
 * 设置界面样式
 */
void LoginWindow::setupStyles()
{
    // 设置标题样式
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #2c3e50; }");
    
    // 设置输入框样式
    QString editStyle = "QLineEdit { "
                       "padding: 8px; "
                       "border: 2px solid #bdc3c7; "
                       "border-radius: 4px; "
                       "font-size: 14px; "
                       "} "
                       "QLineEdit:focus { "
                       "border-color: #3498db; "
                       "}";
    
    m_usernameEdit->setStyleSheet(editStyle);
    m_passwordEdit->setStyleSheet(editStyle);
    
    // 设置按钮样式
    QString buttonStyle = "QPushButton { "
                         "background-color: #3498db; "
                         "color: white; "
                         "border: none; "
                         "padding: 10px 20px; "
                         "border-radius: 4px; "
                         "font-size: 14px; "
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
    
    m_loginButton->setStyleSheet(buttonStyle);
    
    QString settingsButtonStyle = "QPushButton { "
                                 "background-color: #95a5a6; "
                                 "color: white; "
                                 "border: none; "
                                 "padding: 8px 16px; "
                                 "border-radius: 4px; "
                                 "font-size: 12px; "
                                 "} "
                                 "QPushButton:hover { "
                                 "background-color: #7f8c8d; "
                                 "}";
    
    m_serverSettingsButton->setStyleSheet(settingsButtonStyle);
    
    // 设置状态标签样式
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("QLabel { color: #e74c3c; font-size: 12px; }");
}

/**
 * 登录按钮点击事件处理
 */
void LoginWindow::onLoginClicked()
{
    if (!validateInput()) {
        return;
    }
    
    setLoginState(true);
    
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    
    m_apiManager->login(username, password);
}

/**
 * 处理登录结果
 */
void LoginWindow::onLoginResult(bool success, const QString &message, const QString &token)
{
    setLoginState(false);
    
    if (success) {
        m_statusLabel->setText("登录成功!");
        m_statusLabel->setStyleSheet("QLabel { color: #27ae60; font-size: 12px; }");
        
        // 保存认证令牌
        QSettings settings;
        settings.setValue("auth/token", token);
        settings.setValue("auth/username", m_usernameEdit->text());
        
        // 延迟一下再打开主窗口
        QTimer::singleShot(500, this, [this]() {
            MainWindow *mainWindow = new MainWindow();
            mainWindow->show();
            this->close();
        });
    } else {
        m_statusLabel->setText(message.isEmpty() ? "登录失败" : message);
        m_statusLabel->setStyleSheet("QLabel { color: #e74c3c; font-size: 12px; }");
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
    }
}

/**
 * 处理网络错误
 */
void LoginWindow::onNetworkError(const QString &error)
{
    setLoginState(false);
    m_statusLabel->setText("网络错误: " + error);
    m_statusLabel->setStyleSheet("QLabel { color: #e74c3c; font-size: 12px; }");
}

/**
 * 服务器设置按钮点击事件
 */
void LoginWindow::onServerSettingsClicked()
{
    QSettings settings;
    QString currentUrl = settings.value("server/url", "http://localhost:8001/api").toString();
    
    bool ok;
    QString newUrl = QInputDialog::getText(this, "服务器设置", 
                                          "请输入服务器API地址:", 
                                          QLineEdit::Normal, 
                                          currentUrl, &ok);
    
    if (ok && !newUrl.isEmpty()) {
        settings.setValue("server/url", newUrl);
        m_apiManager->setBaseUrl(newUrl);
        m_statusLabel->setText("服务器地址已更新");
        m_statusLabel->setStyleSheet("QLabel { color: #27ae60; font-size: 12px; }");
    }
}

/**
 * 验证用户输入
 */
bool LoginWindow::validateInput()
{
    if (m_usernameEdit->text().trimmed().isEmpty()) {
        m_statusLabel->setText("请输入用户名");
        m_usernameEdit->setFocus();
        return false;
    }
    
    if (m_passwordEdit->text().isEmpty()) {
        m_statusLabel->setText("请输入密码");
        m_passwordEdit->setFocus();
        return false;
    }
    
    return true;
}

/**
 * 设置登录状态
 */
void LoginWindow::setLoginState(bool isLogging)
{
    m_loginButton->setEnabled(!isLogging);
    m_usernameEdit->setEnabled(!isLogging);
    m_passwordEdit->setEnabled(!isLogging);
    m_serverSettingsButton->setEnabled(!isLogging);
    m_progressBar->setVisible(isLogging);
    
    if (isLogging) {
        m_statusLabel->setText("正在登录...");
        m_statusLabel->setStyleSheet("QLabel { color: #3498db; font-size: 12px; }");
    }
}
#include "mainwindow.h"
#include "stringformatter.h"
#include "usermanager.h"
#include "rolemanager.h"
#include "loginwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QShowEvent>
#include <QDebug>
#include <iostream>

/**
 * 主窗口构造函数
 * 初始化UI组件和工具页面
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_currentTool(nullptr)
    , m_statusLabel(nullptr)
    , m_exitAction(nullptr)
    , m_aboutAction(nullptr)
    , m_logoutAction(nullptr)
    , m_settingsAction(nullptr)
    , m_addToolAction(nullptr)
    , m_stringFormatterAction(nullptr)
    , m_userManagementAction(nullptr)
    , m_roleManagementAction(nullptr)
    , m_stringFormatter(nullptr)
    , m_userManager(nullptr)
    , m_roleManager(nullptr)
    , m_apiManager(new ApiManager(this))
{
    qDebug() << "[DEBUG] MainWindow constructor called, instance:" << this;
    setupUI();
    createMenus();
    createStatusBar();
    initializeTools();
    setupStyles();
    
    // 默认显示字符串格式化工具
    showTool(m_stringFormatter);
    
    // 从设置中加载服务器URL和认证令牌
    QSettings settings;
    QString serverUrl = settings.value("server/url", "http://localhost:8001/api").toString();
    m_apiManager->setBaseUrl(serverUrl);
    
    // 恢复认证令牌
    QString savedToken = settings.value("auth/token").toString();
    if (!savedToken.isEmpty()) {
        m_apiManager->setAuthToken(savedToken);
    } else {
        qDebug() << "[DEBUG] MainWindow: No saved token found";
    }
    
    // 连接API管理器信号
    connect(m_apiManager, &ApiManager::loginResult,
            this, &MainWindow::onLoginResult);
    connect(m_apiManager, &ApiManager::logoutResult,
            this, &MainWindow::onLogoutResult);
    connect(m_apiManager, &ApiManager::tokenExpired,
            this, &MainWindow::onTokenExpired);
    
    // 显示欢迎信息
    QString username = settings.value("auth/username", "用户").toString();
    m_statusLabel->setText(QString("欢迎, %1!").arg(username));
}

/**
 * 主窗口析构函数
 */
MainWindow::~MainWindow()
{
    qDebug() << "[DEBUG] MainWindow destructor called, instance:" << this;
    // 清理静态引用
    if (LoginWindow::s_mainWindow == this) {
        qDebug() << "[DEBUG] Clearing s_mainWindow static reference";
        LoginWindow::s_mainWindow = nullptr;
    }
}

/**
 * 初始化UI界面
 */
void MainWindow::setupUI()
{
    setWindowTitle("DBA Tools - 工具集合");
    resize(800, 600);
    
    // 中央widget将在showTool方法中设置
    m_currentTool = nullptr;
}

/**
 * 创建菜单栏
 */
void MainWindow::createMenus()
{
    static bool menusCreated = false;
    if (menusCreated) {
        qDebug() << "[DEBUG] 菜单已存在，跳过创建";
        return;
    }
    
    std::cout << "[DEBUG] 开始创建菜单" << std::endl;
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    std::cout << "[DEBUG] 文件菜单创建完成" << std::endl;
    
    m_settingsAction = new QAction("设置(&S)", this);
    m_settingsAction->setShortcut(QKeySequence::Preferences);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);
    fileMenu->addAction(m_settingsAction);
    qDebug() << "设置Action添加完成:" << m_settingsAction;
    
    fileMenu->addSeparator();
    
    m_logoutAction = new QAction("注销(&L)", this);
    connect(m_logoutAction, &QAction::triggered, this, &MainWindow::onLogoutClicked);
    fileMenu->addAction(m_logoutAction);
    qDebug() << "注销Action添加完成:" << m_logoutAction;
    
    m_exitAction = new QAction("退出(&X)", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::onExitClicked);
    fileMenu->addAction(m_exitAction);
    qDebug() << "退出Action添加完成:" << m_exitAction;
    
    menusCreated = true;
    
    // 工具菜单
    QMenu *toolsMenu = menuBar()->addMenu("工具(&T)");
    qDebug() << "工具菜单创建完成:" << toolsMenu;
    qDebug() << "菜单栏:" << menuBar() << "是否可见:" << menuBar()->isVisible();
    
    m_stringFormatterAction = new QAction("字符串格式化(&S)", this);
    m_stringFormatterAction->setEnabled(true);
    connect(m_stringFormatterAction, &QAction::triggered, this, &MainWindow::onStringFormatterClicked);
    toolsMenu->addAction(m_stringFormatterAction);
    
    toolsMenu->addSeparator();
    
    m_addToolAction = new QAction("添加工具(&A)", this);
    connect(m_addToolAction, &QAction::triggered, this, &MainWindow::onAddToolClicked);
    toolsMenu->addAction(m_addToolAction);
    
    // 管理菜单
    QMenu *managementMenu = menuBar()->addMenu("管理(&M)");
    
    m_userManagementAction = new QAction("用户管理(&U)", this);
    m_userManagementAction->setEnabled(true);
    connect(m_userManagementAction, &QAction::triggered, this, &MainWindow::onUserManagementClicked);
    managementMenu->addAction(m_userManagementAction);
    
    m_roleManagementAction = new QAction("角色管理(&R)", this);
    m_roleManagementAction->setEnabled(true);
    connect(m_roleManagementAction, &QAction::triggered, this, &MainWindow::onRoleManagementClicked);
    managementMenu->addAction(m_roleManagementAction);
    
    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    
    m_aboutAction = new QAction("关于(&A)", this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
    helpMenu->addAction(m_aboutAction);
    
    // 调试信息：检查菜单和Action状态
    qDebug() << "=== 菜单创建完成，状态检查 ===";
    qDebug() << "文件菜单:" << fileMenu << "是否启用:" << fileMenu->isEnabled() << "是否可见:" << fileMenu->isVisible();
    qDebug() << "设置Action:" << m_settingsAction << "是否启用:" << m_settingsAction->isEnabled();
    qDebug() << "注销Action:" << m_logoutAction << "是否启用:" << m_logoutAction->isEnabled();
    qDebug() << "退出Action:" << m_exitAction << "是否启用:" << m_exitAction->isEnabled();
    qDebug() << "菜单栏:" << menuBar() << "是否启用:" << menuBar()->isEnabled() << "是否可见:" << menuBar()->isVisible();
    qDebug() << "================================";
}

/**
 * 创建状态栏
 */
void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel("就绪", this);
    statusBar()->addWidget(m_statusLabel);
    
    // 添加版本信息
    QLabel *versionLabel = new QLabel("v1.0.0", this);
    statusBar()->addPermanentWidget(versionLabel);
}

/**
 * 初始化工具页面
 */
void MainWindow::initializeTools()
{
    // 创建字符串格式化工具
    m_stringFormatter = new StringFormatter(this);
    
    // 创建用户管理工具
    m_userManager = new UserManager(m_apiManager, this);
    
    // 创建角色管理工具
    m_roleManager = new RoleManager(m_apiManager, this);
    
    // 可以在这里添加更多工具
}

/**
 * 设置界面样式
 */
void MainWindow::setupStyles()
{
    // 设置暗黑主题样式
    QString darkTheme = 
        "QMainWindow { "
        "background-color: #2b2b2b; "
        "color: #ffffff; "
        "} "
        "QWidget { "
        "background-color: #2b2b2b; "
        "color: #ffffff; "
        "} "
        "QTextEdit, QLineEdit, QPlainTextEdit { "
        "background-color: #3c3c3c; "
        "color: #ffffff; "
        "border: 1px solid #555555; "
        "padding: 4px; "
        "} "
        "QPushButton { "
        "background-color: #404040; "
        "color: #ffffff; "
        "border: 1px solid #555555; "
        "padding: 6px 12px; "
        "border-radius: 3px; "
        "} "
        "QPushButton:hover { "
        "background-color: #4a4a4a; "
        "} "
        "QPushButton:pressed { "
        "background-color: #353535; "
        "} "
        "QTableWidget { "
        "background-color: #2b2b2b; "
        "color: #ffffff; "
        "gridline-color: #555555; "
        "selection-background-color: #404040; "
        "} "
        "QTableWidget::item { "
        "background-color: #2b2b2b; "
        "color: #ffffff; "
        "border: none; "
        "padding: 4px; "
        "} "
        "QTableWidget::item:selected { "
        "background-color: #404040; "
        "} "
        "QHeaderView::section { "
        "background-color: #3c3c3c; "
        "color: #ffffff; "
        "border: 1px solid #555555; "
        "padding: 4px; "
        "} "
        "QMenuBar { "
        "background-color: #2b2b2b; "
        "color: #ffffff; "
        "border-bottom: 1px solid #555555; "
        "spacing: 0px; "
        "} "
        "QMenuBar::item { "
        "background-color: transparent; "
        "padding: 8px 16px; "
        "margin: 1px; "
        "border: none; "
        "} "
        "QMenuBar::item:hover { "
        "background-color: #4a90e2; "
        "color: #ffffff; "
        "} "
        "QMenuBar::item:selected { "
        "background-color: #357abd; "
        "color: #ffffff; "
        "} "
        "QMenuBar::item:pressed { "
        "background-color: #2968a3; "
        "color: #ffffff; "
        "} "
        "QMenu { "
        "background-color: #2b2b2b; "
        "color: #ffffff; "
        "border: 1px solid #555555; "
        "} "
        "QMenu::item { "
        "padding: 8px 20px; "
        "} "
        "QMenu::item:selected { "
        "background-color: #404040; "
        "} "
        "QMenu::item:pressed { "
        "background-color: #353535; "
        "} "
        "QMenu::separator { "
        "height: 1px; "
        "background-color: #555555; "
        "margin: 4px 0px; "
        "} "
        "QStatusBar { "
        "background-color: #2b2b2b; "
        "color: #ffffff; "
        "border-top: 1px solid #555555; "
        "} "
        "QLabel { "
        "color: #ffffff; "
        "} "
        "QComboBox { "
        "background-color: #3c3c3c; "
        "color: #ffffff; "
        "border: 1px solid #555555; "
        "padding: 4px; "
        "} "
        "QComboBox::drop-down { "
        "border: none; "
        "} "
        "QComboBox::down-arrow { "
        "image: none; "
        "border-left: 5px solid transparent; "
        "border-right: 5px solid transparent; "
        "border-top: 5px solid #ffffff; "
        "} "
        "QScrollBar:vertical { "
        "background-color: #3c3c3c; "
        "width: 12px; "
        "} "
        "QScrollBar::handle:vertical { "
        "background-color: #555555; "
        "border-radius: 6px; "
        "} "
        "QScrollBar::handle:vertical:hover { "
        "background-color: #666666; "
        "}";
    
    // 应用暗黑主题到整个应用程序
    this->setStyleSheet(darkTheme);
}

/**
 * 关于对话框
 */
void MainWindow::onAboutClicked()
{
    QMessageBox::about(this, "关于 DBA Tools",
                      "<h3>DBA Tools v1.0.0</h3>"
                      "<p>一个为数据库管理员设计的工具集合</p>"
                      "<p>包含各种实用的数据库管理和开发工具</p>"
                      "<p>© 2025 redgreat</p>");
}

/**
 * 退出应用程序
 */
void MainWindow::onExitClicked()
{
    qDebug() << "退出菜单被点击";
    QApplication::quit();
}

/**
 * 注销当前用户
 */
void MainWindow::onLogoutClicked()
{
    qDebug() << "注销菜单被点击";
    int ret = QMessageBox::question(this, "确认注销",
                                   "确定要注销当前用户吗?",
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // 连接注销结果信号
        connect(m_apiManager, &ApiManager::logoutResult,
                this, &MainWindow::onLogoutResult, Qt::UniqueConnection);
        
        // 调用API注销
        m_apiManager->logout();
        m_statusLabel->setText("正在注销...");
    }
}

/**
 * 处理注销结果
 */
void MainWindow::onLogoutResult(bool success, const QString &message)
{
    if (success) {
        // 清除认证信息
        QSettings settings;
        settings.remove("auth/token");
        settings.remove("auth/username");
        
        // 显示登录窗口
        LoginWindow *loginWindow = new LoginWindow();
        loginWindow->show();
        
        // 关闭主窗口
        this->close();
    } else {
        m_statusLabel->setText("注销失败: " + message);
        QMessageBox::warning(this, "注销失败", "注销失败: " + message);
    }
}

/**
 * 登录结果处理
 */
void MainWindow::onLoginResult(bool success, const QString &message, const QString &token)
{
    if (success) {
        // 登录成功，数据将在首次切换到对应标签页时自动加载
        // 不在这里直接加载，避免与showEvent冲突
    }
}

/**
 * 处理Token过期事件
 */
void MainWindow::onTokenExpired()
{
    qDebug() << "[DEBUG] Token expired, redirecting to login page";
    
    // 清除认证信息
    QSettings settings;
    settings.remove("auth/token");
    settings.remove("auth/username");
    
    // 显示提示信息
    QMessageBox::information(this, "会话过期", "您的登录会话已过期，请重新登录。");
    
    // 显示登录窗口
    LoginWindow *loginWindow = new LoginWindow();
    loginWindow->show();
    
    // 关闭主窗口
    this->close();
}

/**
 * 设置对话框
 */
void MainWindow::onSettingsClicked()
{
    qDebug() << "设置菜单被点击";
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
    }
}

/**
 * 添加工具
 */
void MainWindow::onAddToolClicked()
{
    QMessageBox::information(this, "添加工具",
                           "此功能将在后续版本中实现\n"
                           "您可以通过扩展代码来添加新的工具页面");
}

/**
 * 用户管理菜单点击事件
 */
void MainWindow::onUserManagementClicked()
{
    qDebug() << "用户管理菜单被点击";
    showTool(m_userManager);
}

/**
 * 角色管理菜单点击事件
 */
void MainWindow::onRoleManagementClicked()
{
    qDebug() << "角色管理菜单被点击";
    showTool(m_roleManager);
}

/**
 * 字符串格式化菜单点击事件
 */
void MainWindow::onStringFormatterClicked()
{
    qDebug() << "字符串格式化菜单被点击";
    showTool(m_stringFormatter);
}

/**
 * 显示指定工具页面
 */
void MainWindow::showTool(QWidget* tool)
{
    if (m_currentTool == tool) {
        return; // 已经是当前工具，无需切换
    }
    
    // 隐藏当前工具
    if (m_currentTool) {
        m_currentTool->hide();
    }
    
    // 设置新的当前工具
    m_currentTool = tool;
    setCentralWidget(m_currentTool);
    
    // 触发显示事件
    if (m_currentTool) {
        QShowEvent showEvent;
        QApplication::sendEvent(m_currentTool, &showEvent);
        m_currentTool->show();
    }
}
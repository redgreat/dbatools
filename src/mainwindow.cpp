#include "mainwindow.h"
#include "stringformatter.h"
#include "loginwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

/**
 * 主窗口构造函数
 * 初始化UI组件和工具页面
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr)
    , m_statusLabel(nullptr)
    , m_exitAction(nullptr)
    , m_aboutAction(nullptr)
    , m_logoutAction(nullptr)
    , m_settingsAction(nullptr)
    , m_addToolAction(nullptr)
    , m_stringFormatter(nullptr)
    , m_apiManager(new ApiManager(this))
{
    setupUI();
    createMenus();
    createStatusBar();
    initializeTools();
    setupStyles();
    
    // 从设置中加载服务器URL和认证令牌
    QSettings settings;
    QString serverUrl = settings.value("server/url", "http://localhost:8080/api").toString();
    m_apiManager->setBaseUrl(serverUrl);
    
    // 显示欢迎信息
    QString username = settings.value("auth/username", "用户").toString();
    m_statusLabel->setText(QString("欢迎, %1!").arg(username));
}

/**
 * 主窗口析构函数
 */
MainWindow::~MainWindow()
{
}

/**
 * 初始化UI界面
 */
void MainWindow::setupUI()
{
    setWindowTitle("DBA Tools - 工具集合");
    resize(800, 600);
    
    // 创建中央标签页组件
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);
    
    // 设置标签页位置
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setTabsClosable(false);
    m_tabWidget->setMovable(true);
}

/**
 * 创建菜单栏
 */
void MainWindow::createMenus()
{
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    
    m_settingsAction = new QAction("设置(&S)", this);
    m_settingsAction->setShortcut(QKeySequence::Preferences);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);
    fileMenu->addAction(m_settingsAction);
    
    fileMenu->addSeparator();
    
    m_logoutAction = new QAction("注销(&L)", this);
    connect(m_logoutAction, &QAction::triggered, this, &MainWindow::onLogoutClicked);
    fileMenu->addAction(m_logoutAction);
    
    m_exitAction = new QAction("退出(&X)", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::onExitClicked);
    fileMenu->addAction(m_exitAction);
    
    // 工具菜单
    QMenu *toolsMenu = menuBar()->addMenu("工具(&T)");
    
    m_addToolAction = new QAction("添加工具(&A)", this);
    connect(m_addToolAction, &QAction::triggered, this, &MainWindow::onAddToolClicked);
    toolsMenu->addAction(m_addToolAction);
    
    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    
    m_aboutAction = new QAction("关于(&A)", this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
    helpMenu->addAction(m_aboutAction);
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
    m_stringFormatter = new StringFormatter(m_apiManager, this);
    m_tabWidget->addTab(m_stringFormatter, "字符串格式化");
    
    // 可以在这里添加更多工具
    // 例如: m_tabWidget->addTab(new OtherTool(), "其他工具");
}

/**
 * 设置界面样式
 */
void MainWindow::setupStyles()
{
    // 设置标签页样式
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { "
        "border: 1px solid #c0c0c0; "
        "background-color: white; "
        "} "
        "QTabBar::tab { "
        "background-color: #f0f0f0; "
        "border: 1px solid #c0c0c0; "
        "padding: 8px 16px; "
        "margin-right: 2px; "
        "} "
        "QTabBar::tab:selected { "
        "background-color: white; "
        "border-bottom-color: white; "
        "} "
        "QTabBar::tab:hover { "
        "background-color: #e0e0e0; "
        "}"
    );
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
                      "<p>© 2024 DBA Tools Team</p>");
}

/**
 * 退出应用程序
 */
void MainWindow::onExitClicked()
{
    QApplication::quit();
}

/**
 * 注销当前用户
 */
void MainWindow::onLogoutClicked()
{
    int ret = QMessageBox::question(this, "确认注销",
                                   "确定要注销当前用户吗?",
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // 清除认证信息
        QSettings settings;
        settings.remove("auth/token");
        settings.remove("auth/username");
        
        // 显示登录窗口
        LoginWindow *loginWindow = new LoginWindow();
        loginWindow->show();
        
        // 关闭主窗口
        this->close();
    }
}

/**
 * 设置对话框
 */
void MainWindow::onSettingsClicked()
{
    QSettings settings;
    QString currentUrl = settings.value("server/url", "http://localhost:8080/api").toString();
    
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
#include "usermanager.h"
#include "usereditor.h"
#include <QHeaderView>
#include <QSplitter>
#include <QGroupBox>
#include <QFormLayout>
#include <QSpacerItem>
#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QShowEvent>

/**
 * 用户管理器构造函数
 */
UserManager::UserManager(ApiManager *apiManager, QWidget *parent)
    : QWidget(parent)
    , m_userTable(nullptr)
    , m_addButton(nullptr)
    , m_editButton(nullptr)
    , m_deleteButton(nullptr)
    , m_refreshButton(nullptr)
    , m_searchButton(nullptr)
    , m_searchEdit(nullptr)
    , m_statusLabel(nullptr)
    , m_totalLabel(nullptr)
    , m_apiManager(apiManager)
    , m_currentPage(0)
    , m_pageSize(50)
    , m_totalUsers(0)
    , m_firstShow(true)
{
    setupUI();
    setupStyles();
    setupTable();
    
    // 连接API信号
    connect(m_apiManager, &ApiManager::userListResult,
            this, &UserManager::onUserListResult);
    connect(m_apiManager, &ApiManager::userInfoResult,
            this, &UserManager::onUserInfoResult);
    connect(m_apiManager, &ApiManager::registerResult,
            this, &UserManager::onRegisterResult);
    connect(m_apiManager, &ApiManager::updateUserResult,
            this, &UserManager::onUpdateUserResult);
    connect(m_apiManager, &ApiManager::networkError,
            this, &UserManager::onNetworkError);
    
    // 不在构造时加载用户列表，等待登录成功后再加载
    // refreshUserList();
}

/**
 * 用户管理器析构函数
 */
UserManager::~UserManager()
{
}

/**
 * 显示事件，首次显示时自动加载数据
 */
void UserManager::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    if (m_firstShow) {
        m_firstShow = false;
        refreshUserList();
    }
}

/**
 * 初始化UI界面
 */
void UserManager::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 工具栏
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    
    // 搜索区域
    QLabel *searchLabel = new QLabel("搜索:", this);
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("输入用户名或邮箱搜索...");
    m_searchButton = new QPushButton("搜索", this);
    
    toolbarLayout->addWidget(searchLabel);
    toolbarLayout->addWidget(m_searchEdit);
    toolbarLayout->addWidget(m_searchButton);
    toolbarLayout->addStretch();
    
    // 操作按钮
    m_addButton = new QPushButton("添加用户", this);
    m_editButton = new QPushButton("编辑用户", this);
    m_deleteButton = new QPushButton("删除用户", this);
    m_refreshButton = new QPushButton("刷新", this);
    
    toolbarLayout->addWidget(m_addButton);
    toolbarLayout->addWidget(m_editButton);
    toolbarLayout->addWidget(m_deleteButton);
    toolbarLayout->addWidget(m_refreshButton);
    
    mainLayout->addLayout(toolbarLayout);
    
    // 用户表格
    m_userTable = new QTableWidget(this);
    mainLayout->addWidget(m_userTable);
    
    // 状态栏
    QHBoxLayout *statusLayout = new QHBoxLayout();
    m_statusLabel = new QLabel("就绪", this);
    m_totalLabel = new QLabel("总计: 0 个用户", this);
    
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_totalLabel);
    
    mainLayout->addLayout(statusLayout);
    
    // 连接信号槽
    connect(m_addButton, &QPushButton::clicked,
            this, &UserManager::onAddUserClicked);
    connect(m_editButton, &QPushButton::clicked,
            this, &UserManager::onEditUserClicked);
    connect(m_deleteButton, &QPushButton::clicked,
            this, &UserManager::onDeleteUserClicked);
    connect(m_refreshButton, &QPushButton::clicked,
            this, &UserManager::onRefreshClicked);
    connect(m_searchButton, &QPushButton::clicked,
            this, &UserManager::onSearchClicked);
    
    connect(m_searchEdit, &QLineEdit::returnPressed,
            this, &UserManager::onSearchClicked);
    
    connect(m_userTable, &QTableWidget::itemSelectionChanged,
            this, &UserManager::onUserTableSelectionChanged);
    connect(m_userTable, &QTableWidget::cellDoubleClicked,
            this, &UserManager::onUserTableDoubleClicked);
}

/**
 * 设置界面样式
 */
void UserManager::setupStyles()
{
    // 设置按钮样式
    QString buttonStyle = "QPushButton { "
                         "background-color: #0078d4; "
                         "color: white; "
                         "border: none; "
                         "padding: 8px 16px; "
                         "border-radius: 4px; "
                         "font-size: 12px; "
                         "} "
                         "QPushButton:hover { "
                         "background-color: #106ebe; "
                         "} "
                         "QPushButton:pressed { "
                         "background-color: #005a9e; "
                         "} "
                         "QPushButton:disabled { "
                         "background-color: #555555; "
                         "}";
    
    m_addButton->setStyleSheet(buttonStyle);
    m_refreshButton->setStyleSheet(buttonStyle);
    m_searchButton->setStyleSheet(buttonStyle);
    
    QString editButtonStyle = buttonStyle;
    editButtonStyle.replace("#0078d4", "#f39c12");
    editButtonStyle.replace("#106ebe", "#e67e22");
    editButtonStyle.replace("#005a9e", "#d35400");
    m_editButton->setStyleSheet(editButtonStyle);
    
    QString deleteButtonStyle = buttonStyle;
    deleteButtonStyle.replace("#0078d4", "#e74c3c");
    deleteButtonStyle.replace("#106ebe", "#c0392b");
    deleteButtonStyle.replace("#005a9e", "#a93226");
    m_deleteButton->setStyleSheet(deleteButtonStyle);
    
    // 设置搜索框样式
    m_searchEdit->setStyleSheet("QLineEdit { "
                               "padding: 6px; "
                               "border: 2px solid #555555; "
                               "border-radius: 4px; "
                               "font-size: 12px; "
                               "background-color: #1e1e1e; "
                               "color: #ffffff; "
                               "} "
                               "QLineEdit:focus { "
                               "border-color: #0078d4; "
                               "}");
    
    // 设置状态标签样式
    m_statusLabel->setStyleSheet("QLabel { color: #ffffff; font-size: 12px; }");
    m_totalLabel->setStyleSheet("QLabel { color: #ffffff; font-size: 12px; }");
}

/**
 * 初始化表格
 */
void UserManager::setupTable()
{
    // 设置列
    QStringList headers;
    headers << "ID" << "用户名" << "邮箱" << "全名" << "状态" << "角色" << "创建时间" << "更新时间";
    
    m_userTable->setColumnCount(headers.size());
    m_userTable->setHorizontalHeaderLabels(headers);
    
    // 设置表格属性
    m_userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_userTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_userTable->setAlternatingRowColors(true);
    m_userTable->setSortingEnabled(true);
    
    // 设置列宽
    QHeaderView *header = m_userTable->horizontalHeader();
    header->setStretchLastSection(true);
    header->resizeSection(0, 60);  // ID
    header->resizeSection(1, 120); // 用户名
    header->resizeSection(2, 200); // 邮箱
    header->resizeSection(3, 150); // 全名
    header->resizeSection(4, 80);  // 状态
    header->resizeSection(5, 120); // 角色
    header->resizeSection(6, 150); // 创建时间
    
    // 设置表格样式
    m_userTable->setStyleSheet("QTableWidget { "
                              "gridline-color: #555555; "
                              "background-color: #1e1e1e; "
                              "alternate-background-color: #2b2b2b; "
                              "color: #ffffff; "
                              "} "
                              "QTableWidget::item { "
                              "padding: 8px; "
                              "border: none; "
                              "color: #ffffff; "
                              "} "
                              "QTableWidget::item:selected { "
                              "background-color: #0078d4; "
                              "color: white; "
                              "} "
                              "QHeaderView::section { "
                              "background-color: #2b2b2b; "
                              "color: #ffffff; "
                              "padding: 8px; "
                              "border: 1px solid #555555; "
                              "}");
}

/**
 * 刷新用户列表
 */
void UserManager::refreshUserList()
{
    showStatus("正在加载用户列表...");
    m_apiManager->getUserList(m_currentPage * m_pageSize, m_pageSize);
}

/**
 * 更新表格数据
 */
void UserManager::updateTable()
{
    m_userTable->setRowCount(m_users.size());
    
    for (int i = 0; i < m_users.size(); ++i) {
        const UserInfo &user = m_users[i];
        
        // ID
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(user.id));
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
        m_userTable->setItem(i, 0, idItem);
        
        // 用户名
        QTableWidgetItem *usernameItem = new QTableWidgetItem(user.username);
        usernameItem->setFlags(usernameItem->flags() & ~Qt::ItemIsEditable);
        m_userTable->setItem(i, 1, usernameItem);
        
        // 邮箱
        QTableWidgetItem *emailItem = new QTableWidgetItem(user.email);
        emailItem->setFlags(emailItem->flags() & ~Qt::ItemIsEditable);
        m_userTable->setItem(i, 2, emailItem);
        
        // 全名
        QTableWidgetItem *fullNameItem = new QTableWidgetItem(user.fullName);
        fullNameItem->setFlags(fullNameItem->flags() & ~Qt::ItemIsEditable);
        m_userTable->setItem(i, 3, fullNameItem);
        
        // 状态
        QTableWidgetItem *statusItem = new QTableWidgetItem(user.isActive ? "激活" : "禁用");
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
        if (!user.isActive) {
            statusItem->setForeground(QBrush(QColor("#e74c3c")));
        }
        m_userTable->setItem(i, 4, statusItem);
        
        // 角色
        QTableWidgetItem *rolesItem = new QTableWidgetItem(user.roles.join(", "));
        rolesItem->setFlags(rolesItem->flags() & ~Qt::ItemIsEditable);
        m_userTable->setItem(i, 5, rolesItem);
        
        // 创建时间
        QTableWidgetItem *createdItem = new QTableWidgetItem(user.createdAt);
        createdItem->setFlags(createdItem->flags() & ~Qt::ItemIsEditable);
        m_userTable->setItem(i, 6, createdItem);
        
        // 最后登录时间
        QTableWidgetItem *loginItem = new QTableWidgetItem(user.lastLogin);
        loginItem->setFlags(loginItem->flags() & ~Qt::ItemIsEditable);
        m_userTable->setItem(i, 7, loginItem);
    }
    
    updateButtonStates();
    m_totalLabel->setText(QString("总计: %1 个用户").arg(m_totalUsers));
}

/**
 * 更新按钮状态
 */
void UserManager::updateButtonStates()
{
    bool hasSelection = m_userTable->currentRow() >= 0;
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}

/**
 * 获取选中的用户
 */
UserInfo UserManager::getSelectedUser() const
{
    int row = m_userTable->currentRow();
    if (row >= 0 && row < m_users.size()) {
        return m_users[row];
    }
    return UserInfo();
}

/**
 * 显示状态信息
 */
void UserManager::showStatus(const QString &message, bool isError)
{
    m_statusLabel->setText(message);
    if (isError) {
        m_statusLabel->setStyleSheet("QLabel { color: #e74c3c; font-size: 12px; }");
    } else {
        m_statusLabel->setStyleSheet("QLabel { color: #2c3e50; font-size: 12px; }");
    }
}

/**
 * 添加用户按钮点击事件
 */
void UserManager::onAddUserClicked()
{
    showUserEditDialog();
}

/**
 * 编辑用户按钮点击事件
 */
void UserManager::onEditUserClicked()
{
    UserInfo user = getSelectedUser();
    if (user.id > 0) {
        showUserEditDialog(user);
    }
}

/**
 * 删除用户按钮点击事件
 */
void UserManager::onDeleteUserClicked()
{
    UserInfo user = getSelectedUser();
    if (user.id <= 0) {
        return;
    }
    
    int ret = QMessageBox::question(this, "确认删除",
                                   QString("确定要删除用户 '%1' 吗?").arg(user.username),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // 注意: API中没有删除用户的接口，这里只是示例
        QMessageBox::information(this, "提示", "删除用户功能暂未实现");
    }
}

/**
 * 刷新按钮点击事件
 */
void UserManager::onRefreshClicked()
{
    refreshUserList();
}

/**
 * 搜索按钮点击事件
 */
void UserManager::onSearchClicked()
{
    QString searchText = m_searchEdit->text().trimmed();
    if (searchText.isEmpty()) {
        refreshUserList();
        return;
    }
    
    // 在本地数据中搜索
    QList<UserInfo> filteredUsers;
    for (const UserInfo &user : m_users) {
        if (user.username.contains(searchText, Qt::CaseInsensitive) ||
            user.email.contains(searchText, Qt::CaseInsensitive) ||
            user.fullName.contains(searchText, Qt::CaseInsensitive)) {
            filteredUsers.append(user);
        }
    }
    
    QList<UserInfo> originalUsers = m_users;
    m_users = filteredUsers;
    updateTable();
    m_users = originalUsers;
    
    showStatus(QString("搜索到 %1 个匹配的用户").arg(filteredUsers.size()));
}

/**
 * 用户列表结果处理
 */
void UserManager::onUserListResult(bool success, const QList<UserInfo> &users, const QString &error)
{
    if (success) {
        m_users = users;
        m_totalUsers = users.size();
        updateTable();
        showStatus("用户列表加载完成");
    } else {
        showStatus("加载用户列表失败: " + error, true);
    }
}

/**
 * 用户信息结果处理
 */
void UserManager::onUserInfoResult(bool success, const UserInfo &user, const QString &error)
{
    if (success) {
        showStatus("用户信息获取成功");
    } else {
        showStatus("获取用户信息失败: " + error, true);
    }
}

/**
 * 用户注册结果处理
 */
void UserManager::onRegisterResult(bool success, const QString &message)
{
    if (success) {
        showStatus("用户创建成功");
        refreshUserList();
    } else {
        showStatus("创建用户失败: " + message, true);
    }
}

/**
 * 用户更新结果处理
 */
void UserManager::onUpdateUserResult(bool success, const UserInfo &user, const QString &error)
{
    if (success) {
        showStatus("用户更新成功");
        refreshUserList();
    } else {
        showStatus("更新用户失败: " + error, true);
    }
}

/**
 * 网络错误处理
 */
void UserManager::onNetworkError(const QString &error)
{
    showStatus("网络错误: " + error, true);
}

/**
 * 表格选择变化事件
 */
void UserManager::onUserTableSelectionChanged()
{
    updateButtonStates();
}

/**
 * 表格双击事件
 */
void UserManager::onUserTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    if (row >= 0 && row < m_users.size()) {
        showUserEditDialog(m_users[row]);
    }
}

/**
 * 显示用户编辑对话框
 */
void UserManager::showUserEditDialog(const UserInfo &user)
{
    UserEditor *editor;
    if (user.id > 0) {
        editor = new UserEditor(m_apiManager, user, this);
    } else {
        editor = new UserEditor(m_apiManager, this);
    }
    
    if (editor->exec() == QDialog::Accepted) {
        // 对话框已经处理了API调用，这里不需要额外操作
        // 刷新将在API响应处理中进行
    }
    
    editor->deleteLater();
}
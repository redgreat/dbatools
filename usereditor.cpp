#include "usereditor.h"
#include <QSplitter>
#include <QGroupBox>
#include <QSpacerItem>
#include <QApplication>

/**
 * 用户编辑器构造函数（新建用户）
 */
UserEditor::UserEditor(ApiManager *apiManager, QWidget *parent)
    : QDialog(parent)
    , m_usernameEdit(nullptr)
    , m_emailEdit(nullptr)
    , m_fullNameEdit(nullptr)
    , m_passwordEdit(nullptr)
    , m_isActiveCheck(nullptr)
    , m_rolesList(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_statusLabel(nullptr)
    , m_apiManager(apiManager)
    , m_isEditMode(false)
{
    setWindowTitle("添加用户");
    setupUI();
    setupStyles();
    loadRoles();
    
    // 连接API信号
    connect(m_apiManager, &ApiManager::roleListResult,
            this, &UserEditor::onRoleListResult);
    connect(m_apiManager, &ApiManager::registerResult,
            this, &UserEditor::onRegisterResult);
    connect(m_apiManager, &ApiManager::networkError,
            this, &UserEditor::onNetworkError);
}

/**
 * 用户编辑器构造函数（编辑用户）
 */
UserEditor::UserEditor(ApiManager *apiManager, const UserInfo &user, QWidget *parent)
    : QDialog(parent)
    , m_usernameEdit(nullptr)
    , m_emailEdit(nullptr)
    , m_fullNameEdit(nullptr)
    , m_passwordEdit(nullptr)
    , m_isActiveCheck(nullptr)
    , m_rolesList(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_statusLabel(nullptr)
    , m_apiManager(apiManager)
    , m_originalUser(user)
    , m_isEditMode(true)
{
    setWindowTitle("编辑用户");
    setupUI();
    setupStyles();
    loadRoles();
    
    // 填充用户数据
    m_usernameEdit->setText(user.username);
    m_usernameEdit->setEnabled(false); // 用户名不可编辑
    m_emailEdit->setText(user.email);
    m_fullNameEdit->setText(user.fullName);
    m_isActiveCheck->setChecked(user.isActive);
    m_passwordEdit->setPlaceholderText("留空表示不修改密码");
    
    // 连接API信号
    connect(m_apiManager, &ApiManager::roleListResult,
            this, &UserEditor::onRoleListResult);
    connect(m_apiManager, &ApiManager::updateUserResult,
            this, &UserEditor::onUpdateUserResult);
    connect(m_apiManager, &ApiManager::networkError,
            this, &UserEditor::onNetworkError);
}

/**
 * 用户编辑器析构函数
 */
UserEditor::~UserEditor()
{
}

/**
 * 初始化UI界面
 */
void UserEditor::setupUI()
{
    setModal(true);
    resize(450, 500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 基本信息组
    QGroupBox *basicGroup = new QGroupBox("基本信息", this);
    QFormLayout *basicLayout = new QFormLayout(basicGroup);
    
    m_usernameEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_fullNameEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_isActiveCheck = new QCheckBox(this);
    
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    
    if (!m_isEditMode) {
        m_usernameEdit->setPlaceholderText("请输入用户名");
        m_emailEdit->setPlaceholderText("请输入邮箱地址");
        m_fullNameEdit->setPlaceholderText("请输入全名");
        m_passwordEdit->setPlaceholderText("请输入密码");
        m_isActiveCheck->setChecked(true);
    }
    
    basicLayout->addRow("用户名:", m_usernameEdit);
    basicLayout->addRow("邮箱:", m_emailEdit);
    basicLayout->addRow("全名:", m_fullNameEdit);
    basicLayout->addRow("密码:", m_passwordEdit);
    basicLayout->addRow("激活状态:", m_isActiveCheck);
    
    mainLayout->addWidget(basicGroup);
    
    // 角色信息组
    QGroupBox *roleGroup = new QGroupBox("角色分配", this);
    QVBoxLayout *roleLayout = new QVBoxLayout(roleGroup);
    
    QLabel *roleLabel = new QLabel("选择用户角色:", this);
    m_rolesList = new QListWidget(this);
    m_rolesList->setSelectionMode(QAbstractItemView::MultiSelection);
    m_rolesList->setMaximumHeight(120);
    
    roleLayout->addWidget(roleLabel);
    roleLayout->addWidget(m_rolesList);
    
    mainLayout->addWidget(roleGroup);
    
    // 状态标签
    m_statusLabel = new QLabel("就绪", this);
    mainLayout->addWidget(m_statusLabel);
    
    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_okButton = new QPushButton(m_isEditMode ? "更新" : "创建", this);
    m_cancelButton = new QPushButton("取消", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(m_okButton, &QPushButton::clicked,
            this, &UserEditor::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &UserEditor::onCancelClicked);
}

/**
 * 设置界面样式
 */
void UserEditor::setupStyles()
{
    // 设置对话框样式
    setStyleSheet("QDialog { background-color: #f8f9fa; }");
    
    // 设置输入框样式
    QString inputStyle = "QLineEdit { "
                        "padding: 8px; "
                        "border: 2px solid #e9ecef; "
                        "border-radius: 4px; "
                        "font-size: 12px; "
                        "background-color: white; "
                        "} "
                        "QLineEdit:focus { "
                        "border-color: #3498db; "
                        "}";
    
    m_usernameEdit->setStyleSheet(inputStyle);
    m_emailEdit->setStyleSheet(inputStyle);
    m_fullNameEdit->setStyleSheet(inputStyle);
    m_passwordEdit->setStyleSheet(inputStyle);
    
    // 设置复选框样式
    m_isActiveCheck->setStyleSheet("QCheckBox { "
                                  "font-size: 12px; "
                                  "spacing: 8px; "
                                  "}");
    
    // 设置列表样式
    m_rolesList->setStyleSheet("QListWidget { "
                              "border: 2px solid #e9ecef; "
                              "border-radius: 4px; "
                              "background-color: white; "
                              "font-size: 12px; "
                              "} "
                              "QListWidget::item { "
                              "padding: 6px; "
                              "border-bottom: 1px solid #f1f3f4; "
                              "} "
                              "QListWidget::item:selected { "
                              "background-color: #3498db; "
                              "color: white; "
                              "}");
    
    // 设置按钮样式
    QString okButtonStyle = "QPushButton { "
                           "background-color: #28a745; "
                           "color: white; "
                           "border: none; "
                           "padding: 10px 20px; "
                           "border-radius: 4px; "
                           "font-size: 12px; "
                           "font-weight: bold; "
                           "} "
                           "QPushButton:hover { "
                           "background-color: #218838; "
                           "} "
                           "QPushButton:pressed { "
                           "background-color: #1e7e34; "
                           "}";
    
    QString cancelButtonStyle = "QPushButton { "
                               "background-color: #6c757d; "
                               "color: white; "
                               "border: none; "
                               "padding: 10px 20px; "
                               "border-radius: 4px; "
                               "font-size: 12px; "
                               "} "
                               "QPushButton:hover { "
                               "background-color: #5a6268; "
                               "} "
                               "QPushButton:pressed { "
                               "background-color: #545b62; "
                               "}";
    
    m_okButton->setStyleSheet(okButtonStyle);
    m_cancelButton->setStyleSheet(cancelButtonStyle);
    
    // 设置状态标签样式
    m_statusLabel->setStyleSheet("QLabel { "
                                "color: #6c757d; "
                                "font-size: 11px; "
                                "padding: 4px; "
                                "}");
    
    // 设置组框样式
    QString groupBoxStyle = "QGroupBox { "
                           "font-weight: bold; "
                           "border: 2px solid #e9ecef; "
                           "border-radius: 6px; "
                           "margin-top: 10px; "
                           "background-color: white; "
                           "} "
                           "QGroupBox::title { "
                           "subcontrol-origin: margin; "
                           "left: 10px; "
                           "padding: 0 8px 0 8px; "
                           "color: #495057; "
                           "}";
    
    findChild<QGroupBox*>()->setStyleSheet(groupBoxStyle);
    findChildren<QGroupBox*>().last()->setStyleSheet(groupBoxStyle);
}

/**
 * 加载角色列表
 */
void UserEditor::loadRoles()
{
    showStatus("正在加载角色列表...");
    m_apiManager->getRoleList();
}

/**
 * 验证输入数据
 */
bool UserEditor::validateInput()
{
    QString username = m_usernameEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    
    if (username.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "用户名不能为空");
        m_usernameEdit->setFocus();
        return false;
    }
    
    if (email.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "邮箱不能为空");
        m_emailEdit->setFocus();
        return false;
    }
    
    // 简单的邮箱格式验证
    if (!email.contains("@") || !email.contains(".")) {
        QMessageBox::warning(this, "输入错误", "请输入有效的邮箱地址");
        m_emailEdit->setFocus();
        return false;
    }
    
    if (!m_isEditMode && password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "密码不能为空");
        m_passwordEdit->setFocus();
        return false;
    }
    
    if (!password.isEmpty() && password.length() < 6) {
        QMessageBox::warning(this, "输入错误", "密码长度不能少于6位");
        m_passwordEdit->setFocus();
        return false;
    }
    
    return true;
}

/**
 * 显示状态信息
 */
void UserEditor::showStatus(const QString &message, bool isError)
{
    m_statusLabel->setText(message);
    if (isError) {
        m_statusLabel->setStyleSheet("QLabel { "
                                    "color: #dc3545; "
                                    "font-size: 11px; "
                                    "padding: 4px; "
                                    "}");
    } else {
        m_statusLabel->setStyleSheet("QLabel { "
                                    "color: #6c757d; "
                                    "font-size: 11px; "
                                    "padding: 4px; "
                                    "}");
    }
}

/**
 * 更新用户角色显示
 */
void UserEditor::updateUserRoles()
{
    if (!m_isEditMode) {
        return;
    }
    
    // 选中用户已有的角色
    for (int i = 0; i < m_rolesList->count(); ++i) {
        QListWidgetItem *item = m_rolesList->item(i);
        int roleId = item->data(Qt::UserRole).toInt();
        
        for (const QString &userRoleName : m_originalUser.roles) {
            for (const RoleInfo &availableRole : m_availableRoles) {
                if (availableRole.name == userRoleName) {
                    if (availableRole.id == roleId) {
                        item->setSelected(true);
                        break;
                    }
                }
            }
        }
    }
}

/**
 * 获取编辑后的用户信息
 */
UserInfo UserEditor::getUserInfo() const
{
    UserInfo user;
    user.id = m_isEditMode ? m_originalUser.id : 0;
    user.username = m_usernameEdit->text().trimmed();
    user.email = m_emailEdit->text().trimmed();
    user.fullName = m_fullNameEdit->text().trimmed();
    user.isActive = m_isActiveCheck->isChecked();
    
    // 获取选中的角色
    for (int i = 0; i < m_rolesList->count(); ++i) {
        QListWidgetItem *item = m_rolesList->item(i);
        if (item->isSelected()) {
            int roleId = item->data(Qt::UserRole).toInt();
            for (const RoleInfo &role : m_availableRoles) {
                if (role.id == roleId) {
                    user.roles.append(role.name);
                    break;
                }
            }
        }
    }
    
    return user;
}

/**
 * 确定按钮点击事件
 */
void UserEditor::onOkClicked()
{
    if (!validateInput()) {
        return;
    }
    
    QString username = m_usernameEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();
    QString fullName = m_fullNameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    bool isActive = m_isActiveCheck->isChecked();
    
    m_okButton->setEnabled(false);
    
    if (m_isEditMode) {
        showStatus("正在更新用户...");
        m_apiManager->updateUser(m_originalUser.id, email, fullName, isActive);
    } else {
        showStatus("正在创建用户...");
        m_apiManager->registerUser(username, email, password, fullName);
    }
}

/**
 * 取消按钮点击事件
 */
void UserEditor::onCancelClicked()
{
    reject();
}

/**
 * 角色列表结果处理
 */
void UserEditor::onRoleListResult(bool success, const QList<RoleInfo> &roles, const QString &error)
{
    if (success) {
        m_availableRoles = roles;
        m_rolesList->clear();
        
        for (const RoleInfo &role : roles) {
            QListWidgetItem *item = new QListWidgetItem(role.displayName, m_rolesList);
            item->setData(Qt::UserRole, role.id);
            item->setToolTip(role.description);
        }
        
        updateUserRoles();
        showStatus("角色列表加载完成");
    } else {
        showStatus("加载角色列表失败: " + error, true);
    }
}

/**
 * 用户注册结果处理
 */
void UserEditor::onRegisterResult(bool success, const QString &message)
{
    m_okButton->setEnabled(true);
    
    if (success) {
        showStatus("用户创建成功");
        accept();
    } else {
        showStatus("创建用户失败: " + message, true);
    }
}

/**
 * 用户更新结果处理
 */
void UserEditor::onUpdateUserResult(bool success, const UserInfo &user, const QString &error)
{
    Q_UNUSED(user)
    m_okButton->setEnabled(true);
    
    if (success) {
        showStatus("用户更新成功");
        accept();
    } else {
        showStatus("更新用户失败: " + error, true);
    }
}

/**
 * 网络错误处理
 */
void UserEditor::onNetworkError(const QString &error)
{
    m_okButton->setEnabled(true);
    showStatus("网络错误: " + error, true);
}
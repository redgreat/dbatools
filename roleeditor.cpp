#include "roleeditor.h"
#include <QMessageBox>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QApplication>

/**
 * 构造函数 - 新建角色
 */
RoleEditor::RoleEditor(ApiManager *apiManager, QWidget *parent)
    : QDialog(parent)
    , m_apiManager(apiManager)
    , m_nameEdit(nullptr)
    , m_descriptionEdit(nullptr)
    , m_permissionScrollArea(nullptr)
    , m_permissionWidget(nullptr)
    , m_permissionLayout(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_statusLabel(nullptr)
    , m_isEditMode(false)
{
    setWindowTitle("新建角色");
    setupUI();
    setupStyles();
    loadPermissions();
    
    // 连接API管理器信号
    connect(m_apiManager, &ApiManager::permissionListResult, this, &RoleEditor::onPermissionListResult);
    connect(m_apiManager, &ApiManager::createRoleResult, this, &RoleEditor::onCreateRoleResult);
    connect(m_apiManager, &ApiManager::networkError, this, &RoleEditor::onNetworkError);
    
    // 连接按钮信号
    connect(m_okButton, &QPushButton::clicked, this, &RoleEditor::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &RoleEditor::onCancelClicked);
}

/**
 * 构造函数 - 编辑角色
 */
RoleEditor::RoleEditor(ApiManager *apiManager, const RoleInfo &role, QWidget *parent)
    : QDialog(parent)
    , m_apiManager(apiManager)
    , m_originalRole(role)
    , m_nameEdit(nullptr)
    , m_descriptionEdit(nullptr)
    , m_permissionScrollArea(nullptr)
    , m_permissionWidget(nullptr)
    , m_permissionLayout(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_statusLabel(nullptr)
    , m_isEditMode(true)
{
    setWindowTitle("编辑角色");
    setupUI();
    setupStyles();
    loadPermissions();
    
    // 填充现有角色信息
    m_nameEdit->setText(role.name);
    m_descriptionEdit->setPlainText(role.description);
    
    // 连接API管理器信号
    connect(m_apiManager, &ApiManager::permissionListResult, this, &RoleEditor::onPermissionListResult);
    connect(m_apiManager, &ApiManager::updateRoleResult, this, &RoleEditor::onUpdateRoleResult);
    connect(m_apiManager, &ApiManager::networkError, this, &RoleEditor::onNetworkError);
    
    // 连接按钮信号
    connect(m_okButton, &QPushButton::clicked, this, &RoleEditor::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &RoleEditor::onCancelClicked);
}

/**
 * 析构函数
 */
RoleEditor::~RoleEditor()
{
}

/**
 * 初始化UI界面
 */
void RoleEditor::setupUI()
{
    setModal(true);
    setMinimumSize(500, 600);
    resize(600, 700);
    
    auto *mainLayout = new QVBoxLayout(this);
    
    // 基本信息组
    auto *basicGroup = new QGroupBox("基本信息");
    auto *basicLayout = new QFormLayout(basicGroup);
    
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("输入角色名称...");
    
    m_descriptionEdit = new QTextEdit();
    m_descriptionEdit->setPlaceholderText("输入角色描述...");
    m_descriptionEdit->setMaximumHeight(100);
    
    basicLayout->addRow("角色名称:", m_nameEdit);
    basicLayout->addRow("角色描述:", m_descriptionEdit);
    
    // 权限组
    auto *permissionGroup = new QGroupBox("权限设置");
    auto *permissionGroupLayout = new QVBoxLayout(permissionGroup);
    
    m_permissionScrollArea = new QScrollArea();
    m_permissionScrollArea->setWidgetResizable(true);
    m_permissionScrollArea->setMinimumHeight(300);
    
    m_permissionWidget = new QWidget();
    m_permissionLayout = new QVBoxLayout(m_permissionWidget);
    m_permissionLayout->setAlignment(Qt::AlignTop);
    
    m_permissionScrollArea->setWidget(m_permissionWidget);
    permissionGroupLayout->addWidget(m_permissionScrollArea);
    
    // 按钮组
    auto *buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton(m_isEditMode ? "更新" : "创建");
    m_cancelButton = new QPushButton("取消");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    // 状态标签
    m_statusLabel = new QLabel("就绪");
    
    mainLayout->addWidget(basicGroup);
    mainLayout->addWidget(permissionGroup);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_statusLabel);
}

/**
 * 设置界面样式
 */
void RoleEditor::setupStyles()
{
    setStyleSheet(
        "QGroupBox {"
        "    font-weight: bold;"
        "    border: 2px solid #cccccc;"
        "    border-radius: 5px;"
        "    margin-top: 1ex;"
        "    padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
        "QPushButton {"
        "    background-color: #f0f0f0;"
        "    border: 1px solid #cccccc;"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "    min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e0e0e0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #d0d0d0;"
        "}"
        "QLineEdit, QTextEdit {"
        "    border: 1px solid #cccccc;"
        "    border-radius: 4px;"
        "    padding: 6px;"
        "}"
        "QCheckBox {"
        "    spacing: 8px;"
        "    padding: 4px;"
        "}"
        "QCheckBox::indicator {"
        "    width: 16px;"
        "    height: 16px;"
        "}"
        "QScrollArea {"
        "    border: 1px solid #cccccc;"
        "    border-radius: 4px;"
        "}"
    );
}

/**
 * 加载权限列表
 */
void RoleEditor::loadPermissions()
{
    showStatus("正在加载权限列表...");
    m_apiManager->getPermissionList();
}

/**
 * 验证输入
 */
bool RoleEditor::validateInput()
{
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入角色名称");
        m_nameEdit->setFocus();
        return false;
    }
    
    if (m_descriptionEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入角色描述");
        m_descriptionEdit->setFocus();
        return false;
    }
    
    return true;
}

/**
 * 显示状态信息
 */
void RoleEditor::showStatus(const QString &message, bool isError)
{
    m_statusLabel->setText(message);
    if (isError) {
        m_statusLabel->setStyleSheet("color: red;");
    } else {
        m_statusLabel->setStyleSheet("color: black;");
    }
}

/**
 * 更新权限显示
 */
void RoleEditor::updatePermissionDisplay()
{
    // 清除现有的复选框
    for (QCheckBox *checkBox : m_permissionCheckBoxes) {
        m_permissionLayout->removeWidget(checkBox);
        delete checkBox;
    }
    m_permissionCheckBoxes.clear();
    
    // 创建新的复选框
    for (const PermissionInfo &permission : m_availablePermissions) {
        QCheckBox *checkBox = new QCheckBox(QString("%1 - %2")
                                           .arg(permission.name)
                                           .arg(permission.description));
        checkBox->setProperty("permissionId", permission.id);
        
        m_permissionCheckBoxes.append(checkBox);
        m_permissionLayout->addWidget(checkBox);
    }
    
    // 如果是编辑模式，设置已选中的权限
    if (m_isEditMode) {
        QList<int> rolePermissions;
        for (const PermissionInfo &permission : m_originalRole.permissions) {
            rolePermissions.append(permission.id);
        }
        setSelectedPermissions(rolePermissions);
    }
}

/**
 * 获取选中的权限列表
 */
QList<int> RoleEditor::getSelectedPermissions() const
{
    QList<int> selectedPermissions;
    for (QCheckBox *checkBox : m_permissionCheckBoxes) {
        if (checkBox->isChecked()) {
            selectedPermissions.append(checkBox->property("permissionId").toInt());
        }
    }
    return selectedPermissions;
}

/**
 * 设置选中的权限
 */
void RoleEditor::setSelectedPermissions(const QList<int> &permissionIds)
{
    for (QCheckBox *checkBox : m_permissionCheckBoxes) {
        int permissionId = checkBox->property("permissionId").toInt();
        checkBox->setChecked(permissionIds.contains(permissionId));
    }
}

/**
 * 获取编辑后的角色信息
 */
RoleInfo RoleEditor::getRoleInfo() const
{
    RoleInfo role = m_originalRole;
    role.name = m_nameEdit->text().trimmed();
    role.description = m_descriptionEdit->toPlainText().trimmed();
    
    // 获取选中的权限
    role.permissions.clear();
    QList<int> selectedPermissionIds = getSelectedPermissions();
    for (int permissionId : selectedPermissionIds) {
        for (const PermissionInfo &permission : m_availablePermissions) {
            if (permission.id == permissionId) {
                role.permissions.append(permission);
                break;
            }
        }
    }
    
    return role;
}

/**
 * 确定按钮点击事件
 */
void RoleEditor::onOkClicked()
{
    if (!validateInput()) {
        return;
    }
    
    RoleInfo role = getRoleInfo();
    
    if (m_isEditMode) {
        showStatus("正在更新角色...");
        m_apiManager->updateRole(role.id, role.displayName, role.description, role.isActive);
    } else {
        showStatus("正在创建角色...");
        m_apiManager->createRole(role.name, role.displayName, role.description);
    }
    
    m_okButton->setEnabled(false);
}

/**
 * 取消按钮点击事件
 */
void RoleEditor::onCancelClicked()
{
    reject();
}

/**
 * 权限列表结果处理
 */
void RoleEditor::onPermissionListResult(bool success, const QList<PermissionInfo> &permissions, const QString &error)
{
    if (success) {
        m_availablePermissions = permissions;
        updatePermissionDisplay();
        showStatus("权限列表加载完成");
    } else {
        showStatus(QString("加载权限列表失败: %1").arg(error), true);
    }
}

/**
 * 角色创建结果处理
 */
void RoleEditor::onCreateRoleResult(bool success, const RoleInfo &role, const QString &error)
{
    m_okButton->setEnabled(true);
    
    if (success) {
        showStatus("角色创建成功");
        QMessageBox::information(this, "成功", "角色创建成功！");
        accept();
    } else {
        showStatus(QString("创建角色失败: %1").arg(error), true);
        QMessageBox::warning(this, "错误", QString("创建角色失败:\n%1").arg(error));
    }
}

/**
 * 角色更新结果处理
 */
void RoleEditor::onUpdateRoleResult(bool success, const RoleInfo &role, const QString &error)
{
    m_okButton->setEnabled(true);
    
    if (success) {
        showStatus("角色更新成功");
        QMessageBox::information(this, "成功", "角色更新成功！");
        accept();
    } else {
        showStatus(QString("更新角色失败: %1").arg(error), true);
        QMessageBox::warning(this, "错误", QString("更新角色失败:\n%1").arg(error));
    }
}

/**
 * 网络错误处理
 */
void RoleEditor::onNetworkError(const QString &error)
{
    m_okButton->setEnabled(true);
    showStatus(QString("网络错误: %1").arg(error), true);
    QMessageBox::warning(this, "网络错误", QString("网络连接出现问题:\n%1").arg(error));
}
#include "rolemanager.h"
#include "roleeditor.h"
#include <QMessageBox>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QHeaderView>

/**
 * 构造函数
 */
RoleManager::RoleManager(ApiManager *apiManager, QWidget *parent)
    : QWidget(parent)
    , m_apiManager(apiManager)
    , m_roleTable(nullptr)
    , m_addButton(nullptr)
    , m_editButton(nullptr)
    , m_deleteButton(nullptr)
    , m_refreshButton(nullptr)
    , m_searchButton(nullptr)
    , m_searchEdit(nullptr)
    , m_statusLabel(nullptr)
    , m_totalLabel(nullptr)
    , m_totalRoles(0)
{
    setupUI();
    setupStyles();
    setupTable();
    
    // 连接API管理器信号
    connect(m_apiManager, &ApiManager::roleListResult, this, &RoleManager::onRoleListResult);
    connect(m_apiManager, &ApiManager::roleInfoResult, this, &RoleManager::onRoleInfoResult);
    connect(m_apiManager, &ApiManager::createRoleResult, this, &RoleManager::onCreateRoleResult);
    connect(m_apiManager, &ApiManager::updateRoleResult, this, &RoleManager::onUpdateRoleResult);
    connect(m_apiManager, &ApiManager::deleteRoleResult, this, &RoleManager::onDeleteRoleResult);
    connect(m_apiManager, &ApiManager::networkError, this, &RoleManager::onNetworkError);
    
    // 连接按钮信号
    connect(m_addButton, &QPushButton::clicked, this, &RoleManager::onAddRoleClicked);
    connect(m_editButton, &QPushButton::clicked, this, &RoleManager::onEditRoleClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &RoleManager::onDeleteRoleClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &RoleManager::onRefreshClicked);
    connect(m_searchButton, &QPushButton::clicked, this, &RoleManager::onSearchClicked);
    
    // 连接表格信号
    connect(m_roleTable, &QTableWidget::itemSelectionChanged, this, &RoleManager::onRoleTableSelectionChanged);
    connect(m_roleTable, &QTableWidget::cellDoubleClicked, this, &RoleManager::onRoleTableDoubleClicked);
    
    // 初始加载角色列表
    refreshRoleList();
}

/**
 * 析构函数
 */
RoleManager::~RoleManager()
{
}

/**
 * 初始化UI界面
 */
void RoleManager::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // 顶部工具栏
    auto *toolbarLayout = new QHBoxLayout();
    
    // 搜索区域
    auto *searchGroup = new QGroupBox("搜索角色");
    auto *searchLayout = new QHBoxLayout(searchGroup);
    
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("输入角色名称或描述...");
    m_searchButton = new QPushButton("搜索");
    
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchButton);
    
    // 操作按钮区域
    auto *buttonGroup = new QGroupBox("操作");
    auto *buttonLayout = new QHBoxLayout(buttonGroup);
    
    m_addButton = new QPushButton("添加角色");
    m_editButton = new QPushButton("编辑角色");
    m_deleteButton = new QPushButton("删除角色");
    m_refreshButton = new QPushButton("刷新");
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_refreshButton);
    
    toolbarLayout->addWidget(searchGroup);
    toolbarLayout->addWidget(buttonGroup);
    
    // 角色表格
    m_roleTable = new QTableWidget();
    
    // 状态栏
    auto *statusLayout = new QHBoxLayout();
    m_statusLabel = new QLabel("就绪");
    m_totalLabel = new QLabel("总计: 0 个角色");
    
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_totalLabel);
    
    mainLayout->addLayout(toolbarLayout);
    mainLayout->addWidget(m_roleTable);
    mainLayout->addLayout(statusLayout);
}

/**
 * 设置界面样式
 */
void RoleManager::setupStyles()
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
        "    padding: 6px 12px;"
        "    min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e0e0e0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #d0d0d0;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #f5f5f5;"
        "    color: #999999;"
        "}"
        "QLineEdit {"
        "    border: 1px solid #cccccc;"
        "    border-radius: 4px;"
        "    padding: 6px;"
        "}"
        "QTableWidget {"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: #3875d7;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f5f5f5;"
        "    padding: 6px;"
        "    border: 1px solid #e0e0e0;"
        "    font-weight: bold;"
        "}"
    );
}

/**
 * 初始化表格
 */
void RoleManager::setupTable()
{
    QStringList headers;
    headers << "ID" << "角色名称" << "描述" << "权限数量" << "创建时间" << "更新时间";
    
    m_roleTable->setColumnCount(headers.size());
    m_roleTable->setHorizontalHeaderLabels(headers);
    
    // 设置表格属性
    m_roleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_roleTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_roleTable->setAlternatingRowColors(true);
    m_roleTable->setSortingEnabled(true);
    
    // 设置列宽
    m_roleTable->horizontalHeader()->setStretchLastSection(true);
    m_roleTable->setColumnWidth(0, 80);   // ID
    m_roleTable->setColumnWidth(1, 150);  // 角色名称
    m_roleTable->setColumnWidth(2, 200);  // 描述
    m_roleTable->setColumnWidth(3, 100);  // 权限数量
    m_roleTable->setColumnWidth(4, 150);  // 创建时间
    
    updateButtonStates();
}

/**
 * 刷新角色列表
 */
void RoleManager::refreshRoleList()
{
    showStatus("正在加载角色列表...");
    m_apiManager->getRoleList();
}

/**
 * 更新表格数据
 */
void RoleManager::updateTable()
{
    m_roleTable->setRowCount(m_roles.size());
    
    for (int i = 0; i < m_roles.size(); ++i) {
        const RoleInfo &role = m_roles[i];
        
        m_roleTable->setItem(i, 0, new QTableWidgetItem(QString::number(role.id)));
        m_roleTable->setItem(i, 1, new QTableWidgetItem(role.name));
        m_roleTable->setItem(i, 2, new QTableWidgetItem(role.description));
        m_roleTable->setItem(i, 3, new QTableWidgetItem(QString::number(role.permissions.size())));
        m_roleTable->setItem(i, 4, new QTableWidgetItem(role.createdAt));
        m_roleTable->setItem(i, 5, new QTableWidgetItem(role.updatedAt));
        
        // 设置ID列不可编辑
        m_roleTable->item(i, 0)->setFlags(m_roleTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
    }
    
    m_totalLabel->setText(QString("总计: %1 个角色").arg(m_roles.size()));
    updateButtonStates();
}

/**
 * 更新按钮状态
 */
void RoleManager::updateButtonStates()
{
    bool hasSelection = m_roleTable->currentRow() >= 0;
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}

/**
 * 获取选中的角色
 */
RoleInfo RoleManager::getSelectedRole() const
{
    int currentRow = m_roleTable->currentRow();
    if (currentRow >= 0 && currentRow < m_roles.size()) {
        return m_roles[currentRow];
    }
    return RoleInfo();
}

/**
 * 显示角色编辑对话框
 */
void RoleManager::showRoleEditDialog(const RoleInfo &role)
{
    RoleEditor dialog(m_apiManager, role, this);
    if (dialog.exec() == QDialog::Accepted) {
        // 刷新角色列表
        refreshRoleList();
    }
}

/**
 * 显示状态信息
 */
void RoleManager::showStatus(const QString &message, bool isError)
{
    m_statusLabel->setText(message);
    if (isError) {
        m_statusLabel->setStyleSheet("color: red;");
    } else {
        m_statusLabel->setStyleSheet("color: black;");
    }
}

/**
 * 添加角色按钮点击事件
 */
void RoleManager::onAddRoleClicked()
{
    showRoleEditDialog();
}

/**
 * 编辑角色按钮点击事件
 */
void RoleManager::onEditRoleClicked()
{
    RoleInfo role = getSelectedRole();
    if (role.id > 0) {
        showRoleEditDialog(role);
    }
}

/**
 * 删除角色按钮点击事件
 */
void RoleManager::onDeleteRoleClicked()
{
    RoleInfo role = getSelectedRole();
    if (role.id <= 0) {
        return;
    }
    
    int ret = QMessageBox::question(this, "确认删除", 
                                   QString("确定要删除角色 '%1' 吗？\n此操作不可撤销。")
                                   .arg(role.name),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        showStatus("正在删除角色...");
        m_apiManager->deleteRole(role.id);
    }
}

/**
 * 刷新按钮点击事件
 */
void RoleManager::onRefreshClicked()
{
    refreshRoleList();
}

/**
 * 搜索按钮点击事件
 */
void RoleManager::onSearchClicked()
{
    QString searchText = m_searchEdit->text().trimmed();
    if (searchText.isEmpty()) {
        refreshRoleList();
        return;
    }
    
    // 在本地角色列表中搜索
    QList<RoleInfo> filteredRoles;
    for (const RoleInfo &role : m_roles) {
        if (role.name.contains(searchText, Qt::CaseInsensitive) ||
            role.description.contains(searchText, Qt::CaseInsensitive)) {
            filteredRoles.append(role);
        }
    }
    
    // 临时更新显示
    QList<RoleInfo> originalRoles = m_roles;
    m_roles = filteredRoles;
    updateTable();
    m_roles = originalRoles;
    
    showStatus(QString("搜索到 %1 个匹配的角色").arg(filteredRoles.size()));
}

/**
 * 角色列表结果处理
 */
void RoleManager::onRoleListResult(bool success, const QList<RoleInfo> &roles, const QString &error)
{
    if (success) {
        m_roles = roles;
        updateTable();
        showStatus("角色列表加载完成");
    } else {
        showStatus(QString("加载角色列表失败: %1").arg(error), true);
    }
}

/**
 * 角色信息结果处理
 */
void RoleManager::onRoleInfoResult(bool success, const RoleInfo &role, const QString &error)
{
    if (success) {
        showStatus("角色信息获取成功");
    } else {
        showStatus(QString("获取角色信息失败: %1").arg(error), true);
    }
}

/**
 * 角色创建结果处理
 */
void RoleManager::onCreateRoleResult(bool success, const RoleInfo &role, const QString &error)
{
    if (success) {
        showStatus("角色创建成功");
        refreshRoleList();
    } else {
        showStatus(QString("创建角色失败: %1").arg(error), true);
    }
}

/**
 * 角色更新结果处理
 */
void RoleManager::onUpdateRoleResult(bool success, const RoleInfo &role, const QString &error)
{
    if (success) {
        showStatus("角色更新成功");
        refreshRoleList();
    } else {
        showStatus(QString("更新角色失败: %1").arg(error), true);
    }
}

/**
 * 角色删除结果处理
 */
void RoleManager::onDeleteRoleResult(bool success, const QString &message)
{
    if (success) {
        showStatus("角色删除成功");
        refreshRoleList();
    } else {
        showStatus(QString("删除角色失败: %1").arg(message), true);
    }
}

/**
 * 网络错误处理
 */
void RoleManager::onNetworkError(const QString &error)
{
    showStatus(QString("网络错误: %1").arg(error), true);
}

/**
 * 表格选择变化事件
 */
void RoleManager::onRoleTableSelectionChanged()
{
    updateButtonStates();
}

/**
 * 表格双击事件
 */
void RoleManager::onRoleTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    if (row >= 0 && row < m_roles.size()) {
        onEditRoleClicked();
    }
}
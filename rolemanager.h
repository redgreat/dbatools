#ifndef ROLEMANAGER_H
#define ROLEMANAGER_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include "apimanager.h"

class RoleManager : public QWidget
{
    Q_OBJECT

public:
    explicit RoleManager(ApiManager *apiManager, QWidget *parent = nullptr);
    ~RoleManager();

protected:
    /**
     * 显示事件，首次显示时自动加载数据
     */
    void showEvent(QShowEvent *event) override;

public slots:
    /**
     * 刷新角色列表
     */
    void refreshRoleList();

private slots:
    /**
     * 添加角色按钮点击事件
     */
    void onAddRoleClicked();
    
    /**
     * 编辑角色按钮点击事件
     */
    void onEditRoleClicked();
    
    /**
     * 删除角色按钮点击事件
     */
    void onDeleteRoleClicked();
    
    /**
     * 刷新按钮点击事件
     */
    void onRefreshClicked();
    
    /**
     * 搜索按钮点击事件
     */
    void onSearchClicked();
    
    /**
     * 角色列表结果处理
     */
    void onRoleListResult(bool success, const QList<RoleInfo> &roles, const QString &error);
    
    /**
     * 角色信息结果处理
     */
    void onRoleInfoResult(bool success, const RoleInfo &role, const QString &error);
    
    /**
     * 角色创建结果处理
     */
    void onCreateRoleResult(bool success, const RoleInfo &role, const QString &error);
    
    /**
     * 角色更新结果处理
     */
    void onUpdateRoleResult(bool success, const RoleInfo &role, const QString &error);
    
    /**
     * 角色删除结果处理
     */
    void onDeleteRoleResult(bool success, const QString &message);
    
    /**
     * 网络错误处理
     */
    void onNetworkError(const QString &error);
    
    /**
     * 表格选择变化事件
     */
    void onRoleTableSelectionChanged();
    
    /**
     * 表格双击事件
     */
    void onRoleTableDoubleClicked(int row, int column);

private:
    /**
     * 初始化UI界面
     */
    void setupUI();
    
    /**
     * 设置界面样式
     */
    void setupStyles();
    
    /**
     * 初始化表格
     */
    void setupTable();
    
    /**
     * 更新表格数据
     */
    void updateTable();
    
    /**
     * 更新按钮状态
     */
    void updateButtonStates();
    
    /**
     * 获取选中的角色
     */
    RoleInfo getSelectedRole() const;
    
    /**
     * 显示角色编辑对话框
     */
    void showRoleEditDialog(const RoleInfo &role = RoleInfo());
    
    /**
     * 显示状态信息
     */
    void showStatus(const QString &message, bool isError = false);

private:
    // UI组件
    QTableWidget *m_roleTable;
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
    QPushButton *m_refreshButton;
    QPushButton *m_searchButton;
    QLineEdit *m_searchEdit;
    QLabel *m_statusLabel;
    QLabel *m_totalLabel;
    
    // 数据
    ApiManager *m_apiManager;
    QList<RoleInfo> m_roles;
    int m_totalRoles;
    bool m_firstShow;
};

#endif // ROLEMANAGER_H
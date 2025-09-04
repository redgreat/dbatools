#ifndef ROLEEDITOR_H
#define ROLEEDITOR_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QScrollArea>
#include "apimanager.h"

class RoleEditor : public QDialog
{
    Q_OBJECT

public:
    /**
     * 构造函数 - 新建角色
     */
    explicit RoleEditor(ApiManager *apiManager, QWidget *parent = nullptr);
    
    /**
     * 构造函数 - 编辑角色
     */
    explicit RoleEditor(ApiManager *apiManager, const RoleInfo &role, QWidget *parent = nullptr);
    
    ~RoleEditor();
    
    /**
     * 获取编辑后的角色信息
     */
    RoleInfo getRoleInfo() const;

private slots:
    /**
     * 确定按钮点击事件
     */
    void onOkClicked();
    
    /**
     * 取消按钮点击事件
     */
    void onCancelClicked();
    
    /**
     * 权限列表结果处理
     */
    void onPermissionListResult(bool success, const QList<PermissionInfo> &permissions, const QString &error);
    
    /**
     * 角色创建结果处理
     */
    void onCreateRoleResult(bool success, const RoleInfo &role, const QString &error);
    
    /**
     * 角色更新结果处理
     */
    void onUpdateRoleResult(bool success, const RoleInfo &role, const QString &error);
    
    /**
     * 网络错误处理
     */
    void onNetworkError(const QString &error);

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
     * 加载权限列表
     */
    void loadPermissions();
    
    /**
     * 验证输入
     */
    bool validateInput();
    
    /**
     * 显示状态信息
     */
    void showStatus(const QString &message, bool isError = false);
    
    /**
     * 更新权限显示
     */
    void updatePermissionDisplay();
    
    /**
     * 获取选中的权限列表
     */
    QList<int> getSelectedPermissions() const;
    
    /**
     * 设置选中的权限
     */
    void setSelectedPermissions(const QList<int> &permissionIds);

private:
    // UI组件
    QLineEdit *m_nameEdit;
    QTextEdit *m_descriptionEdit;
    QScrollArea *m_permissionScrollArea;
    QWidget *m_permissionWidget;
    QVBoxLayout *m_permissionLayout;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QLabel *m_statusLabel;
    
    // 数据
    ApiManager *m_apiManager;
    RoleInfo m_originalRole;
    QList<PermissionInfo> m_availablePermissions;
    QList<QCheckBox*> m_permissionCheckBoxes;
    bool m_isEditMode;
};

#endif // ROLEEDITOR_H
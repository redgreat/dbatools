#ifndef USEREDITOR_H
#define USEREDITOR_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include "apimanager.h"

class UserEditor : public QDialog
{
    Q_OBJECT

public:
    explicit UserEditor(ApiManager *apiManager, QWidget *parent = nullptr);
    explicit UserEditor(ApiManager *apiManager, const UserInfo &user, QWidget *parent = nullptr);
    ~UserEditor();

    /**
     * 获取编辑后的用户信息
     */
    UserInfo getUserInfo() const;

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
     * 角色列表结果处理
     */
    void onRoleListResult(bool success, const QList<RoleInfo> &roles, const QString &error);
    
    /**
     * 用户注册结果处理
     */
    void onRegisterResult(bool success, const QString &message);
    
    /**
     * 用户更新结果处理
     */
    void onUpdateUserResult(bool success, const UserInfo &user, const QString &error);
    
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
     * 加载角色列表
     */
    void loadRoles();
    
    /**
     * 验证输入数据
     */
    bool validateInput();
    
    /**
     * 显示状态信息
     */
    void showStatus(const QString &message, bool isError = false);
    
    /**
     * 更新用户角色显示
     */
    void updateUserRoles();

private:
    // UI组件
    QLineEdit *m_usernameEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_fullNameEdit;
    QLineEdit *m_passwordEdit;
    QCheckBox *m_isActiveCheck;
    QListWidget *m_rolesList;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QLabel *m_statusLabel;
    
    // 数据
    ApiManager *m_apiManager;
    UserInfo m_originalUser;
    QList<RoleInfo> m_availableRoles;
    bool m_isEditMode;
};

#endif // USEREDITOR_H
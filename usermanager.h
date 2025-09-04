#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QComboBox>
#include <QCheckBox>
#include "apimanager.h"

QT_BEGIN_NAMESPACE
class QTableWidget;
class QPushButton;
class QLineEdit;
class QLabel;
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE

class UserManager : public QWidget
{
    Q_OBJECT

public:
    explicit UserManager(ApiManager *apiManager, QWidget *parent = nullptr);
    ~UserManager();

public slots:
    /**
     * 刷新用户列表
     */
    void refreshUserList();

private slots:
    // 按钮事件
    void onAddUserClicked();
    void onEditUserClicked();
    void onDeleteUserClicked();
    void onRefreshClicked();
    void onSearchClicked();
    
    // API响应处理
    void onUserListResult(bool success, const QList<UserInfo> &users, const QString &error);
    void onUserInfoResult(bool success, const UserInfo &user, const QString &error);
    void onRegisterResult(bool success, const QString &message);
    void onUpdateUserResult(bool success, const UserInfo &user, const QString &error);
    void onNetworkError(const QString &error);
    
    // 表格事件
    void onUserTableSelectionChanged();
    void onUserTableDoubleClicked(int row, int column);

private:
    // UI组件
    QTableWidget *m_userTable;
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
    QPushButton *m_refreshButton;
    QPushButton *m_searchButton;
    QLineEdit *m_searchEdit;
    QLabel *m_statusLabel;
    QLabel *m_totalLabel;
    
    // API管理器
    ApiManager *m_apiManager;
    
    // 数据
    QList<UserInfo> m_users;
    int m_currentPage;
    int m_pageSize;
    int m_totalUsers;
    
    // 初始化UI
    void setupUI();
    
    // 设置样式
    void setupStyles();
    
    // 初始化表格
    void setupTable();
    
    // 更新表格数据
    void updateTable();
    
    // 更新按钮状态
    void updateButtonStates();
    
    // 获取选中的用户
    UserInfo getSelectedUser() const;
    
    // 显示用户编辑对话框
    void showUserEditDialog(const UserInfo &user = UserInfo());
    
    // 显示状态信息
    void showStatus(const QString &message, bool isError = false);
};

#endif // USERMANAGER_H
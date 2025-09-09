#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QSettings>
#include "apimanager.h"

QT_BEGIN_NAMESPACE
class QTabWidget;
class QLabel;
QT_END_NAMESPACE

class StringFormatter;
class UserManager;
class RoleManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 菜单动作
    void onAboutClicked();
    void onExitClicked();
    void onLogoutClicked();
    void onLogoutResult(bool success, const QString &message);
    void onLoginResult(bool success, const QString &message, const QString &token);
    void onTokenExpired();
    void onSettingsClicked();
    
    // 工具相关
    void onAddToolClicked();
    
    // 管理功能
    void onUserManagementClicked();
    void onRoleManagementClicked();
    
    // 工具切换
    void onStringFormatterClicked();
    void showTool(QWidget* tool);

private:
    // UI组件
    QWidget *m_currentTool;
    QLabel *m_statusLabel;
    
    // 菜单和动作
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_logoutAction;
    QAction *m_settingsAction;
    QAction *m_addToolAction;
    QAction *m_stringFormatterAction;
    QAction *m_userManagementAction;
    QAction *m_roleManagementAction;
    
    // 工具页面
    StringFormatter *m_stringFormatter;
    UserManager *m_userManager;
    RoleManager *m_roleManager;
    
    // API管理器
    ApiManager *m_apiManager;
    
    // 初始化UI
    void setupUI();
    
    // 创建菜单
    void createMenus();
    
    // 创建状态栏
    void createStatusBar();
    
    // 初始化工具页面
    void initializeTools();
    
    // 设置样式
    void setupStyles();
};

#endif // MAINWINDOW_H
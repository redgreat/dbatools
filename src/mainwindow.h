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
    void onSettingsClicked();
    
    // 工具相关
    void onAddToolClicked();

private:
    // UI组件
    QTabWidget *m_tabWidget;
    QLabel *m_statusLabel;
    
    // 菜单和动作
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_logoutAction;
    QAction *m_settingsAction;
    QAction *m_addToolAction;
    
    // 工具页面
    StringFormatter *m_stringFormatter;
    
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
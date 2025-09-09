#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QProgressBar>
#include <QCheckBox>
#include <QSettings>
#include "apimanager.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QLabel;
class QProgressBar;
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    
    // 静态MainWindow实例指针
    static class MainWindow *s_mainWindow;

private slots:
    // 登录按钮点击事件
    void onLoginClicked();
    
    // 处理登录结果
    void onLoginResult(bool success, const QString &message, const QString &token);
    
    // 处理网络错误
    void onNetworkError(const QString &error);
    
    // 服务器设置按钮点击事件
    void onServerSettingsClicked();

private:
    // UI组件
    QWidget *m_centralWidget;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginButton;
    QPushButton *m_serverSettingsButton;
    QLabel *m_titleLabel;
    QLabel *m_usernameLabel;
    QLabel *m_passwordLabel;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QCheckBox *m_rememberPasswordCheckBox;
    
    // API管理器
    ApiManager *m_apiManager;
    
    // 设置对象
    QSettings *m_settings;
    
    // 初始化UI
    void setupUI();
    
    // 设置样式
    void setupStyles();
    
    // 验证输入
    bool validateInput();
    
    // 设置登录状态
    void setLoginState(bool isLogging);
    
    // 加载保存的登录信息
    void loadSavedCredentials();
    
    // 保存登录信息
    void saveCredentials();
};

#endif // LOGINWINDOW_H
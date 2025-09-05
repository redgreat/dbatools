#include <QApplication>
#include <QIcon>
#include "loginwindow.h"

/**
 * 应用程序主入口函数
 * 初始化QT应用程序并显示登录窗口
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("DBA Tools");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("redgreat");
    
    // 设置应用程序图标
    app.setWindowIcon(QIcon(":/icon.svg"));
    
    // 创建并显示登录窗口
    LoginWindow loginWindow;
    loginWindow.show();
    
    return app.exec();
}
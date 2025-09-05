#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>

// 用户信息结构
struct UserInfo {
    int id;
    QString username;
    QString email;
    QString fullName;
    bool isActive;
    bool isSuperuser;
    QString createdAt;
    QString lastLogin;
    QStringList roles;
};

// 权限信息结构
struct PermissionInfo {
    int id;
    QString name;
    QString displayName;
    QString description;
    QString resource;
    QString action;
};

// 角色信息结构
struct RoleInfo {
    int id;
    QString name;
    QString displayName;
    QString description;
    bool isActive;
    QString createdAt;
    QString updatedAt;
    QList<PermissionInfo> permissions;
};

class ApiManager : public QObject
{
    Q_OBJECT

public:
    explicit ApiManager(QObject *parent = nullptr);
    
    // 设置API基础URL
    void setBaseUrl(const QString &url);
    
    // 设置认证令牌
    void setAuthToken(const QString &token);
    
    // 认证相关
    void login(const QString &username, const QString &password);
    void logout();
    void registerUser(const QString &username, const QString &email, const QString &password, const QString &fullName = "");
    
    // 用户管理
    void getCurrentUserInfo();
    void getUserList(int skip = 0, int limit = 100);
    void getUserInfo(int userId);
    void updateUser(int userId, const QString &email = "", const QString &fullName = "", bool isActive = true);
    
    // 角色管理
    void getRoleList(int skip = 0, int limit = 100);
    void getRoleInfo(int roleId);
    void createRole(const QString &name, const QString &displayName, const QString &description = "");
    void updateRole(int roleId, const QString &displayName = "", const QString &description = "", bool isActive = true);
    void deleteRole(int roleId);
    void assignRoleToUser(int userId, int roleId);
    void removeRoleFromUser(int userId, int roleId);
    
    // 权限管理
    void getPermissionList(int skip = 0, int limit = 100);
    
    // 格式化字符串
    void formatString(const QString &input, const QString &formatType);
    
    // 获取当前认证状态
    bool isAuthenticated() const { return !m_authToken.isEmpty(); }
    const QString& getAuthToken() const { return m_authToken; }
    
signals:
    // 认证相关信号
    void loginResult(bool success, const QString &message, const QString &token);
    void logoutResult(bool success, const QString &message);
    void registerResult(bool success, const QString &message, const UserInfo &userInfo);
    
    // 用户管理信号
    void currentUserInfoResult(bool success, const UserInfo &userInfo, const QString &error);
    void userListResult(bool success, const QList<UserInfo> &users, const QString &error);
    void userInfoResult(bool success, const UserInfo &userInfo, const QString &error);
    void updateUserResult(bool success, const UserInfo &userInfo, const QString &error);
    
    // 角色管理信号
    void roleListResult(bool success, const QList<RoleInfo> &roles, const QString &error);
    void roleInfoResult(bool success, const RoleInfo &roleInfo, const QString &error);
    void createRoleResult(bool success, const RoleInfo &roleInfo, const QString &error);
    void updateRoleResult(bool success, const RoleInfo &roleInfo, const QString &error);
    void deleteRoleResult(bool success, const QString &message, const QString &error);
    void assignRoleResult(bool success, const QString &message, const QString &error);
    void removeRoleResult(bool success, const QString &message, const QString &error);
    
    // 权限管理信号
    void permissionListResult(bool success, const QList<PermissionInfo> &permissions, const QString &error);
    
    // 格式化字符串相关信号
    void formatStringResult(bool success, const QString &result, const QString &error);
    
    // 网络错误信号
    void networkError(const QString &error);
    
    // Token过期信号
    void tokenExpired();

private slots:
    // 处理网络请求完成
    void onRequestFinished();
    
    // 处理网络错误
    void onNetworkError(QNetworkReply::NetworkError error);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_baseUrl;
    QString m_authToken;
    
    // 发送POST请求
    void sendPostRequest(const QString &endpoint, const QJsonObject &data, const QString &requestType);
    
    // 发送GET请求
    void sendGetRequest(const QString &endpoint, const QString &requestType);
    
    // 发送PUT请求
    void sendPutRequest(const QString &endpoint, const QJsonObject &data, const QString &requestType);
    
    // 发送DELETE请求
    void sendDeleteRequest(const QString &endpoint, const QString &requestType);
    
    // 处理响应数据
    void handleResponse(QNetworkReply *reply, const QString &requestType);
    
    // 数据解析辅助方法
    UserInfo parseUserInfo(const QJsonObject &json);
    RoleInfo parseRoleInfo(const QJsonObject &json);
    PermissionInfo parsePermissionInfo(const QJsonObject &json);
    QList<UserInfo> parseUserList(const QJsonArray &jsonArray);
    QList<RoleInfo> parseRoleList(const QJsonArray &jsonArray);
    QList<PermissionInfo> parsePermissionList(const QJsonArray &jsonArray);
};

#endif // APIMANAGER_H
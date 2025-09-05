#include "apimanager.h"
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>

/**
 * API管理器构造函数
 * 初始化网络访问管理器
 */
ApiManager::ApiManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:8001/api")
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &ApiManager::onRequestFinished);
}

/**
 * 设置API基础URL
 */
void ApiManager::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
}

/**
 * 设置认证令牌
 */
void ApiManager::setAuthToken(const QString &token)
{
    m_authToken = token;
}

/**
 * 用户登录请求
 */
void ApiManager::login(const QString &username, const QString &password)
{
    qDebug() << "[DEBUG] ApiManager::login called with username:" << username;
    qDebug() << "[DEBUG] Password length:" << password.length();
    qDebug() << "[DEBUG] Base URL:" << m_baseUrl;
    
    QJsonObject loginData;
    loginData["username"] = username;
    loginData["password"] = password;
    
    qDebug() << "[DEBUG] Login data prepared:" << loginData;
    qDebug() << "[DEBUG] About to send POST request to /auth/login";
    
    sendPostRequest("/auth/login", loginData, "login");
    qDebug() << "[DEBUG] POST request sent";
}

/**
 * 用户登出
 */
void ApiManager::logout()
{
    sendPostRequest("/auth/logout", QJsonObject(), "logout");
    m_authToken.clear();
}

/**
 * 用户注册
 */
void ApiManager::registerUser(const QString &username, const QString &email, const QString &password, const QString &fullName)
{
    QJsonObject data;
    data["username"] = username;
    data["email"] = email;
    data["password"] = password;
    if (!fullName.isEmpty()) {
        data["full_name"] = fullName;
    }
    
    sendPostRequest("/auth/register", data, "register");
}

/**
 * 获取当前用户信息
 */
void ApiManager::getCurrentUserInfo()
{
    sendGetRequest("/users/me", "current_user");
}

/**
 * 获取用户列表
 */
void ApiManager::getUserList(int skip, int limit)
{
    QString endpoint = QString("/users/?skip=%1&limit=%2").arg(skip).arg(limit);
    sendGetRequest(endpoint, "user_list");
}

/**
 * 获取指定用户信息
 */
void ApiManager::getUserInfo(int userId)
{
    QString endpoint = QString("/users/%1").arg(userId);
    sendGetRequest(endpoint, "user_info");
}

/**
 * 更新用户信息
 */
void ApiManager::updateUser(int userId, const QString &email, const QString &fullName, bool isActive)
{
    QJsonObject data;
    if (!email.isEmpty()) {
        data["email"] = email;
    }
    if (!fullName.isEmpty()) {
        data["full_name"] = fullName;
    }
    data["is_active"] = isActive;
    
    QString endpoint = QString("/users/%1").arg(userId);
    sendPutRequest(endpoint, data, "update_user");
}

/**
 * 获取角色列表
 */
void ApiManager::getRoleList(int skip, int limit)
{
    QString endpoint = QString("/roles/?skip=%1&limit=%2").arg(skip).arg(limit);
    sendGetRequest(endpoint, "role_list");
}

/**
 * 获取指定角色信息
 */
void ApiManager::getRoleInfo(int roleId)
{
    QString endpoint = QString("/roles/%1").arg(roleId);
    sendGetRequest(endpoint, "role_info");
}

/**
 * 创建角色
 */
void ApiManager::createRole(const QString &name, const QString &displayName, const QString &description)
{
    QJsonObject data;
    data["name"] = name;
    data["display_name"] = displayName;
    if (!description.isEmpty()) {
        data["description"] = description;
    }
    
    sendPostRequest("/roles/", data, "create_role");
}

/**
 * 更新角色信息
 */
void ApiManager::updateRole(int roleId, const QString &displayName, const QString &description, bool isActive)
{
    QJsonObject data;
    if (!displayName.isEmpty()) {
        data["display_name"] = displayName;
    }
    if (!description.isEmpty()) {
        data["description"] = description;
    }
    data["is_active"] = isActive;
    
    QString endpoint = QString("/roles/%1").arg(roleId);
    sendPutRequest(endpoint, data, "update_role");
}

/**
 * 删除角色
 */
void ApiManager::deleteRole(int roleId)
{
    QString endpoint = QString("/roles/%1").arg(roleId);
    sendDeleteRequest(endpoint, "delete_role");
}

/**
 * 为用户分配角色
 */
void ApiManager::assignRoleToUser(int userId, int roleId)
{
    QString endpoint = QString("/roles/users/%1/assign/%2").arg(userId).arg(roleId);
    sendPostRequest(endpoint, QJsonObject(), "assign_role");
}

/**
 * 移除用户角色
 */
void ApiManager::removeRoleFromUser(int userId, int roleId)
{
    QString endpoint = QString("/roles/users/%1/remove/%2").arg(userId).arg(roleId);
    sendDeleteRequest(endpoint, "remove_role");
}

/**
 * 格式化字符串请求
 */
void ApiManager::formatString(const QString &input, const QString &formatType)
{
    QJsonObject data;
    data["input"] = input;
    data["type"] = formatType;
    
    sendPostRequest("/tools/format-string", data, "format");
}

/**
 * 获取权限列表
 */
void ApiManager::getPermissionList(int skip, int limit)
{
    QString endpoint = QString("/permissions/?skip=%1&limit=%2").arg(skip).arg(limit);
    sendGetRequest(endpoint, "permission_list");
}

/**
 * 发送POST请求
 */
void ApiManager::sendPostRequest(const QString &endpoint, const QJsonObject &data, const QString &requestType)
{
    QUrl url(m_baseUrl + endpoint);
    qDebug() << "[DEBUG] sendPostRequest - Full URL:" << url.toString();
    qDebug() << "[DEBUG] sendPostRequest - Request type:" << requestType;
    qDebug() << "[DEBUG] sendPostRequest - Data:" << data;
    
    QNetworkRequest request(url);
    
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 如果有认证令牌，添加到请求头
    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_authToken).toUtf8());
        qDebug() << "[DEBUG] sendPostRequest - Added auth token";
    }
    
    // 设置请求类型标识
    request.setAttribute(QNetworkRequest::User, requestType);
    
    // 转换数据为JSON
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();
    qDebug() << "[DEBUG] sendPostRequest - JSON data:" << jsonData;
    
    // 发送请求
    QNetworkReply *reply = m_networkManager->post(request, jsonData);
    qDebug() << "[DEBUG] sendPostRequest - Request sent, reply object created";
    
    // 连接信号
    connect(reply, &QNetworkReply::finished, [this, reply, requestType]() {
        handleResponse(reply, requestType);
    });
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &ApiManager::onNetworkError);
    qDebug() << "[DEBUG] sendPostRequest - Signals connected";
}

/**
 * 发送GET请求
 */
void ApiManager::sendGetRequest(const QString &endpoint, const QString &requestType)
{
    QUrl url(m_baseUrl + endpoint);
    QNetworkRequest request(url);
    
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 如果有认证令牌，添加到请求头
    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_authToken).toUtf8());
    }
    
    // 设置请求类型标识
    request.setAttribute(QNetworkRequest::User, requestType);
    
    // 发送请求
    QNetworkReply *reply = m_networkManager->get(request);
    qDebug() << "[DEBUG] sendGetRequest - Request sent";
    
    // 连接信号
    connect(reply, &QNetworkReply::finished, [this, reply, requestType]() {
        handleResponse(reply, requestType);
    });
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &ApiManager::onNetworkError);
}

/**
 * 发送PUT请求
 */
void ApiManager::sendPutRequest(const QString &endpoint, const QJsonObject &data, const QString &requestType)
{
    QUrl url(m_baseUrl + endpoint);
    QNetworkRequest request(url);
    
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 如果有认证令牌，添加到请求头
    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_authToken).toUtf8());
    }
    
    // 设置请求类型标识
    request.setAttribute(QNetworkRequest::User, requestType);
    
    // 转换数据为JSON
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();
    
    // 发送请求
    QNetworkReply *reply = m_networkManager->put(request, jsonData);
    
    // 连接信号
    connect(reply, &QNetworkReply::finished, [this, reply, requestType]() {
        handleResponse(reply, requestType);
    });
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &ApiManager::onNetworkError);
}

/**
 * 发送DELETE请求
 */
void ApiManager::sendDeleteRequest(const QString &endpoint, const QString &requestType)
{
    QUrl url(m_baseUrl + endpoint);
    QNetworkRequest request(url);
    
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 如果有认证令牌，添加到请求头
    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_authToken).toUtf8());
    }
    
    // 设置请求类型标识
    request.setAttribute(QNetworkRequest::User, requestType);
    
    // 发送请求
    QNetworkReply *reply = m_networkManager->deleteResource(request);
    
    // 连接信号
    connect(reply, &QNetworkReply::finished, [this, reply, requestType]() {
        handleResponse(reply, requestType);
    });
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &ApiManager::onNetworkError);
}

/**
 * 处理网络请求完成
 */
void ApiManager::onRequestFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }
    
    QString requestType = reply->request().attribute(QNetworkRequest::User).toString();
    handleResponse(reply, requestType);
    
    reply->deleteLater();
}

/**
 * 处理网络错误
 */
void ApiManager::onNetworkError(QNetworkReply::NetworkError error)
{
    qDebug() << "[DEBUG] onNetworkError called with error code:" << error;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        QString errorString = reply->errorString();
        qDebug() << "[DEBUG] Network error string:" << errorString;
        qDebug() << "[DEBUG] Request URL:" << reply->url().toString();
        qDebug() << "[DEBUG] HTTP status code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        emit networkError(errorString);
        qDebug() << "[DEBUG] networkError signal emitted";
    } else {
        qDebug() << "[DEBUG] No reply object found in onNetworkError";
    }
}

/**
 * 处理响应数据
 */
void ApiManager::handleResponse(QNetworkReply *reply, const QString &requestType)
{
    QByteArray responseData = reply->readAll();
    
    // 检查HTTP状态码
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    // 检查Token是否过期（401状态码）
    if (statusCode == 401) {
        qDebug() << "[DEBUG] Token expired (401), clearing auth token and emitting tokenExpired signal";
        m_authToken.clear();
        emit tokenExpired();
        return;
    }
    
    // 解析JSON响应
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        emit networkError("JSON解析错误: " + parseError.errorString());
        return;
    }
    
    QJsonObject response = doc.object();
    bool success = (statusCode >= 200 && statusCode < 300);
    
    if (requestType == "login") {
        qDebug() << "[DEBUG] Processing login response";
        qDebug() << "[DEBUG] Raw response data:" << responseData;
        qDebug() << "[DEBUG] HTTP status code:" << statusCode;
        qDebug() << "[DEBUG] Success flag:" << success;
        qDebug() << "[DEBUG] JSON response object:" << response;
        
        QString message = response["message"].toString();
        QString token = response["access_token"].toString();
        
        qDebug() << "[DEBUG] Extracted message:" << message;
        qDebug() << "[DEBUG] Extracted token:" << token;
        
        // 根据API规范，登录成功时应该返回Token对象
        // 检查是否有token_type字段来确认这是正确的登录响应
        QString tokenType = response["token_type"].toString();
        qDebug() << "[DEBUG] Token type:" << tokenType;
        
        if (tokenType.isEmpty() && success) {
            // 如果没有token_type但有access_token，仍然认为是有效响应
            tokenType = "bearer";
            qDebug() << "[DEBUG] Using default token type: bearer";
        }
        
        // 如果没有message字段，使用默认消息
        if (message.isEmpty()) {
            message = success ? "登录成功" : "登录失败";
            qDebug() << "[DEBUG] Using default message:" << message;
        }
        
        if (success && !token.isEmpty()) {
            m_authToken = token;
            qDebug() << "[DEBUG] Token saved to m_authToken:" << m_authToken;
        }
        
        qDebug() << "[DEBUG] About to emit loginResult with:";
        qDebug() << "[DEBUG]   success:" << success;
        qDebug() << "[DEBUG]   message:" << message;
        qDebug() << "[DEBUG]   token:" << token;
        
        emit loginResult(success, message, token);
        qDebug() << "[DEBUG] loginResult signal emitted";
    }
    else if (requestType == "logout") {
        QString message = success ? "退出登录成功" : "退出登录失败";
        if (success) {
            m_authToken.clear();
        }
        emit logoutResult(success, message);
    }
    else if (requestType == "register") {
        QString message = response["message"].toString();
        if (success) {
            UserInfo user = parseUserInfo(response);
            emit registerResult(success, message, user);
        } else {
            emit registerResult(success, message, UserInfo());
        }
    }
    else if (requestType == "current_user") {
        if (success) {
            UserInfo user = parseUserInfo(response);
            emit currentUserInfoResult(true, user, "");
        } else {
            emit currentUserInfoResult(false, UserInfo(), response["detail"].toString());
        }
    }
    else if (requestType == "user_list") {
        qDebug() << "[DEBUG] Processing user_list response";
        qDebug() << "[DEBUG] Raw response data:" << responseData;
        qDebug() << "[DEBUG] HTTP status code:" << statusCode;
        qDebug() << "[DEBUG] Success flag:" << success;
        qDebug() << "[DEBUG] JSON response object:" << response;
        
        if (success) {
            // 检查响应是否是数组格式（直接返回用户列表）还是对象格式（包含items字段）
            QJsonArray usersArray;
            
            // 重新解析响应数据，因为可能是数组而不是对象
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
            
            if (parseError.error == QJsonParseError::NoError) {
                if (doc.isArray()) {
                    // 直接返回的是用户数组
                    usersArray = doc.array();
                    qDebug() << "[DEBUG] Parsed as array, items count:" << usersArray.size();
                } else if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("items")) {
                        // 标准格式：包含items字段的对象
                        usersArray = obj["items"].toArray();
                        qDebug() << "[DEBUG] Parsed as object with items, items count:" << usersArray.size();
                    }
                }
            }
            
            QList<UserInfo> users = parseUserList(usersArray);
            qDebug() << "[DEBUG] Parsed users count:" << users.size();
            emit userListResult(true, users, "");
        } else {
            emit userListResult(false, QList<UserInfo>(), response["detail"].toString());
        }
    }
    else if (requestType == "user_info") {
        if (success) {
            UserInfo user = parseUserInfo(response);
            emit userInfoResult(true, user, "");
        } else {
            emit userInfoResult(false, UserInfo(), response["detail"].toString());
        }
    }
    else if (requestType == "update_user") {
        if (success) {
            UserInfo user = parseUserInfo(response);
            emit updateUserResult(true, user, "");
        } else {
            emit updateUserResult(false, UserInfo(), response["detail"].toString());
        }
    }
    else if (requestType == "role_list") {
        qDebug() << "[DEBUG] Processing role_list response";
        qDebug() << "[DEBUG] Raw response data:" << responseData;
        qDebug() << "[DEBUG] HTTP status code:" << statusCode;
        qDebug() << "[DEBUG] Success flag:" << success;
        qDebug() << "[DEBUG] JSON response object:" << response;
        
        if (success) {
            QJsonArray rolesArray;
            if (response.contains("items") && response["items"].isArray()) {
                // 如果响应包含 items 字段
                rolesArray = response["items"].toArray();
                qDebug() << "[DEBUG] Role list items array:" << rolesArray;
            } else if (doc.isArray()) {
                // 如果响应直接是数组
                rolesArray = doc.array();
                qDebug() << "[DEBUG] Role list direct array:" << rolesArray;
            }
            
            qDebug() << "[DEBUG] Role list items count:" << rolesArray.size();
            QList<RoleInfo> roles = parseRoleList(rolesArray);
            qDebug() << "[DEBUG] Parsed roles count:" << roles.size();
            emit roleListResult(true, roles, "");
        } else {
            qDebug() << "[DEBUG] Role list request failed";
            emit roleListResult(false, QList<RoleInfo>(), response["detail"].toString());
        }
    }
    else if (requestType == "role_info") {
        if (success) {
            RoleInfo role = parseRoleInfo(response);
            emit roleInfoResult(true, role, "");
        } else {
            emit roleInfoResult(false, RoleInfo(), response["detail"].toString());
        }
    }
    else if (requestType == "create_role") {
        if (success) {
            RoleInfo role = parseRoleInfo(response);
            emit createRoleResult(true, role, "");
        } else {
            emit createRoleResult(false, RoleInfo(), response["detail"].toString());
        }
    }
    else if (requestType == "update_role") {
        if (success) {
            RoleInfo role = parseRoleInfo(response);
            emit updateRoleResult(true, role, "");
        } else {
            emit updateRoleResult(false, RoleInfo(), response["detail"].toString());
        }
    }
    else if (requestType == "delete_role") {
        QString message = success ? "删除角色成功" : "";
        QString error = success ? "" : response["detail"].toString();
        emit deleteRoleResult(success, message, error);
    }
    else if (requestType == "assign_role") {
        QString message = success ? "分配角色成功" : "";
        QString error = success ? "" : response["detail"].toString();
        emit assignRoleResult(success, message, error);
    }
    else if (requestType == "remove_role") {
        QString message = success ? "移除角色成功" : "";
        QString error = success ? "" : response["detail"].toString();
        emit removeRoleResult(success, message, error);
    }
    else if (requestType == "permission_list") {
        if (success) {
            QList<PermissionInfo> permissions = parsePermissionList(response["items"].toArray());
            emit permissionListResult(true, permissions, "");
        } else {
            emit permissionListResult(false, QList<PermissionInfo>(), response["detail"].toString());
        }
    }
    else if (requestType == "format") {
        QString result = response["result"].toString();
        QString error = response["error"].toString();
        
        emit formatStringResult(success, result, error);
    }
}



/**
 * 解析用户信息
 */
UserInfo ApiManager::parseUserInfo(const QJsonObject &json)
{
    UserInfo user;
    user.id = json["id"].toInt();
    user.username = json["username"].toString();
    user.email = json["email"].toString();
    user.fullName = json["full_name"].toString();
    user.isActive = json["is_active"].toBool();
    user.createdAt = json["created_at"].toString();
    user.lastLogin = json["last_login"].toString();
    
    // 解析角色列表
    QJsonArray rolesArray = json["roles"].toArray();
    for (const QJsonValue &roleValue : rolesArray) {
        if (roleValue.isString()) {
            user.roles.append(roleValue.toString());
        } else if (roleValue.isObject()) {
            QJsonObject roleObj = roleValue.toObject();
            user.roles.append(roleObj["name"].toString());
        }
    }
    
    return user;
}

/**
 * 解析用户列表
 */
QList<UserInfo> ApiManager::parseUserList(const QJsonArray &jsonArray)
{
    QList<UserInfo> users;
    for (const QJsonValue &value : jsonArray) {
        users.append(parseUserInfo(value.toObject()));
    }
    return users;
}

/**
 * 解析角色信息
 */
RoleInfo ApiManager::parseRoleInfo(const QJsonObject &json)
{
    RoleInfo role;
    role.id = json["id"].toInt();
    role.name = json["name"].toString();
    role.displayName = json["display_name"].toString();
    role.description = json["description"].toString();
    role.isActive = json["is_active"].toBool();
    role.createdAt = json["created_at"].toString();
    role.updatedAt = json["updated_at"].toString();
    
    // 解析权限列表
    QJsonArray permissionsArray = json["permissions"].toArray();
    for (const QJsonValue &value : permissionsArray) {
        QJsonObject permObj = value.toObject();
        PermissionInfo permission;
        permission.id = permObj["id"].toInt();
        permission.name = permObj["name"].toString();
        permission.displayName = permObj["display_name"].toString();
        permission.description = permObj["description"].toString();
        permission.resource = permObj["resource"].toString();
        permission.action = permObj["action"].toString();
        role.permissions.append(permission);
    }
    
    return role;
}

/**
 * 解析角色列表
 */
QList<RoleInfo> ApiManager::parseRoleList(const QJsonArray &jsonArray)
{
    QList<RoleInfo> roles;
    for (const QJsonValue &value : jsonArray) {
        roles.append(parseRoleInfo(value.toObject()));
    }
    return roles;
}

/**
 * 解析权限信息
 */
PermissionInfo ApiManager::parsePermissionInfo(const QJsonObject &json)
{
    PermissionInfo permission;
    permission.id = json["id"].toInt();
    permission.name = json["name"].toString();
    permission.displayName = json["display_name"].toString();
    permission.description = json["description"].toString();
    permission.resource = json["resource"].toString();
    permission.action = json["action"].toString();
    return permission;
}

/**
 * 解析权限列表
 */
QList<PermissionInfo> ApiManager::parsePermissionList(const QJsonArray &jsonArray)
{
    QList<PermissionInfo> permissions;
    for (const QJsonValue &value : jsonArray) {
        permissions.append(parsePermissionInfo(value.toObject()));
    }
    return permissions;
}
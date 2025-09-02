#include "apimanager.h"
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QDebug>

/**
 * API管理器构造函数
 * 初始化网络访问管理器
 */
ApiManager::ApiManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:8080/api")
{
    // 连接网络管理器的信号
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
 * 用户登录请求
 */
void ApiManager::login(const QString &username, const QString &password)
{
    QJsonObject loginData;
    loginData["username"] = username;
    loginData["password"] = password;
    
    sendPostRequest("/auth/login", loginData, "login");
}

/**
 * 格式化字符串请求
 */
void ApiManager::formatString(const QString &input, const QString &formatType)
{
    QJsonObject formatData;
    formatData["input"] = input;
    formatData["type"] = formatType;
    
    sendPostRequest("/tools/format-string", formatData, "format");
}

/**
 * 发送POST请求
 */
void ApiManager::sendPostRequest(const QString &endpoint, const QJsonObject &data, const QString &requestType)
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
    QNetworkReply *reply = m_networkManager->post(request, jsonData);
    
    // 连接错误信号
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
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        QString errorString = reply->errorString();
        emit networkError(errorString);
        qDebug() << "Network error:" << errorString;
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
    
    // 解析JSON响应
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        emit networkError("JSON解析错误: " + parseError.errorString());
        return;
    }
    
    QJsonObject response = doc.object();
    
    if (requestType == "login") {
        bool success = (statusCode == 200) && response["success"].toBool();
        QString message = response["message"].toString();
        QString token = response["token"].toString();
        
        if (success && !token.isEmpty()) {
            m_authToken = token;
        }
        
        emit loginResult(success, message, token);
    }
    else if (requestType == "format") {
        bool success = (statusCode == 200) && response["success"].toBool();
        QString result = response["result"].toString();
        QString error = response["error"].toString();
        
        emit formatStringResult(success, result, error);
    }
}
#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

class ApiManager : public QObject
{
    Q_OBJECT

public:
    explicit ApiManager(QObject *parent = nullptr);
    
    // 设置API基础URL
    void setBaseUrl(const QString &url);
    
    // 用户登录
    void login(const QString &username, const QString &password);
    
    // 格式化字符串
    void formatString(const QString &input, const QString &formatType);
    
signals:
    // 登录结果信号
    void loginResult(bool success, const QString &message, const QString &token);
    
    // 格式化字符串结果信号
    void formatStringResult(bool success, const QString &result, const QString &error);
    
    // 网络错误信号
    void networkError(const QString &error);

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
    
    // 处理响应数据
    void handleResponse(QNetworkReply *reply, const QString &requestType);
};

#endif // APIMANAGER_H
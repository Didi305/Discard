#pragma once
#include <QJsonObject>
#include <QString>

class Client;

// Handles account requests and pending sign-in state. Manager loads application data.
class AccountAuthentication {
  public:
    enum class SignInStatus { Failed, SecondFactorRequired, Authenticated };

    struct SignInResult {
        SignInStatus status = SignInStatus::Failed;
        QJsonObject user;
    };

    void setClient(Client *client);
    QString lastError() const;
    SignInResult authenticate(const QString &username, const QString &password);
    bool secondFactorRequired() const;
    SignInResult verifySecondFactor(const QString &code);
    void cancelAuthentication();
    QJsonObject securityRequest(const QString &operation, const QJsonObject &arguments = {});
    bool checkUsernameLength(const QString &username);
    bool createUser(const QString &username, const QString &password, const QString &nickname);
    bool checkUsernameAvailability(const QString &username);
    bool checkNicknameAvailability(const QString &nickname);

  private:
    Client *client = nullptr;
    bool awaitingSecondFactor = false;
    QString requestError;

    QJsonObject call(const QString &operation, const QJsonObject &arguments = {});
    SignInResult completeSignIn(const QJsonObject &result);
};

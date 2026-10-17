#include "accountauthentication.h"
#include "client.h"
#include "utils/errormessages.h"
#include "utils/inputlimits.h"
#include <QCoreApplication>
#include <QStringList>

void AccountAuthentication::setClient(Client *client) {
    this->client = client;
}

QString AccountAuthentication::lastError() const {
    return requestError;
}

QJsonObject AccountAuthentication::call(const QString &operation, const QJsonObject &arguments) {
    requestError.clear();
    if (!client) {
        requestError = QCoreApplication::translate("Manager", "Client is not initialized.");
        return {};
    }
    const auto reply = client->request(operation, arguments);
    if (reply["ok"].toBool()) {
        return reply;
    }
    requestError =
        ErrorMessages::message(operation, reply["error"].toString(), client->errorString());
    return reply;
}

bool AccountAuthentication::checkUsernameLength(const QString &username) {
    requestError.clear();
    if (username.size() > InputLimits::MaxUsernameLength) {
        requestError = ErrorMessages::message({}, "username_too_long", {});
        return false;
    }
    return true;
}

AccountAuthentication::SignInResult AccountAuthentication::authenticate(const QString &username,
                                                                        const QString &password) {
    awaitingSecondFactor = false;
    if (!checkUsernameLength(username)) {
        return {};
    }
    const auto reply = call("login", {{"username", username}, {"password", password}});
    if (!reply["ok"].toBool()) {
        return {};
    }
    const auto result = reply["result"].toObject();
    if (result["mfa_required"].toBool()) {
        awaitingSecondFactor = true;
        return {SignInStatus::SecondFactorRequired, {}};
    }
    return completeSignIn(result);
}

bool AccountAuthentication::secondFactorRequired() const {
    return awaitingSecondFactor;
}

AccountAuthentication::SignInResult AccountAuthentication::verifySecondFactor(const QString &code) {
    if (!awaitingSecondFactor) {
        requestError = QCoreApplication::translate(
            "Manager", "Start sign-in again with your username and password.");
        return {};
    }
    const auto reply = call("login_verify", {{"code", code.trimmed()}});
    if (!reply["ok"].toBool()) {
        return {};
    }
    return completeSignIn(reply["result"].toObject());
}

AccountAuthentication::SignInResult
AccountAuthentication::completeSignIn(const QJsonObject &result) {
    const auto userObject = result["user"].toObject();
    if (userObject["id"].toInt() <= 0) {
        requestError = QCoreApplication::translate(
            "Manager", "The server returned an incomplete sign-in response.");
        return {};
    }
    awaitingSecondFactor = false;
    return {SignInStatus::Authenticated, userObject};
}

void AccountAuthentication::cancelAuthentication() {
    awaitingSecondFactor = false;
    if (client) {
        client->disconnectFromServer();
    }
}

QJsonObject AccountAuthentication::securityRequest(const QString &operation,
                                                   const QJsonObject &arguments) {
    static const QStringList allowedOperations = {
        "mfa_status", "mfa_setup_begin", "mfa_setup_confirm", "mfa_setup_cancel", "mfa_disable"};
    if (!allowedOperations.contains(operation)) {
        requestError =
            QCoreApplication::translate("Manager", "Unknown account security operation.");
        return {};
    }
    return call(operation, arguments);
}

bool AccountAuthentication::createUser(const QString &username,
                                       const QString &password,
                                       const QString &nickname) {
    if (!checkUsernameLength(username)) {
        return false;
    }
    const auto reply =
        call("register", {{"username", username}, {"password", password}, {"nickname", nickname}});
    return reply["ok"].toBool();
}

bool AccountAuthentication::checkUsernameAvailability(const QString &username) {
    if (!checkUsernameLength(username)) {
        return false;
    }
    const auto reply = call("available", {{"field", "username"}, {"value", username}});
    return reply["ok"].toBool() && reply["result"].toObject()["available"].toBool();
}

bool AccountAuthentication::checkNicknameAvailability(const QString &nickname) {
    const auto reply = call("available", {{"field", "nickname"}, {"value", nickname}});
    return reply["ok"].toBool() && reply["result"].toObject()["available"].toBool();
}

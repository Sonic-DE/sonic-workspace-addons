/*
    SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSet>
#include <QTemporaryDir>
#include <QTest>
#include <QXmlStreamReader>

#include "commandrunner.h"

class SonicMenuTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void commandsRunThroughTheShell();
    void emptyCommandsAreRefused();
    void defaultMenuItemsAreWellFormed();

private:
    static QString readFile(const QString &path);
    static QSet<QString> knownActions();
    static void checkItems(const QJsonArray &items, const QSet<QString> &actions, const QString &where);
};

QString SonicMenuTest::readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    return QString::fromUtf8(file.readAll());
}

// The action names SessionActions.qml maps, read from its actions table
QSet<QString> SonicMenuTest::knownActions()
{
    QSet<QString> names;
    const QString qml = readFile(QStringLiteral(SONICMENU_SOURCE_DIR "/SessionActions.qml"));
    static const QRegularExpression entry(QStringLiteral(R"re("(\w+)":\s*\{\s*slot:)re"));
    for (auto it = entry.globalMatch(qml); it.hasNext();) {
        names.insert(it.next().captured(1));
    }
    return names;
}

void SonicMenuTest::commandsRunThroughTheShell()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString marker = dir.filePath(QStringLiteral("ran"));
    // Redirection only works if the command really goes through a shell
    QVERIFY(CommandRunner::run(QStringLiteral("echo 'one two' > '%1'").arg(marker)));
    QTRY_COMPARE(readFile(marker), QStringLiteral("one two\n"));
}

void SonicMenuTest::emptyCommandsAreRefused()
{
    QVERIFY(!CommandRunner::run(QString()));
    QVERIFY(!CommandRunner::run(QStringLiteral("   ")));
}

void SonicMenuTest::checkItems(const QJsonArray &items, const QSet<QString> &actions, const QString &where)
{
    for (qsizetype i = 0; i < items.size(); ++i) {
        const QString at = where + QLatin1Char('/') + QString::number(i);
        QVERIFY2(items.at(i).isObject(), qPrintable(at));
        const QJsonObject item = items.at(i).toObject();
        if (item.value(QLatin1String("separator")).toBool()) {
            continue;
        }
        QVERIFY2(!item.value(QLatin1String("name")).toString().isEmpty(), qPrintable(at + QStringLiteral(": no name")));

        // Exactly one of: a submenu, a session action, a command
        const bool hasSubmenu = item.contains(QLatin1String("submenu"));
        const bool hasAction = item.contains(QLatin1String("action"));
        const bool hasCommand = !item.value(QLatin1String("command")).toString().isEmpty();
        QVERIFY2(int(hasSubmenu) + int(hasAction) + int(hasCommand) == 1, qPrintable(at + QStringLiteral(": needs exactly one of submenu, action, command")));

        if (hasAction) {
            const QString action = item.value(QLatin1String("action")).toString();
            QVERIFY2(actions.contains(action), qPrintable(at + QStringLiteral(": unknown session action ") + action));
        }
        if (hasSubmenu) {
            checkItems(item.value(QLatin1String("submenu")).toArray(), actions, at);
        }
    }
}

void SonicMenuTest::defaultMenuItemsAreWellFormed()
{
    QXmlStreamReader kcfg(readFile(QStringLiteral(SONICMENU_SOURCE_DIR "/main.xml")));
    QString json;
    bool inMenuJson = false;
    while (!kcfg.atEnd()) {
        kcfg.readNext();
        if (!kcfg.isStartElement()) {
            continue;
        }
        if (kcfg.name() == QLatin1String("entry")) {
            inMenuJson = kcfg.attributes().value(QLatin1String("name")) == QLatin1String("menuJson");
        } else if (inMenuJson && kcfg.name() == QLatin1String("default")) {
            json = kcfg.readElementText();
        }
    }
    QVERIFY2(!kcfg.hasError(), qPrintable(kcfg.errorString()));
    QVERIFY(!json.isEmpty());

    QJsonParseError error;
    const QJsonDocument menu = QJsonDocument::fromJson(json.toUtf8(), &error);
    QVERIFY2(error.error == QJsonParseError::NoError, qPrintable(error.errorString()));
    QVERIFY(menu.isArray());
    QVERIFY(!menu.array().isEmpty());

    const QSet<QString> actions = knownActions();
    QVERIFY(!actions.isEmpty());
    checkItems(menu.array(), actions, QString());
}

QTEST_GUILESS_MAIN(SonicMenuTest)

#include "sonicmenutest.moc"

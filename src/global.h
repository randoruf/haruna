/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <KSharedConfig>
#include <QStandardPaths>

class QCommandLineParser;

class Global : public QObject
{
    Q_OBJECT
public:
    static Global *instance();

    enum ConfigFile { Main, CustomCommands };

    const QString systemConfigPath();
    const QString appConfigDirPath();
    const QString appConfigFilePath(ConfigFile configFile = ConfigFile::Main);

Q_SIGNALS:
    void error(const QString &message);

private:
    Global();
    ~Global() = default;

    Global(const Global &) = delete;
    Global &operator=(const Global &) = delete;
    Global(Global &&) = delete;
    Global &operator=(Global &&) = delete;

    KSharedConfig::Ptr m_config;
    KSharedConfig::Ptr m_ccConfig;
};

#endif // GLOBALS_H

/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "global.h"

#include <QFileInfo>

Global *Global::instance()
{
    static Global g;
    return &g;
}

Global::Global()
    : m_config(KSharedConfig::openConfig("haruna/haruna.conf"))
    , m_ccConfig(KSharedConfig::openConfig("haruna/haruna-custom-commands.conf"))
{
}

const QString Global::systemConfigPath()
{
    return QStandardPaths::writableLocation(m_config->locationType()).append("/");
}

const QString Global::appConfigDirPath()
{
    QFileInfo configFile(QString(systemConfigPath()).append(m_config->name()));
    return configFile.absolutePath();
}

const QString Global::appConfigFilePath(ConfigFile configFile)
{
    switch (configFile) {
    case ConfigFile::Main:
        return QString(systemConfigPath()).append(m_config->name());
    case ConfigFile::CustomCommands:
        return QString(systemConfigPath()).append(m_ccConfig->name());
    default:
        return QString();
    }
}

#include "moc_global.cpp"

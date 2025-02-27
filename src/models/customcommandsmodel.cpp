/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "customcommandsmodel.h"
#include "actionsmodel.h"

#include <KConfigGroup>
#include <global.h>

CustomCommandsModel::CustomCommandsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &QAbstractListModel::rowsMoved, this, [=]() {
        for (int i = 0; i < m_customCommands.size(); ++i) {
            auto configGroup = m_customCommandsConfig->group(m_customCommands[i]->commandId);
            configGroup.writeEntry("Order", i);
            configGroup.sync();
        }
    });
}

int CustomCommandsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_customCommands.size();
}

QVariant CustomCommandsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Command *command = m_customCommands[index.row()];

    switch (role) {
    case CommandIdRole:
        return QVariant(command->commandId);
    case CommandRole:
        return QVariant(command->command);
    case OsdMessageRole:
        return QVariant(command->osdMessage);
    case TypeRole:
        return QVariant(command->type);
    case ShortcutRole:
        return QVariant(command->shortcut);
    case SetOnStartupRole:
        return QVariant(command->setOnStartup);
    }

    return QVariant();
}

QHash<int, QByteArray> CustomCommandsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[CommandIdRole] = "commandId";
    roles[OsdMessageRole] = "osdMessage";
    roles[CommandRole] = "command";
    roles[TypeRole] = "type";
    roles[ShortcutRole] = "shortcut";
    roles[SetOnStartupRole] = "setOnStartup";
    return roles;
}

void CustomCommandsModel::init()
{
    connect(appActionsModel(), &ActionsModel::shortcutChanged, this, [=](const QString &name, const QString &shortcut) {
        for (int i{0}; i < m_customCommands.count(); ++i) {
            if (m_customCommands[i]->commandId == name) {
                m_customCommands[i]->shortcut = shortcut;
                Q_EMIT dataChanged(index(i, 0), index(i, 0));
                return;
            }
        }
    });

    QString ccConfig = Global::instance()->appConfigFilePath(Global::ConfigFile::CustomCommands);
    m_customCommandsConfig = KSharedConfig::openConfig(ccConfig, KConfig::SimpleConfig);
    QStringList groups = m_customCommandsConfig->groupList();

    beginInsertRows(QModelIndex(), 0, groups.size());
    for (const QString &groupName : qAsConst((groups))) {
        auto configGroup = m_customCommandsConfig->group(groupName);
        auto c = new Command();
        c->commandId = groupName;
        c->command = configGroup.readEntry("Command", QString());
        c->osdMessage = configGroup.readEntry("OsdMessage", QString());
        c->type = configGroup.readEntry("Type", QString());
        c->shortcut = appActionsModel()->getShortcut(c->commandId, QString());
        c->order = configGroup.readEntry("Order", 0);
        c->setOnStartup = configGroup.readEntry("SetOnStartup", true);
        m_customCommands << c;
        if (c->type == QStringLiteral("shortcut")) {
            Action action;
            action.name = c->commandId;
            action.text = c->command;
            action.description = c->osdMessage;
            action.shortcut = c->shortcut;
            action.type = QStringLiteral("CustomAction");
            appActionsModel()->appendCustomAction(action);
        }
    }

    std::sort(m_customCommands.begin(), m_customCommands.end(), [=](Command *c1, Command *c2) {
        return c1->order < c2->order;
    });

    endInsertRows();
}

void CustomCommandsModel::moveRows(int oldIndex, int newIndex)
{
    if (oldIndex < newIndex) {
        beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), newIndex + 1);
    } else {
        beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), newIndex);
    }
    Command *c = m_customCommands.takeAt(oldIndex);
    m_customCommands.insert(newIndex, c);
    endMoveRows();
}

void CustomCommandsModel::saveCustomCommand(const QString &command, const QString &osdMessage, const QString &type)
{
    int counter = m_customCommandsConfig->group(QString()).readEntry("Counter", 0);
    const QString &groupName = QString("Command_%1").arg(counter);

    if (m_customCommandsConfig->group(groupName).exists()) {
        return;
    }

    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Command"), command);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("OsdMessage"), osdMessage);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Type"), type);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Order"), rowCount());
    m_customCommandsConfig->group(QString()).writeEntry(QStringLiteral("Counter"), counter + 1);
    m_customCommandsConfig->sync();

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    auto configGroup = m_customCommandsConfig->group(groupName);
    auto c = new Command();
    c->commandId = groupName;
    c->command = configGroup.readEntry("Command", QString());
    c->osdMessage = configGroup.readEntry("OsdMessage", QString()), c->type = configGroup.readEntry("Type", QString());
    m_customCommands << c;
    endInsertRows();

    if (c->type == QStringLiteral("shortcut")) {
        Action action;
        action.name = c->commandId;
        action.text = c->command;
        action.description = c->osdMessage;
        action.shortcut = appActionsModel()->getShortcut(action.name, QString());
        action.type = QStringLiteral("CustomAction");
        appActionsModel()->appendCustomAction(action);
    }
}

void CustomCommandsModel::editCustomCommand(int row, const QString &command, const QString &osdMessage, const QString &type)
{
    auto c = m_customCommands[row];
    c->command = command;
    c->osdMessage = osdMessage;
    c->type = type;

    QString groupName = c->commandId;
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Command"), command);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("OsdMessage"), osdMessage);
    m_customCommandsConfig->group(groupName).writeEntry(QStringLiteral("Type"), type);
    m_customCommandsConfig->sync();

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
    if (c->type == QStringLiteral("shortcut")) {
        appActionsModel()->editCustomAction(c->commandId, c->command, c->osdMessage);
    }
}

void CustomCommandsModel::toggleCustomCommand(const QString &groupName, int row, bool setOnStartup)
{
    auto group = m_customCommandsConfig->group(groupName);
    if (setOnStartup == group.readEntry("SetOnStartup", true)) {
        return;
    }
    auto customCommand = m_customCommands[row];
    customCommand->setOnStartup = setOnStartup;

    group.writeEntry("SetOnStartup", setOnStartup);
    m_customCommandsConfig->sync();

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void CustomCommandsModel::deleteCustomCommand(const QString &groupName, int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_customCommandsConfig->deleteGroup(groupName);
    m_customCommandsConfig->sync();
    m_customCommands.removeAt(row);
    endRemoveRows();

    KSharedConfig::Ptr config = KSharedConfig::openConfig(Global::instance()->appConfigFilePath());
    config->group("Shortcuts").deleteEntry(groupName);
    config->sync();
}

ActionsModel *CustomCommandsModel::appActionsModel()
{
    return m_appActionsModel;
}

void CustomCommandsModel::setAppActionsModel(ActionsModel *_appActionsModel)
{
    if (m_appActionsModel == _appActionsModel) {
        return;
    }
    m_appActionsModel = _appActionsModel;
    Q_EMIT appActionsModelChanged();
}

#include "moc_customcommandsmodel.cpp"

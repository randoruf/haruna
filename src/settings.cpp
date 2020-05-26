/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings.h"
#include "_debug.h"

#include <KConfig>
#include <KConfigGroup>
#include <QVariant>

Settings::Settings(QObject *parent) : QObject(parent)
{
    m_defaultSettings = {
        {"SeekSmallStep",         QVariant(5)},
        {"SeekMediumStep",        QVariant(15)},
        {"SeekBigStep",           QVariant(30)},
        {"VolumeStep",            QVariant(5)},
        {"OsdFontSize",           QVariant(25)},
        {"SubtitlesFolders",      QVariant(QStringLiteral("subs"))},
        {"LastPlayedFile",        QVariant(QStringLiteral())},
        {"LastUrl",               QVariant(QStringLiteral())},
        {"Volume",                QVariant(75)},
        // mouse actions
        {"Left",                  QVariant(QStringLiteral())},
        {"Left.x2",               QVariant(QStringLiteral("toggleFullscreenAction"))},
        {"Middle",                QVariant(QStringLiteral("muteAction"))},
        {"Middle.x2",             QVariant(QStringLiteral("configureAction"))},
        {"Right",                 QVariant(QStringLiteral("playPauseAction"))},
        {"Right.x2",              QVariant(QStringLiteral())},
        {"ScrollUp",              QVariant(QStringLiteral("volumeUpAction"))},
        {"ScrollDown",            QVariant(QStringLiteral("volumeDownAction"))},
        // playlist
        {"CanToggleWithMouse",    QVariant(true)},
        {"Position",              QVariant(QStringLiteral("right"))},
        {"RowHeight",             QVariant(50)},
        {"RowSpacing",            QVariant(1)},
        {"BigFontFullscreen",     QVariant(true)},
        // playback
        {"SkipChaptersWordList",  QVariant(QStringLiteral())},
        {"ShowOsdOnSkipChapters", QVariant(true)},
        {"SkipChapters",          QVariant(false)},
        // audio
        {"PreferredLanguage",     QVariant(QStringLiteral())},
        {"PreferredTrack",        QVariant(0)},
        // subtitle
        {"PreferredLanguage",     QVariant(QStringLiteral())},
        {"PreferredTrack",        QVariant(0)},
        // view
        {"MenuBarVisible",        QVariant(true)},
        {"HeaderVisible",         QVariant(true)},
    };

    m_config = KSharedConfig::openConfig("georgefb/haruna.conf");
}

int Settings::seekSmallStep()
{
    return get("General", "SeekSmallStep").toInt();
}

void Settings::setSeekSmallStep(int step)
{
    if (step == seekSmallStep()) {
        return;
    }
    set("General", "SeekSmallStep", QString::number(step));
    emit seekSmallStep();
}

int Settings::seekMediumStep()
{
    return get("General", "SeekMediumStep").toInt();
}

void Settings::setSeekMediumStep(int step)
{
    if (step == seekMediumStep()) {
        return;
    }
    set("General", "SeekMediumStep", QString::number(step));
    emit seekMediumStep();
}

int Settings::seekBigStep()
{
    return get("General", "SeekBigStep").toInt();
}

void Settings::setSeekBigStep(int step)
{
    if (step == seekBigStep()) {
        return;
    }
    set("General", "SeekBigStep", QString::number(step));
    emit seekBigStep();
}

bool Settings::skipChapters()
{
    return get("Playback", "SkipChapters").toBool();
}

void Settings::setSkipChapters(bool skip)
{
    if (skipChapters() == skip)
        return;
    set("Playback", "SkipChapters", QVariant(skip).toString());
    emit skipChaptersChanged();
}


int Settings::volumeStep()
{
    return get("General", "VolumeStep").toInt();
}

void Settings::setVolumeStep(int step)
{
    if (step == volumeStep()) {
        return;
    }
    set("General", "VolumeStep", QString::number(step));
    emit volumeStepChanged();
}

int Settings::osdFontSize()
{
    return get("", "OsdFontSize").toInt();
}

void Settings::setOsdFontSize(int fontSize)
{
    if (fontSize == osdFontSize()) {
        return;
    }
    set("General", "OsdFontSize", QString::number(fontSize));
    emit osdFontSizeChanged();
}

QString Settings::skipChaptersWordList()
{
    return get("Playback", "SkipChaptersWordList").toString();
}

void Settings::setSkipChaptersWordList(QString wordList)
{
    if (wordList == skipChaptersWordList()) {
        return;
    }
    set("Playback", "SkipChaptersWordList", wordList);
    emit skipChaptersWordListChanged();
}

bool Settings::showOsdOnSkipChapters()
{
    return get("Playback", "ShowOsdOnSkipChapters").toBool();
}

void Settings::setShowOsdOnSkipChapters(bool show)
{
    if (showOsdOnSkipChapters() == show) {
        return;
    }
    set("Playback", "ShowOsdOnSkipChapters", QVariant(show).toString());
    emit showOsdOnSkipChaptersChanged();
}

QString Settings::audioPreferredLanguage()
{
    return get("Audio", "PreferredLanguage").toString();
}

void Settings::setAudioPreferredLanguage(QString lang)
{
    if (lang == audioPreferredLanguage()) {
        return;
    }
    set("Audio", "PreferredLanguage", lang);
    emit audioPreferredLanguageChanged();
}

int Settings::audioPreferredTrack()
{
    return get("Audio", "PreferredTrack").toInt();
}

void Settings::setAudioPreferredTrack(int track)
{
    if (track == audioPreferredTrack()) {
        return;
    }
    set("Audio", "PreferredTrack", QString::number(track));
    emit audioPreferredTrackChanged();
}

QString Settings::playlistPosition()
{
    return get("Playlist", "Position").toString();
}

void Settings::setPlaylistPosition(QString position)
{
    if (position == playlistPosition()) {
        return;
    }
    set("Playlist", "Position", position);
    emit playlistPositionChanged();
}

int Settings::playlistRowHeight()
{
    return get("Playlist", "RowHeight").toInt();
}

void Settings::setPlaylistRowHeight(int height)
{
    if (height == playlistRowHeight()) {
        return;
    }
    set("Playlist", "RowHeight", QString::number(height));
    emit playlistRowHeightChanged();
}

int Settings::playlistRowSpacing()
{
    return get("Playlist", "RowSpacing").toInt();
}

void Settings::setPlaylistRowSpacing(int spacing)
{
    if (spacing == playlistRowSpacing()) {
        return;
    }
    set("Playlist", "RowSpacing", QString::number(spacing));
    emit playlistRowSpacingChanged();
}

bool Settings::playlistCanToggleWithMouse()
{
    return get("Playlist", "CanToggleWithMouse").toBool();
}

void Settings::setPlaylistCanToggleWithMouse(bool toggleWithMouse)
{
    if (toggleWithMouse == playlistCanToggleWithMouse()) {
        return;
    }
    set("Playlist", "CanToggleWithMouse", QVariant(toggleWithMouse).toString());
    emit playlistCanToggleWithMouseChanged();
}

bool Settings::playlistBigFontFullscreen()
{
    return get("Playlist", "BigFontFullscreen").toBool();
}

void Settings::setPlaylistBigFontFullscreen(bool bigFont)
{
    if (bigFont == playlistBigFontFullscreen()) {
        return;
    }
    set("Playlist", "BigFontFullscreen", QVariant(bigFont).toString());
    emit playlistBigFontFullscreenChanged();
}




QVariant Settings::get(const QString &group, const QString &key)
{
    return m_config->group(group).readEntry(key, defaultSetting(key));
}

void Settings::set(const QString &group, const QString &key, const QString &value)
{
    m_config->group(group).writeEntry(key, value);
    m_config->sync();

    emit settingsChanged();
}

QVariant Settings::getPath(const QString &group, const QString &key)
{
    return m_config->group(group).readPathEntry(key, QStringList());
}

void Settings::setPath(const QString &group, const QString &key, const QString &value)
{
    m_config->group(group).writePathEntry(key, value);
    m_config->sync();
}

QVariant Settings::defaultSetting(const QString &key)
{
    return m_defaultSettings[key];
}

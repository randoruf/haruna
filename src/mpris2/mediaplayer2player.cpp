/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mediaplayer2player.h"

#include "application.h"
#include "mpvitem.h"
#include "videosettings.h"
#include "worker.h"

#include <QBuffer>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QImage>

#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/SimpleExtractionResult>

MediaPlayer2Player::MediaPlayer2Player(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    connect(this, &MediaPlayer2Player::mpvChanged, this, &MediaPlayer2Player::setupConnections);
}

void MediaPlayer2Player::setupConnections()
{
    if (!m_mpv) {
        return;
    }

    connect(m_mpv, &MpvItem::pauseChanged, this, [=]() {
        propertiesChanged("PlaybackStatus", PlaybackStatus());
        Q_EMIT playbackStatusChanged();
    });
    connect(m_mpv, &MpvItem::positionChanged, this, [=]() {
        propertiesChanged("PlaybackStatus", PlaybackStatus());
        Q_EMIT playbackStatusChanged();
    });
    connect(m_mpv, &MpvItem::volumeChanged, this, [=]() {
        propertiesChanged("Volume", Volume());
        Q_EMIT volumeChanged();
    });
    connect(m_mpv, &MpvItem::fileLoaded, this, [=]() {
        propertiesChanged("Metadata", Metadata());
        Q_EMIT metadataChanged();
    });
}

void MediaPlayer2Player::propertiesChanged(const QString &property, const QVariant &value)
{
    QDBusMessage msg = QDBusMessage::createSignal(QStringLiteral("/org/mpris/MediaPlayer2"),
                                                  QStringLiteral("org.freedesktop.DBus.Properties"),
                                                  QStringLiteral("PropertiesChanged"));

    QVariantMap properties;
    properties[property] = value;

    msg << QString("org.mpris.MediaPlayer2.Player");
    msg << properties;
    msg << QStringList();

    QDBusConnection::sessionBus().send(msg);
}

void MediaPlayer2Player::Next()
{
    Q_EMIT next();
}

void MediaPlayer2Player::Previous()
{
    Q_EMIT previous();
}

void MediaPlayer2Player::Pause()
{
    Q_EMIT pause();
}

void MediaPlayer2Player::PlayPause()
{
    Q_EMIT playpause();
}

void MediaPlayer2Player::Stop()
{
    Q_EMIT stop();
}

void MediaPlayer2Player::Play()
{
    Q_EMIT play();
}

void MediaPlayer2Player::Seek(qlonglong offset)
{
    Q_EMIT seek(offset / 1000 / 1000);
}

void MediaPlayer2Player::SetPosition(const QDBusObjectPath &trackId, qlonglong pos)
{
    Q_UNUSED(trackId)
    m_mpv->setProperty("time-pos", pos / 1000 / 1000);
}

void MediaPlayer2Player::OpenUri(const QString &uri)
{
    Q_EMIT openUri(uri);
}

QString MediaPlayer2Player::PlaybackStatus()
{
    if (!m_mpv) {
        return QString();
    }
    bool isPaused = m_mpv->getProperty("pause").toBool();
    int position = m_mpv->getProperty("time-pos").toInt();

    return isPaused && position == 0 ? "Stopped" : (isPaused ? "Paused" : "Playing");
}

QVariantMap MediaPlayer2Player::Metadata()
{
    if (!m_mpv) {
        return QVariantMap();
    }
    QVariantMap metadata;
    metadata.insert(QStringLiteral("mpris:length"), m_mpv->getProperty("duration").toDouble() * 1000 * 1000);
    metadata.insert(QStringLiteral("mpris:trackid"), QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath("/org/kde/haruna")));

    auto mpvMediaTitle = m_mpv->getProperty("media-title").toString();
    auto mpvFilename = m_mpv->getProperty("filename").toString();
    auto title = mpvMediaTitle.isEmpty() || mpvMediaTitle.isNull() ? mpvFilename : mpvMediaTitle;
    metadata.insert(QStringLiteral("xesam:title"), title);

    auto path = m_mpv->getProperty("path").toString();

    metadata.insert(QStringLiteral("mpris:artUrl"), getThumbnail(path));

    QUrl url(path);
    url.setScheme("file");
    metadata.insert(QStringLiteral("xesam:url"), url.toString());

    return metadata;
}

double MediaPlayer2Player::Volume()
{
    if (!m_mpv) {
        return 0;
    }
    return m_mpv->getProperty("volume").toDouble() / 100;
}

qlonglong MediaPlayer2Player::Position()
{
    if (!m_mpv) {
        return 0;
    }
    return m_mpv->getProperty("time-pos").toDouble() * 1000 * 1000;
}

bool MediaPlayer2Player::CanGoNext()
{
    return true;
}

bool MediaPlayer2Player::CanGoPrevious()
{
    return true;
}

bool MediaPlayer2Player::CanPlay()
{
    return true;
}

bool MediaPlayer2Player::CanPause()
{
    return true;
}

bool MediaPlayer2Player::CanSeek()
{
    return true;
}

bool MediaPlayer2Player::CanControl()
{
    return true;
}

void MediaPlayer2Player::setPosition(int pos)
{
    if (!m_mpv) {
        return;
    }
    m_mpv->setProperty("position", pos);
}

void MediaPlayer2Player::setVolume(double vol)
{
    if (!m_mpv) {
        return;
    }
    m_mpv->setProperty("volume", vol * 100);
}

MpvItem *MediaPlayer2Player::mpv() const
{
    return m_mpv;
}

void MediaPlayer2Player::setMpv(MpvItem *mpv)
{
    if (m_mpv == mpv) {
        return;
    }
    m_mpv = mpv;
    Q_EMIT mpvChanged();
}

QString MediaPlayer2Player::getThumbnail(const QString &path)
{
    QString mimeType = Application::mimeType(QUrl(path));
    KFileMetaData::ExtractorCollection exCol;
    QList<KFileMetaData::Extractor *> extractors = exCol.fetchExtractors(mimeType);
    KFileMetaData::SimpleExtractionResult result(path, mimeType, KFileMetaData::ExtractionResult::ExtractImageData);

    if (extractors.size() > 0) {
        KFileMetaData::Extractor *ex = extractors.first();
        ex->extract(&result);
        QString base64 = QStringLiteral("data:image/png;base64,");
        auto imageData = result.imageData();
        if (!imageData.isEmpty()) {
            // look for image inside the file
            using namespace KFileMetaData;
            if (imageData.contains(EmbeddedImageData::FrontCover)) {
                return base64.append(imageData[EmbeddedImageData::FrontCover].toBase64());
            } else if (imageData.contains(EmbeddedImageData::BackCover)) {
                return base64.append(imageData[EmbeddedImageData::BackCover].toBase64());
            } else if (imageData.contains(EmbeddedImageData::MovieScreenCapture)) {
                return base64.append(imageData[EmbeddedImageData::MovieScreenCapture].toBase64());
            } else if (imageData.contains(EmbeddedImageData::Other)) {
                return base64.append(imageData[EmbeddedImageData::Other].toBase64());
            } else {
                return base64.append(imageData[EmbeddedImageData::Unknown].toBase64());
            }
        } else {
            // try to generate QImage from file
            QImage image = Worker::instance()->frameToImage(path, 250);
            if (image.isNull()) {
                // generation failed, use user set default cover
                return VideoSettings::defaultCover();
            } else {
                // generation succeeded
                QByteArray byteArray;
                QBuffer buffer(&byteArray);
                image.save(&buffer, "JPEG");
                return base64.append(byteArray.toBase64().data());
            }
        }
    }
    return QString();
}

#include "moc_mediaplayer2player.cpp"

#include "Metadata.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>

#include "JsonValidator.h"

const ImageMetadata::SpriteMetadata& 
    ImageMetadata::GetSpriteMetadata(const QString& name) const
{
    auto it = sprites_metadata_.find(name);
    if (it == sprites_metadata_.end())
    {
        qFatal("%s", QString("Unable to find sprite metadata for: %1").arg(name).toLatin1().data());
    }
    return *it;
}

bool ImageMetadata::IsValidState(const QString& name) const
{
    if (!Valid())
    {
        return false;
    }
    return sprites_metadata_.find(name) != sprites_metadata_.end();
}

void ImageMetadata::Init(const QString& name, int width, int height)
{   
    qDebug() << "Begin to init metadata for " << name;

    width_ = width;
    height_ = height;

    QFile file(name + ".json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open metadata file, trying without metadata: " << name;
        InitWithoutMetadata();
    }
    else
    {
        QByteArray data = file.readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(data, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qFatal("%s", QString("Json parase error: %1").arg(error.errorString()).toLatin1().data());
        }
        ParseInfo(document.object());
    }

    if (!Valid())
    {
        qFatal("Fail during metadata parsing, abort!");
    }
    qDebug() << "End load metadata for " << name;
}

void ImageMetadata::InitWithoutMetadata()
{
    qDebug() << "Fail metadata load, try without it";

    sprites_metadata_.clear();

    sprites_metadata_[""].first_frame_pos = 0;

    sprites_metadata_[""].frames_sequence.push_back(0);
    valid_ = true;
}

void ImageMetadata::ParseInfo(const QJsonObject& metadata)
{
    if (metadata.isEmpty())
    {
        qFatal("Corrupted metadata!");
    }

    QJsonObject info;
    if (!ValidateKey(metadata, "info", &info))
    {
        qFatal("Corrupted 'info' key in metadata!");
    }
    if (!ValidateKey(info, "version", &version_))
    {
        qFatal("Corrupted 'version' key in info!");
    }
    if (!ValidateKey(info, "width", &width_))
    {
        qFatal("Corrupted 'width' key in info!");
    }
    if (!ValidateKey(info, "height", &height_))
    {
        qFatal("Corrupted 'height' key in info!");
    }

    QJsonArray states;
    if (!ValidateKey(metadata, "states", &states))
    {
        qFatal("Corrupted 'states' key in metadata!");
    }

    int first_frame_pos = 0;
    for (const QJsonValue& state_value : states)
    {
        QJsonObject state;
        if (!ValidateValue(state_value, &state))
        {
            qFatal("Corrupted state in states!");
        }
        QString state_name;
        if (!ValidateKey(state, "state", &state_name))
        {
            qFatal("Corrupted 'state' key in state!");
        }
        qDebug() << "State name:" << state_name;

        SpriteMetadata& current_metadata = sprites_metadata_[state_name];
        current_metadata.first_frame_pos = first_frame_pos;

        if (!ValidateKey(state, "dirs", &current_metadata.dirs))
        {
            qFatal("Unable to load dirs from state!");
        }
        int frames_amount;
        if (!ValidateKey(state, "frames", &frames_amount))
        {
            qFatal("Unable to load dirs from state!");
        }
        current_metadata.frames_data.resize(frames_amount);
        first_frame_pos += frames_amount * current_metadata.dirs;

        QJsonArray delays;
        if (ValidateKey(state, "delay", &delays))
        {
            if (delays.size() != current_metadata.frames_data.size())
            {
                qFatal("Frames-delays amount mismatch!");
            }
            for (int i = 0; i < delays.size(); ++i)
            {
                const QJsonValue& value = delays[i];
                double delay;
                if (!ValidateValue(value, &delay))
                {
                    qFatal("%s", QString("Corrupted delay value: %1").arg(i).toLatin1().data());
                }
                current_metadata.frames_data[i].delay = delay;
            }
        }
        else if (current_metadata.frames_data.size() > 1)
        {
            qFatal("Delays are missing for frames!");
        }

        ValidateKey(state, "rewind", &current_metadata.rewind);
        ValidateKey(state, "loop", &current_metadata.loop);

        QJsonArray hotspot;
        if (ValidateKey(state, "hotspot", &hotspot))
        {
            const int HOTSPOT_SIZE = 3;
            if (hotspot.size() != HOTSPOT_SIZE)
            {
                qFatal("Invalid hotspot size!");
            }
            for (int i = 0; i < HOTSPOT_SIZE; ++i)
            {
                const QJsonValue& value = hotspot[i];
                int hotspot_value;
                if (!ValidateValue(value, &hotspot_value))
                {
                    qFatal("Corrupted hotspot value!");
                }
                current_metadata.hotspot[i] = hotspot_value;
            }
        }
    }

    qDebug() << "Begin make sequence";

    MakeSequence();

    qDebug() << "End make sequence";

    valid_ = true;
}

void ImageMetadata::MakeSequence()
{
    for (auto it = sprites_metadata_.begin(); it != sprites_metadata_.end(); ++it)
    {
        auto& metadata = it->frames_data;
        auto& sequence = it->frames_sequence;
        int local_loop = it->loop;
        if (it->loop == -1 || it->loop == 0)
        {
            local_loop = 1;
        }

        for (int loop_i = 0; loop_i < local_loop; ++loop_i)
        {
            for (qint32 i = 0; i < metadata.size(); ++i)
            {
                sequence.push_back(i);
            }
            if (it->rewind)
            {
                qint32 from = metadata.size() - 2;
                if (from < 0)
                {
                    from = 0;
                }
                for (qint32 i = from; i > 0; --i)
                {
                    sequence.push_back(i);
                }
            }
        }
        if (!(it->loop == -1 || it->loop == 0))
        {
            sequence.push_back(-1);
        }
    }
}

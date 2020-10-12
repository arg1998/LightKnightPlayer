#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QString>
#include <string>
#include <QMediaPlayer>

#include "Color.h"
#include "Frame.h"


class Project
{
public:
    QString name;
    QString projectPath;
    QString musicFileName;
    int numRgbChannels;
    int numBinaryChannels;
    int numOpacityChannels;
    int fps;
    qint64 duration;
    int framesCount;
    QVector<Frame> frames;
    QDir projectDirectory;


public:
    Project(QDir dir);
    Project();
    ~Project();
    QString transformFrameData();
    QVector<Frame> getFrames();
    void clear();
    void setProjectPath(QDir dir);

};

#endif // PROJECT_H

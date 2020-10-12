#include "project.h"

Project::Project(QDir dir)
{
    name = nullptr;
    projectPath = dir.absolutePath();
    musicFileName = nullptr;
    fps = 25;
    duration = 0;
    numRgbChannels = 0;
    numBinaryChannels = 0;
    numOpacityChannels =0;
    framesCount = 0;
    projectDirectory = dir;
}

Project::Project()
{
    name = nullptr;
    projectPath = nullptr;
    musicFileName = nullptr;
    fps = 25;
    duration = 0;
    numRgbChannels = 0;
    numBinaryChannels = 0;
    numOpacityChannels = 0;
    framesCount = 0;
    projectDirectory = nullptr;
}

Project::~Project()
{
    clear();
}

QString Project::transformFrameData(){
    QString error = nullptr;
    if(projectPath == nullptr){
        return (error = "ERROR >> project path is not defined");
    }
    QString configFilePath = projectDirectory.absolutePath() + "/config.json";
    QString channelsDataFilePath =  projectDirectory.absolutePath() + "/channelsData.json";
    if(!(QFileInfo::exists(configFilePath) && QFileInfo::exists(channelsDataFilePath))){
       error = "EROR << required files do not exist in the provided directory!";
       qDebug() << error;
    }

    QFile fileReader;
    fileReader.setFileName(configFilePath);
    fileReader.open(QIODevice::ReadOnly | QIODevice::Text);
    QString output = fileReader.readAll();

    QJsonObject configs;
    QJsonObject channelsData;

    configs = QJsonDocument::fromJson(output.toUtf8()).object();

    fileReader.flush();
    fileReader.close();
    fileReader.setFileName(channelsDataFilePath);
    fileReader.open(QIODevice::ReadOnly | QIODevice::Text);

    output = fileReader.readAll();

    channelsData = QJsonDocument::fromJson(output.toUtf8()).object();

    fileReader.flush();
    fileReader.close();

    this->projectPath = projectDirectory.absolutePath();
    this->name = configs["projectName"].toString();
    this->musicFileName = configs["musicName"].toString();
    this->framesCount = channelsData["framesCount"].toInt(0) + 5;
    this->fps = 25;

    QJsonArray rgbChannelsIDs;
    QJsonArray binaryChannelsIDs;
    QJsonArray opacityChannelsIDs;

    if(channelsData.contains("rgbChannels")){
        rgbChannelsIDs = channelsData["rgbChannels"].toArray();
    }

    if(channelsData.contains("binaryChannels")){
        binaryChannelsIDs = channelsData["binaryChannels"].toArray();
    }

    if(channelsData.contains("opacityChannels")){
        opacityChannelsIDs = channelsData["opacityChannels"].toArray();
    }


    this->numRgbChannels = rgbChannelsIDs.size();
    this->numBinaryChannels = binaryChannelsIDs.size();
    this->numOpacityChannels = opacityChannelsIDs.size();

    frames = QVector<Frame>(this->framesCount);

    for(int f = 0; f< this->framesCount; f++){
        Color rgbData[numRgbChannels];
        QByteArray binaryData(numBinaryChannels, LED_OFF);
        QByteArray opacityData(numOpacityChannels, LED_OFF);

        //25 frame per seconds gives a 40ms interval between each consequetive frame
        QString pos_ms = QString::fromStdString(std::to_string(f * 40));
        for(int x = 0; x < numRgbChannels; x++){
            QString _channelId= rgbChannelsIDs[x].toString();
            if(channelsData[_channelId].toObject().contains(pos_ms)){

                QString frameColor = channelsData[_channelId].toObject()[pos_ms].toString();
                rgbData[x] = Color(frameColor);
            } else{
                rgbData[x] = LED_BLACK_COLOR;
            }
        }

        for(int x = 0; x < numOpacityChannels; x++){
            QString _channelId= opacityChannelsIDs[x].toString();
            if(channelsData[_channelId].toObject().contains(pos_ms)){
                unsigned char alpha = channelsData[_channelId].toObject()[pos_ms].toVariant().value<uchar>();
                opacityData[x] = alpha;
            }else {
                opacityData[x] = LED_OFF;
            }
        }

        for(int x = 0; x < numBinaryChannels; x++){
            QString _channelId= binaryChannelsIDs[x].toString();
            if(channelsData[_channelId].toObject().contains(pos_ms)){
                binaryData[x] = LED_ON;
            }else {
               binaryData[x] = LED_OFF;
            }
        }

        Frame frame(rgbData, numRgbChannels,opacityData, numOpacityChannels, binaryData, numBinaryChannels);

        frames[f] = frame;
    }
    return error;

}

QVector<Frame> Project::getFrames()
{
    return this->frames;
}

void Project::clear()
{
    if(!frames.empty()){
        frames.clear();
        qDebug() <<"deleting project from memory";
    }
    qDebug() <<"cleared project";
}

void Project::setProjectPath(QDir dir)
{
    projectPath = dir.absolutePath();
    projectDirectory = dir;
}


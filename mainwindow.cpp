#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this->ui->btn_play , SIGNAL(clicked()), this, SLOT(play_and_pause()));
    connect(this->ui->btn_import_new_project, SIGNAL(clicked()), this, SLOT(import_new_project()));
    connect(this->ui->btn_refresh_ports, SIGNAL(clicked()), this, SLOT(refresh_ports()));
    connect(this->ui->btn_connect, SIGNAL(clicked()), this, SLOT(connect_disconnect_port()));
    currentProject = Project();
    mediaPlayer = new QMediaPlayer(this, QMediaPlayer::Flag::LowLatency);
    mediaPlayer->setNotifyInterval(10);
    addLog("media player notify interval : " + QString::number(100));
    mediaPlayer->setVolume(100);
    connect(mediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaPlayerStateChanged(QMediaPlayer::State)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(audioProgress(qint64)));
    connect(this, SIGNAL(updateSlider(int)), this->ui->slider_song_progress, SLOT(setValue(int)));
    connect(this->ui->slider_song_progress, SIGNAL(sliderPressed()), this, SLOT(beginDrag()));
    connect(this->ui->slider_song_progress, SIGNAL(sliderReleased()), this, SLOT(endDrag()));
    connect(this->ui->cmbx_ports, SIGNAL(currentIndexChanged(int)), this, SLOT(onPortChangedIndex(int)));
    m_isPlaying = false;
    isDraggingSlider = false;
    this->ui->btn_play->setEnabled(false);
    this->ui->listview_logs->setAutoScroll(true);
    this->ui->btn_connect->setEnabled(false);
    selectedPort = nullptr;
    isPortConnected = false;
    currentFrame = -1;
    _temp_pos = -1;
    port = nullptr;
    refresh_ports();
}

MainWindow::~MainWindow()
{
    if(mediaPlayer->state() == QMediaPlayer::State::PlayingState){
        mediaPlayer->stop();
    }
    delete mediaPlayer;
    delete ui;

}

void MainWindow::play_and_pause(){

    if(this->m_isPlaying){
        pause();
    }else {
        play();
    }
}

void MainWindow::refresh_ports(){

    this->ui->cmbx_ports->clear();
    portlist.clear();
    portlist = QSerialPortInfo::availablePorts();
    for(auto &portInfo : portlist ){
        QString item =(portInfo.serialNumber())+ " - " + portInfo.portName();
        qDebug() << item;
        this->ui->cmbx_ports->addItem(item);
    }
    addLog(QString("refreshed ports list, found %1 ports").arg(portlist.length()));
}

void MainWindow::play()
{
    mediaPlayer->play();
    this->ui->btn_play->setText("Pause");
    this->m_isPlaying = true;
    addLog("media player state : playing");
}

void MainWindow::pause()
{
    mediaPlayer->pause();
    this->ui->btn_play->setText("Play");
    this->m_isPlaying = false;
    addLog("media player state : paused");
}

void MainWindow::addLog(QString log)
{
    this->ui->listview_logs->addItem(log);
    this->ui->listview_logs->scrollToBottom();
}

void MainWindow::displayProjectInfo()
{
    int min = (currentProject.duration / 1000) / 60;
    int sec = (currentProject.duration / 1000) % 60;
    QString duration = currentProject.duration == 0 ?
                "" :
                QString("\nSong Duration : %1:%2").arg(QString::number(min).rightJustified(2,'0')).arg(QString::number(sec).rightJustified(2,'0'));
    this->ui->label_project_info->setText("Project Name: " + currentProject.name + "\nSong Name: "  + currentProject.musicFileName + "\nFrames Count: "+ QString::number(currentProject.framesCount) + duration );

}

void MainWindow::audioProgress(qint64 position)
{
    if(port != nullptr && port->isOpen()){
        _temp_pos = position / 40; // caculate the frame offset, each frame lasts 40ms
        if(_temp_pos != currentFrame && _temp_pos < currentProject.framesCount){
            port->write(currentProject.frames[_temp_pos].toByteArray());
            currentFrame = _temp_pos;
        }
    }

    if(!isDraggingSlider){
        emit updateSlider(mediaPlayer->position());
    }
}

void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::MediaStatus::LoadingMedia){
        this->ui->label_song_info->setText("Laoding ...");
        this->ui->btn_play->setEnabled(false);
        addLog("media player status changed : Loading");
    }else if(status == QMediaPlayer::MediaStatus::LoadedMedia){
        this->ui->btn_play->setEnabled(true);
        currentProject.duration = mediaPlayer->duration();
        this->ui->label_song_info->setText(QString("Song has been loaded successfuly"));
        this->ui->slider_song_progress->setMinimum(0);
        this->ui->slider_song_progress->setMaximum(mediaPlayer->duration());
        addLog("media player status changed : Loaded");
        displayProjectInfo();
    }else if(status == QMediaPlayer::MediaStatus::InvalidMedia){
        this->ui->label_song_info->setText("Invalid Media!");
        this->ui->btn_play->setEnabled(false);
        addLog("media player status changed : Invalid");
    }
}

void MainWindow::beginDrag()
{
    isDraggingSlider = true;
}

void MainWindow::endDrag()
{
    isDraggingSlider = false;
    mediaPlayer->setPosition(this->ui->slider_song_progress->sliderPosition());
}

void MainWindow::resetMusic()
{
    mediaPlayer->setPosition(0);
    pause();
}

void MainWindow::mediaPlayerStateChanged(QMediaPlayer::State state)
{
    if(state == QMediaPlayer::State::StoppedState){
        addLog("media player state : Stopped");
        resetMusic();
    }
}

void MainWindow::onPortChangedIndex(int index)
{
    if(index < 0){
        delete  selectedPort;
        selectedPort = nullptr;
        this->ui->btn_connect->setEnabled(false);
        addLog("no port is selected");
    }else {
        selectedPort = new QSerialPortInfo(portlist[index]);
        this->ui->btn_connect->setEnabled(true);
        addLog(QString("selected port %1").arg(selectedPort->portName()));
    }
}

void MainWindow::connect_disconnect_port()
{
    if(isPortConnected){
        //disconnect the port
        this->ui->btn_connect->setText("Connect");
        if(port == nullptr){
            addLog("ERROR >>> port is not initialized");
            return;
        }
        port->flush();
        port->clear(QSerialPort::Direction::Output);
        port->close();

        addLog("port has been closed successfully");
        this->ui->btn_refresh_ports->setEnabled(true);
        this->ui->cmbx_ports->setEnabled(true);
        this->ui->label_port_connection_status->setText("Not Connected");
        isPortConnected = false;

    }else{
        //connect to the port
        if(selectedPort == nullptr){
            addLog("ERROR >>> no port selected");
            return;
        }

        port = new QSerialPort();
        port->setPort(*selectedPort);
        port->setBaudRate(QSerialPort::Baud9600);
        port->setDataBits(QSerialPort::Data8);
        port->setParity(QSerialPort::NoParity);
        port->setStopBits(QSerialPort::OneStop);
        port->setFlowControl(QSerialPort::NoFlowControl);
        bool succeed = port->open(QIODevice::WriteOnly);
        if(!succeed){
            addLog("ERROR >>> can not open the port : " + port->errorString());
            delete port;
            port = nullptr;
            return;
        }
        addLog("port has been opened successfully");
        this->ui->btn_refresh_ports->setEnabled(false);
        this->ui->cmbx_ports->setEnabled(false);
        this->ui->btn_connect->setText("Disconnect");
        this->ui->label_port_connection_status->setText("Connected");
        isPortConnected = true;
    }
}

void MainWindow::import_new_project(){
    QString dir = QFileDialog::getExistingDirectory(this, "Select a Project Directory", "", QFileDialog::ShowDirsOnly);
    this->ui->btn_play->setEnabled(false);
    if(m_isPlaying){
        pause();
    }

    if(dir.isNull() || dir.isEmpty()){
        addLog("user returned with inavlid directory");
        this->ui->label_project_info->setText("No project loaded yet");
        return;
    }
    currentProject.clear();
    currentProject.setProjectPath(QDir(dir));

    QString err = currentProject.transformFrameData();
    if(err != nullptr){
        addLog("ERROR >>> frame transformation faild: " + err);
        return;
    }

    QMediaContent mediaContent(QUrl(currentProject.projectPath + "/" + currentProject.musicFileName));
    mediaPlayer->setMedia(mediaContent);
    this->ui->label_project_info->setText("Project Name: " + currentProject.name + "\nSong Name: " + currentProject.musicFileName);
    addLog("project loaded sucessfuly");
}


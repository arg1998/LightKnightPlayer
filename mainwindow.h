#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMediaPlayer>

#include "project.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool m_isPlaying;
    Project currentProject;
    QMediaPlayer *mediaPlayer;
    bool isDraggingSlider;
    QList<QSerialPortInfo> portlist;
    QSerialPortInfo *selectedPort;
    bool isPortConnected;
    QSerialPort *port;
    int currentFrame;
    int _temp_pos;

public:
    bool connectToArduino();
    void displayProjectInfo();
    void resetMusic();
    void play();
    void pause();
    void addLog(QString);

signals:
     void updateSlider(int value);

public slots:
    void import_new_project();
    void play_and_pause();
    void refresh_ports();
    void audioProgress(qint64);
    void mediaStatusChanged(QMediaPlayer::MediaStatus);
    void beginDrag();
    void endDrag();
    void mediaPlayerStateChanged(QMediaPlayer::State);
    void onPortChangedIndex(int);
    void connect_disconnect_port();

};
#endif // MAINWINDOW_H

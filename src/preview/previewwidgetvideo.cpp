#include "previewwidgetvideo.h"
#include "ui_previewwidgetvideo.h"

#include <QtWidgets>
#include <QtGui/QMovie>
#include <QtWidgets/QWidget>

PreviewWidgetVideo::PreviewWidgetVideo(const QString &file_path, QWidget *parent)
        : QWidget(parent),
        //mediaPlayer(0, QMediaPlayer::VideoSurface),
          ui(new Ui::PreviewWidgetVideo) {
    ui->setupUi(this);
    ui->playButton->setEnabled(false);
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(ui->playButton, &QAbstractButton::clicked, this, &PreviewWidgetVideo::play);
    ui->positionSlider->setRange(0, 0);
    connect(ui->positionSlider, &QAbstractSlider::sliderMoved, this, &PreviewWidgetVideo::setPosition);
    ui->errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    mediaPlayer.setVideoOutput(ui->videoWidget);

    connect(&mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &PreviewWidgetVideo::mediaStateChanged);
    connect(&mediaPlayer, &QMediaPlayer::positionChanged, this, &PreviewWidgetVideo::positionChanged);
    connect(&mediaPlayer, &QMediaPlayer::durationChanged, this, &PreviewWidgetVideo::durationChanged);

    connect(ui->stopButton, SIGNAL(clicked()), &mediaPlayer, SLOT(stop()));

    connect(&mediaPlayer, &QMediaPlayer::errorOccurred, this, &PreviewWidgetVideo::handleError);
    mediaPlayer.setSource(QUrl::fromLocalFile(file_path));
    ui->playButton->setEnabled(true);
}

PreviewWidgetVideo::~PreviewWidgetVideo() {
    mediaPlayer.stop();
    delete ui;
}

void PreviewWidgetVideo::setUrl(const QUrl &url) {
    ui->errorLabel->setText(QString());
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    mediaPlayer.setSource(url);
    ui->playButton->setEnabled(true);
}

void PreviewWidgetVideo::play() {
    if (mediaPlayer.playbackState() == QMediaPlayer::PlayingState) {
        mediaPlayer.pause();
    } else {
        mediaPlayer.play();
    }
}

void PreviewWidgetVideo::mediaStateChanged(QMediaPlayer::PlaybackState state) {
    if (state == QMediaPlayer::PlayingState) {
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        ui->playButton->setText(tr("Play"));
    } else {
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        ui->playButton->setText(tr("Pause"));
    }
}

void PreviewWidgetVideo::positionChanged(int position) {
    ui->positionSlider->setValue(position);
}

void PreviewWidgetVideo::durationChanged(int duration) {
    ui->positionSlider->setRange(0, duration);
}

void PreviewWidgetVideo::setPosition(int position) {
    mediaPlayer.setPosition(position);
}

void PreviewWidgetVideo::handleError(QMediaPlayer::Error error, const QString &errorString) {
    ui->playButton->setEnabled(false);
    QString message = "Error: ";
    if (errorString.isEmpty())
        message += " #" + QString::number(int(error));
    else
        message += errorString;
    ui->errorLabel->setText(message);
}

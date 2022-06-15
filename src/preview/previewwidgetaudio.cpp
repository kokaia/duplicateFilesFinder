#include "previewwidgetaudio.h"
#include "ui_previewwidgetaudio.h"

#include <QUrl>
#include <QStyle>

PreviewWidgetAudio::PreviewWidgetAudio(const QString &file_path, QWidget *parent)
        : QWidget(parent),
        // mediaPlayer(0, QMediaPlayer::VideoSurface),
          ui(new Ui::PreviewWidgetAudio) {
    ui->setupUi(this);
    ui->playButton->setEnabled(false);
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(ui->playButton, &QAbstractButton::clicked, this,
            &PreviewWidgetAudio::play);
    ui->positionSlider->setRange(0, 0);
    connect(ui->positionSlider, &QAbstractSlider::sliderMoved, this, &PreviewWidgetAudio::setPosition);
    ui->errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    connect(&mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &PreviewWidgetAudio::mediaStateChanged);
    connect(&mediaPlayer, &QMediaPlayer::positionChanged, this, &PreviewWidgetAudio::positionChanged);
    connect(&mediaPlayer, &QMediaPlayer::durationChanged, this, &PreviewWidgetAudio::durationChanged);
    connect(ui->stopButton, SIGNAL(clicked()), &mediaPlayer, SLOT(stop()));

    connect(&mediaPlayer, &QMediaPlayer::errorOccurred, this, &PreviewWidgetAudio::handleError);
    mediaPlayer.setSource(QUrl::fromLocalFile(file_path));
    ui->playButton->setEnabled(true);
}

PreviewWidgetAudio::~PreviewWidgetAudio() {
    mediaPlayer.stop();
    delete ui;
}

void PreviewWidgetAudio::setUrl(const QUrl &url) {
    ui->errorLabel->setText(QString());
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    mediaPlayer.setSource(url);
    ui->playButton->setEnabled(true);
}

void PreviewWidgetAudio::play() {
    if (mediaPlayer.playbackState() == QMediaPlayer::PlayingState) {
        mediaPlayer.pause();
    } else {
        mediaPlayer.play();
    }
}

void PreviewWidgetAudio::mediaStateChanged(QMediaPlayer::PlaybackState state) {
    if (state == QMediaPlayer::PlayingState) {
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        ui->playButton->setText(tr("Play"));
    } else {
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        ui->playButton->setText(tr("Pause"));
    }
}

void PreviewWidgetAudio::positionChanged(int position) {
    ui->positionSlider->setValue(position);
}

void PreviewWidgetAudio::durationChanged(int duration) {
    ui->positionSlider->setRange(0, duration);
}

void PreviewWidgetAudio::setPosition(int position) {
    mediaPlayer.setPosition(position);
}

void PreviewWidgetAudio::handleError(QMediaPlayer::Error error, const QString &errorString) {
    ui->playButton->setEnabled(false);
    QString message = "Error: ";
    if (errorString.isEmpty())
        message += " #" + QString::number(int(error));
    else
        message += errorString;
    ui->errorLabel->setText(message);
}

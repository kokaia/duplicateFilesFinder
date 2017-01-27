#include "previewwidgetvideo.h"
#include "ui_previewwidgetvideo.h"

PreviewWidgetVideo::PreviewWidgetVideo(const QString &file_path,
                                       QWidget *parent)
    : QWidget(parent),
      //mediaPlayer(0, QMediaPlayer::VideoSurface),
      ui(new Ui::PreviewWidgetVideo) {
  ui->setupUi(this);
  ui->playButton->setEnabled(false);
  ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  connect(ui->playButton, &QAbstractButton::clicked, this,
          &PreviewWidgetVideo::play);
  ui->positionSlider->setRange(0, 0);
  connect(ui->positionSlider, &QAbstractSlider::sliderMoved, this,
          &PreviewWidgetVideo::setPosition);
  ui->errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  mediaPlayer.setVideoOutput(ui->videoWidget);
  connect(&mediaPlayer, &QMediaPlayer::stateChanged, this,
          &PreviewWidgetVideo::mediaStateChanged);
  connect(&mediaPlayer, &QMediaPlayer::positionChanged, this,
          &PreviewWidgetVideo::positionChanged);
  connect(&mediaPlayer, &QMediaPlayer::durationChanged, this,
          &PreviewWidgetVideo::durationChanged);

  connect(ui->stopButton, SIGNAL(clicked()), &mediaPlayer,
          SLOT(stop()));

  typedef void (QMediaPlayer::*ErrorSignal)(QMediaPlayer::Error);
  connect(&mediaPlayer, static_cast<ErrorSignal>(&QMediaPlayer::error), this,
          &PreviewWidgetVideo::handleError);
  mediaPlayer.setMedia(QUrl::fromLocalFile(file_path));
  ui->playButton->setEnabled(true);
}

PreviewWidgetVideo::~PreviewWidgetVideo() { delete ui; }

void PreviewWidgetVideo::setUrl(const QUrl &url) {
  ui->errorLabel->setText(QString());
  setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
  mediaPlayer.setMedia(url);
  ui->playButton->setEnabled(true);
}

void PreviewWidgetVideo::play() {
  if (mediaPlayer.state() == QMediaPlayer::PlayingState) {
    mediaPlayer.pause();
  } else {
    mediaPlayer.play();
  }
}

void PreviewWidgetVideo::mediaStateChanged(QMediaPlayer::State state) {
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

void PreviewWidgetVideo::handleError() {
  ui->playButton->setEnabled(false);
  const QString errorString = mediaPlayer.errorString();
  QString message = "Error: ";
  if (errorString.isEmpty())
    message += " #" + QString::number(int(mediaPlayer.error()));
  else
    message += errorString;
  ui->errorLabel->setText(message);
}

#ifndef PREVIEWWIDGETAUDIO_H
#define PREVIEWWIDGETAUDIO_H

#include <QWidget>
#include <QUrl>
#include <QMediaPlayer>
#include <QStyle>

namespace Ui {
class PreviewWidgetAudio;
}

class PreviewWidgetAudio : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidgetAudio(const QString &file_path, QWidget *parent = nullptr);
    ~PreviewWidgetAudio();

    void setUrl(const QUrl &url);

public slots:
    void play();

private slots:
    void mediaStateChanged(QMediaPlayer::PlaybackState state);
    void positionChanged(int position);
    void durationChanged(int duration);
    void setPosition(int position);
    void handleError();

private:
    Ui::PreviewWidgetAudio *ui;
    QMediaPlayer mediaPlayer;
};
#endif // PREVIEWWIDGETAUDIO_H

#ifndef PREVIEWWIDGETAUDIO_H
#define PREVIEWWIDGETAUDIO_H

#include <QWidget>
#include <QMediaPlayer>

namespace Ui {
class PreviewWidgetAudio;
}

class PreviewWidgetAudio : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidgetAudio(const QString &file_path, QWidget *parent = nullptr);
    ~PreviewWidgetAudio() override;

    void setUrl(const QUrl &url);

public slots:
    void play();

private slots:
    void mediaStateChanged(QMediaPlayer::PlaybackState state);
    void positionChanged(int position);
    void durationChanged(int duration);
    void setPosition(int position);
    void handleError(QMediaPlayer::Error error, const QString &errorString);

private:
    Ui::PreviewWidgetAudio *ui;
    QMediaPlayer mediaPlayer;
};
#endif // PREVIEWWIDGETAUDIO_H

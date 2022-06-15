#ifndef PREVIEWWIDGETVIDEO_H
#define PREVIEWWIDGETVIDEO_H

#include <QWidget>
#include <QMediaPlayer>

namespace Ui {
class PreviewWidgetVideo;
}

class PreviewWidgetVideo : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidgetVideo(const QString &file_path, QWidget *parent = nullptr);
    ~PreviewWidgetVideo() override;

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
    Ui::PreviewWidgetVideo *ui;
    QMediaPlayer mediaPlayer;
};

#endif // PREVIEWWIDGETVIDEO_H

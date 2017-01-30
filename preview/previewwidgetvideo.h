#ifndef PREVIEWWIDGETVIDEO_H
#define PREVIEWWIDGETVIDEO_H

#include <QWidget>
#include <QtWidgets>
#include <qmediaplayer.h>
#include <QtGui/QMovie>
#include <QtWidgets/QWidget>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>

namespace Ui {
class PreviewWidgetVideo;
}

class PreviewWidgetVideo : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidgetVideo(const QString &file_path, QWidget *parent = 0);
    ~PreviewWidgetVideo();

    void setUrl(const QUrl &url);

public slots:
    void play();

private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(int position);
    void durationChanged(int duration);
    void setPosition(int position);
    void handleError();

private:
    Ui::PreviewWidgetVideo *ui;
    QMediaPlayer mediaPlayer;
};

#endif // PREVIEWWIDGETVIDEO_H

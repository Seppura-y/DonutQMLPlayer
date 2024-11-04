#ifndef PLAYLIST_MODEL_H
#define PLAYLIST_MODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QQmlEngine>

class PlaylistModel : public QAbstractListModel
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	Q_PROPERTY(int playingIndex MEMBER playing_index_ NOTIFY playingIndexChanged)
public:
	enum PlaylistRoles
	{
		TitleRole = Qt::UserRole + 1
	};

public:
	static PlaylistModel* getInstance();
	PlaylistModel(QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent) const override;
	QHash<int, QByteArray> roleNames() const override;
	QVariant data(const QModelIndex& index, int role) const override;

public:
	Q_INVOKABLE void addLocalFiles(const QList<QUrl>& file_urls);
	Q_INVOKABLE void removeItem(int index);
	Q_INVOKABLE void clear();
	Q_INVOKABLE void playItem(int index);
	Q_INVOKABLE void playNextItem();
	Q_INVOKABLE int getPlayingIndex();
	Q_INVOKABLE QList<QUrl> getPlayList();
signals:
	void playingIndexChanged();
	void playingSingleDrop(int index);
private:


private:
	static PlaylistModel* s_instance_;

	QStringList titles_;
	QList<QUrl> file_urls_;
	int playing_index_ = -1;
};

#endif
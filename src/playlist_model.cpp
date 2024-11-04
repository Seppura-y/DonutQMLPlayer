#include "playlist_model.h"

#include "donut_qml_av_manager.h"

#include <QFileInfo>

PlaylistModel* PlaylistModel::s_instance_ = nullptr;

PlaylistModel::PlaylistModel(QObject* parent) : QAbstractListModel(parent), playing_index_(-1)
{

}

PlaylistModel* PlaylistModel::getInstance()
{
	if (s_instance_ == nullptr)
	{
		s_instance_ = new PlaylistModel();
	}
	return s_instance_;
}

int PlaylistModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return titles_.count();
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[TitleRole] = QByteArrayLiteral("title");
	return roles;
}

QVariant PlaylistModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	switch (role)
	{
	case TitleRole:
		return titles_[row];
	default:
		return QVariant();
	}
}

void PlaylistModel::addLocalFiles(const QList<QUrl>& file_urls)
{
	int start = titles_.count();
	int count = file_urls.count();

	if(start > 0 || count > 0)
		beginInsertRows(QModelIndex(), start, start + count - 1);
	else
		beginInsertRows(QModelIndex(), start, 0);
	for (const auto& url : file_urls)
	{
		titles_ << QFileInfo(url.toLocalFile()).fileName();
		file_urls_ << url;
	}
	endInsertRows();

	if (count == 1)
	{
		emit playingSingleDrop(start + count - 1);
	}
}

void PlaylistModel::removeItem(int index)
{
	if (index < 0 || index >= titles_.length())
	{
		return;
	}

	beginRemoveRows(QModelIndex(), index, index);
	titles_.removeAt(index);
	file_urls_.removeAt(index);
	endRemoveRows();
}

void PlaylistModel::clear()
{
	if (titles_.count() < 1)
	{
		return;
	}

	beginRemoveRows(QModelIndex(), 0, titles_.count() - 1);
	titles_.clear();
	file_urls_.clear();
	endRemoveRows();
}

void PlaylistModel::playItem(int index)
{
	if (index >= 0 && index < titles_.count())
	{
		int ret = Donut::DonutQMLAVManager::getInstance()->onOpenMediaFile(file_urls_[index].toLocalFile());
		if (ret == 0)
		{
			playing_index_ = index;
			playingIndexChanged();
		}
	}
	
}

void PlaylistModel::playNextItem()
{
	playItem(playing_index_ + 1);
}

int PlaylistModel::getPlayingIndex()
{
	return playing_index_;
}

QList<QUrl> PlaylistModel::getPlayList()
{
	return file_urls_;
}

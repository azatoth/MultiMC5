/* Copyright 2013 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LWJGLVERSIONLIST_H
#define LWJGLVERSIONLIST_H

#include <QObject>
#include <QAbstractListModel>

#include <QUrl>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "libmmc_config.h"

class LIBMULTIMC_EXPORT LWJGLVersion : public QObject
{
	Q_OBJECT
	
	/*!
	 * The name of the LWJGL version.
	 */
	Q_PROPERTY(QString name READ name)
	
	/*!
	 * The URL for this version of LWJGL.
	 */
	Q_PROPERTY(QUrl url READ url)
public:
	LWJGLVersion(const QString &name, const QUrl &url, QObject *parent = 0) :
		QObject(parent), m_name(name), m_url(url) { }
	
	LWJGLVersion(const LWJGLVersion &other) :
		QObject(other.parent()), m_name(other.name()), m_url(other.url()) { }
	
	QString name() const { return m_name; }
	
	QUrl url() const { return m_url; }
	
protected:
	QString m_name;
	QUrl m_url;
};

class LIBMULTIMC_EXPORT LWJGLVersionList : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit LWJGLVersionList(QObject *parent = 0);
	
	static LWJGLVersionList &get();
	
	bool isLoaded() { return m_vlist.length() > 0; }
	
	const LWJGLVersion *getVersion(const QString &versionName);
	LWJGLVersion &at(int index) { return m_vlist[index]; }
	const LWJGLVersion &at(int index) const { return m_vlist[index]; }
	
	int count() const { return m_vlist.length(); }
	
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	virtual int rowCount(const QModelIndex &parent) const { return count(); }
	virtual int columnCount(const QModelIndex &parent) const;
	
	virtual bool isLoading() const;
	virtual bool errored() const { return m_errored; }
	
	virtual QString lastErrorMsg() const { return m_lastErrorMsg; }
	
public slots:
	/*!
	 * Loads the version list.
	 * This is done asynchronously. On success, the loadListFinished() signal will
	 * be emitted. The list model will be reset as well, resulting in the modelReset() 
	 * signal being emitted. Note that the model will be reset before loadListFinished() is emitted.
	 * If loading the list failed, the loadListFailed(QString msg),
	 * signal will be emitted.
	 */
	virtual void loadList();
	
signals:
	/*!
	 * Emitted when the list either starts or finishes loading.
	 * \param loading Whether or not the list is loading.
	 */
	void loadingStateUpdated(bool loading);
	
	void loadListFinished();
	
	void loadListFailed(QString msg);
	
private:
	QList<LWJGLVersion> m_vlist;
	
	QNetworkReply *m_netReply;
	
	QNetworkAccessManager netMgr;
	QNetworkReply *reply;
	
	bool m_loading;
	bool m_errored;
	QString m_lastErrorMsg;
	
	void failed(QString msg);
	
	void finished();
	
	void setLoading(bool loading);
	
private slots:
	virtual void netRequestComplete();
};

#endif // LWJGLVERSIONLIST_H

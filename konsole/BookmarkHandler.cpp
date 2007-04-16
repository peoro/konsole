/*  This file was part of the KDE libraries
    
    Copyright 2002 Carsten Pfeiffer <pfeiffer@kde.org>
    Copyright 2007 Robert Knight <robertknight@gmail.com> 

    library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation, version 2
    or ( at your option ), any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

// Born as kdelibs/kio/kfile/kfilebookmarkhandler.characterpp

// Qt
#include <QFile>

// KDE
#include <kshell.h>

#include <KBookmarkMenu>
#include <KDebug>
#include <KIO/Job>
#include <KIO/NetAccess>
#include <KMenu>
#include <KStandardDirs>

// Konsole
#include "BookmarkHandler.h"
#include "ViewProperties.h"

using namespace Konsole;

BookmarkHandler::BookmarkHandler( KActionCollection* collection, 
                                  KMenu* menu, 
                                  bool toplevel , 
                                  QObject* parent )
    : QObject( parent ),
      KBookmarkOwner(),
      m_toplevel(toplevel),
      m_activeView(0)
{
    setObjectName( "BookmarkHandler" );

    m_menu = menu;

    QString new_bm_file = KStandardDirs::locateLocal( "data", "konsole/bookmarks.xml" );

    m_file = KStandardDirs::locate( "data", "konsole/bookmarks.xml" );
    if ( m_file.isEmpty() )
        m_file = KStandardDirs::locateLocal( "data", "konsole/bookmarks.xml" );

    KBookmarkManager *manager = KBookmarkManager::managerForFile( m_file, "konsole", false);
    
    manager->setUpdate( true );

    if (toplevel) {
        m_bookmarkMenu = new KBookmarkMenu( manager, this, m_menu,
                                            collection );
    } else {
        m_bookmarkMenu = new KBookmarkMenu( manager, this, m_menu,
                                            NULL);
    }
}

BookmarkHandler::~BookmarkHandler()
{
    delete m_bookmarkMenu;
}

void BookmarkHandler::openBookmark( const KBookmark & bm, Qt::MouseButtons, Qt::KeyboardModifiers )
{
    emit openUrl( bm.url() );
}

bool BookmarkHandler::addBookmarkEntry() const
{
    return m_toplevel;
}

bool BookmarkHandler::editBookmarkEntry() const
{
    return m_toplevel;
}

QString BookmarkHandler::currentUrl() const
{
    return urlForView(m_activeView);
}

QString BookmarkHandler::urlForView(ViewProperties* view) const
{
    if ( view )
    {
        return view->url().prettyUrl();
    }
    else
    {
        return QString(); 
    }
}

QString BookmarkHandler::currentTitle() const
{
    return titleForView(m_activeView);
}

QString BookmarkHandler::titleForView(ViewProperties* view) const
{
    const KUrl &u = view ? view->url() : KUrl(); 
    if (u.isLocalFile())
    {
       QString path = u.path();
       path = KShell::tildeExpand(path);
       return path;
    }
    return u.prettyUrl();
}

bool BookmarkHandler::supportsTabs() const
{
    return true;
}

QList<QPair<QString,QString> > BookmarkHandler::currentBookmarkList() const
{
    QList<QPair<QString,QString> > list;

    QListIterator<ViewProperties*> iter( m_views );
    
    while ( iter.hasNext() )
    {
        ViewProperties* next = iter.next();
        list << QPair<QString,QString>(titleForView(next) , urlForView(next));
    }

    return list;
}

void BookmarkHandler::setViews(const QList<ViewProperties*>& views) 
{
    qDebug() << "BookmarkHandler - View list changed.";
    m_views = views;
}
QList<ViewProperties*> BookmarkHandler::views() const
{
    return m_views;
}
void BookmarkHandler::setActiveView( ViewProperties* view )
{
    m_activeView = view;
}
ViewProperties* BookmarkHandler::activeView() const
{
    return m_activeView;
}

#include "BookmarkHandler.moc"

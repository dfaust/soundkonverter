/*
   Copyright (C) 2000 Michael Matz <matz@kde.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <config.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <errno.h>
#include <unistd.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qregexp.h>
//#include <qapp.h>
#include <qstring.h>
// #include <qcursor.h>
//#include <kdebug.h>
#include <ksock.h>
#include <kextsock.h>
#include <klocale.h>
#include <kinputdialog.h>

#include "cddb.h"
#include "cddb.moc"

// FIXME //kdDebug

CDDB::CDDB()
  : ks(0), port(80), remote(false), save_local(false)
{
  QString s = QDir::homeDirPath()+"/.cddb";
  cddb_dirs +=s;
}



CDDB::~CDDB()
{
  deinit();
}



bool
CDDB::set_server(const char *hostname, unsigned short int _port)
{
  if (ks)
    {
      if (h_name == hostname && port == _port)
        return true;
      deinit();
    }
  remote = (hostname != 0) && (*hostname != 0);
  //kdDebug(7101) << "CDDB: set_server, host=" << hostname << "port=" << _port << endl;
  if (remote)
    {
      ks = new KExtendedSocket(hostname, _port);
      if (ks->connect() < 0)
	{
	  //kdDebug(7101) << "CDDB: Can't connect!" << endl;
	  delete ks;
	  ks = 0;
          return false;
	}

      h_name = hostname;
      port = _port;
      QCString r;
      readLine(r); // the server greeting
      writeLine("cddb hello kde-user blubb kio_audiocd 0.4");
      readLine(r);
    }
  return true;
}



bool
CDDB::deinit()
{
  if (ks)
    {
      writeLine("quit");
      QCString r;
      readLine(r);
      ks->close();
    }
  h_name.resize(0);
  port = 0;
  remote = false;
  ks = 0;
  return true;
}



bool
CDDB::readLine(QCString& ret)
{
  int read_length = 0;
  char small_b[128];
  //fd_set set;

  ret.resize(0);
  while (read_length < 40000)
    {
      // Look for a \n in buf
      int ni = buf.find('\n');
      if (ni >= 0)
        {
	  // Nice, so return this substring (without the \n),
	  // and truncate buf accordingly
	  ret = buf.left(ni);
	  if (ret.length() && ret[ret.length()-1] == '\r')
	    ret.resize(ret.length());
	  buf.remove(0, ni+1);
	  //kdDebug(7101) << "CDDB: got  `" << ret << "'" << endl;
	  return true;
	}

      // Try to refill the buffer
      ks->waitForMore(60 * 1000);
      ssize_t l = ks->readBlock(small_b, sizeof(small_b)-1);
      if (l <= 0)
        {
	  // l==0 normally means fd got closed, but we really need a lineend
	  return false;
	}
      small_b[l] = 0;
      read_length += l;
      buf += small_b;
    }
  return false;
}



bool
CDDB::writeLine(const QCString& line)
{
  const char *b = line.data();
  int l = line.length();
  //kdDebug(7101) << "CDDB: send `" << line << "'" << endl;
  while (l)
    {
      ssize_t wl = ks->writeBlock(b, l);
      if (wl < 0 && errno != EINTR)
        return false;
      if (wl < 0)
        wl = 0;
      l -= wl;
      b += wl;
    }
  l = line.length();
  if (l && line.data()[l-1] != '\n')
    {
      char c = '\n';
      ssize_t wl;
      do {
	wl = ks->writeBlock(&c, 1);
      } while (wl <= 0 && errno == EINTR);
      if (wl<=0 && errno != EINTR)
        return false;
    }
  return true;
}



unsigned int
CDDB::get_discid(QValueList<int>& track_ofs)
{
    unsigned int id = 0;
    int num_tracks = track_ofs.count() - 2;

    // the last two track_ofs[] are disc begin and disc end

    for (int i = num_tracks - 1; i >= 0; i--)
      {
	int n = track_ofs[i];
	n /= 75;
	while (n > 0)
	  {
	    id += n % 10;
	    n /= 10;
	  }
      }
    unsigned int l = track_ofs[num_tracks + 1];
    l -= track_ofs[num_tracks];
    l /= 75;
    id = ((id % 255) << 24) | (l << 8) | num_tracks;
    return id;
}



static int
get_code (const QCString &s)
{
  bool ok;
  int code = s.left(3).toInt(&ok);
  if (!ok)
    code = -1;
  return code;
}



static void
parse_query_resp (const QCString& _r, QCString& catg, QCString& d_id, QCString& title)
{
  QCString r = _r.stripWhiteSpace();
  int i = r.find(' ');
  if (i)
    {
      catg = r.left(i).stripWhiteSpace();
      r.remove(0, i+1);
      r = r.stripWhiteSpace();
    }
  i = r.find(' ');
  if (i)
    {
      d_id = r.left(i).stripWhiteSpace();
      r.remove(0, i+1);
      r = r.stripWhiteSpace();
    }
  title = r;
}



QString
CDDB::track(int i) const
{
  if (i < 0 || i >= int(m_names.count()))
    return QString();
  return m_names[i].utf8();
}




QString
CDDB::artist(int i) const
{
  if (i < 0 || i >= int(m_artists.count()))
    return QString();
  return m_artists[i].utf8();
}



bool
CDDB::parse_read_resp(QTextStream *stream, QTextStream *write_stream)
{
  /* Note, that m_names and m_title should be empty */
  QCString end = ".";

  m_disc = 0;
  m_year = 0;
  m_genre = "";

  /* Fill table, so we can index it below.  */
  for (int i = 0; i < m_tracks; i++)
    {
      m_names.append("");
      m_artists.append("");
    }
  while (1)
    {
      QCString r;
      if (stream)
        {
	  if (stream->atEnd())
	    break;
	  r = stream->readLine().latin1();
	}
      else
        {
          if (!readLine(r))
            return false;
        }
      /* Normally the "." is not saved into the local files, but be
	 tolerant about this.  */
      if (r == end)
        break;
      if (write_stream)
        *write_stream << r << endl;
      r = r.stripWhiteSpace();
      if (r.isEmpty() || r[0] == '#')
        continue;
      if (r.left(7) == "DTITLE=")
        {
	  r.remove(0, 7);
	  m_title += QString::fromLocal8Bit(r.stripWhiteSpace());
	}
      else if (r.left(6) == "TTITLE")
        {
	  r.remove(0, 6);
	  int e = r.find('=');
	  if (e)
	    {
	      bool ok;
	      int i = r.left(e).toInt(&ok);
	      if (ok && i >= 0 && i < m_tracks)
	        {
		  r.remove(0, e+1);
		  m_names[i] += QString::fromLocal8Bit(r);
		}
	    }
	}
      else if (r.left(6) == "DYEAR=")
        {
	  r.remove(0, 6);
	  QString year = QString::fromLocal8Bit(r.stripWhiteSpace());
          m_year = year.toInt();
          //kdDebug(7101) << "CDDB: found Year: " << QString().sprintf("%04i",m_year) << endl;
	}
      else if (r.left(7) == "DGENRE=")
        {
	  r.remove(0, 7);
	  m_genre = QString::fromLocal8Bit(r.stripWhiteSpace());
          //kdDebug(7101) << "CDDB: found Genre: " << m_genre << endl;
	}
    }

  /* XXX We should canonicalize the strings ("\n" --> '\n' e.g.) */

  int si = m_title.find(" / ");
  if (si > 0)
    {
      m_artist = m_title.left(si).stripWhiteSpace();
      m_title.remove(0, si+3);
      m_title = m_title.stripWhiteSpace();
    }

  si = m_title.find(" - CD");
  if (si > 0)
    {
      QString disc = m_title.right(m_title.length()-(si+5)).stripWhiteSpace();
      m_disc = disc.toInt();
      //kdDebug(7101) << "CDDB: found Disc: " << disc << endl;
      m_title = m_title.left(si).stripWhiteSpace();
    }

  if (m_title.isEmpty())
    m_title = i18n("No Title");
  /*else
    m_title.replace(QRegExp("/"), "%2f");*/
  if (m_artist.isEmpty())
    m_artist = i18n("Unknown");
  /*else
    m_artist.replace(QRegExp("/"), "%2f");*/

  //kdDebug(7101) << "CDDB: found Title: `" << m_title << "'" << endl;
  for (int i = 0; i < m_tracks; i++)
    {
      if (m_names[i].isEmpty())
        m_names[i] += i18n("Track %1").arg(i);
      //m_names[i].replace(QRegExp("/"), "%2f");
      si = m_names[i].find(" - ");
      if (si < 0)
        {
          si = m_names[i].find(" / ");
        }
      if (si > 0)
        {
          m_artists[i] = m_names[i].left(si).stripWhiteSpace();
          m_names[i].remove(0, si+3);
          m_names[i] = m_names[i].stripWhiteSpace();
        }
      else
        {
          m_artists[i] = m_artist;
        }
      //kdDebug(7101) << "CDDB: found Track " << i+1 << ": `" << m_names[i] << "'" << endl;
    }
  return true;
}



void
CDDB::add_cddb_dirs(const QStringList& list)
{
  QString s = QDir::homeDirPath()+"/.cddb";

  cddb_dirs = list;
  if (cddb_dirs.isEmpty())
    cddb_dirs += s;
}



/* Locates and opens the local file corresponding to that discid.
   Returns TRUE, if file is found and ready for reading.
   Returns FALSE, if file isn't found.  In this case ret_file is initialized
   with a QFile which resides in the first cddb_dir, and has a temp name
   (the ID + getpid()).  You can open it for writing.  */
bool
CDDB::searchLocal(unsigned int id, QFile *ret_file)
{
  QDir dir;
  QString filename;
  filename = QString("%1").arg(id, 0, 16).rightJustify(8, '0');
  QStringList::ConstIterator it;
  for (it = cddb_dirs.begin(); it != cddb_dirs.end(); ++it)
    {
      dir.setPath(*it);
      if (!dir.exists())
        continue;
      /* First look in dir directly.  */
      ret_file->setName (*it + "/" + filename);
      if (ret_file->exists() && ret_file->open(IO_ReadOnly))
        return true;
      /* And then in the subdirs of dir (representing the categories normally).
       */
      const QFileInfoList *subdirs = dir.entryInfoList (QDir::Dirs);
      QFileInfoListIterator fiit(*subdirs);
      QFileInfo *fi;
      while ((fi = fiit.current()) != 0)
        {
	  ret_file->setName (*it + "/" + fi->fileName() + "/" + filename);
	  if (ret_file->exists() && ret_file->open(IO_ReadOnly))
	    return true;
	  ++fiit;
	}
    }
  QString pid;
  pid.setNum(::getpid());
  ret_file->setName (cddb_dirs[0] + "/" + filename + "." + pid);
  /* Try to create the save location.  */
  dir.setPath(cddb_dirs[0]);
  if (save_local && !dir.exists())
    {
      //dir = QDir::current();
      dir.mkdir(cddb_dirs[0]);
    }
  return false;
}



bool
CDDB::queryCD(QValueList<int>& track_ofs)
{
  int num_tracks = track_ofs.count() - 2;
  if (num_tracks < 1)
    return false;
  unsigned int id = get_discid(track_ofs);
  QFile file;
  bool local;

  /* Already read this ID.  */
  if (id == m_discid)
    return true;

  emit cddbMessage(i18n("Searching local cddb entry ..."));
  qApp->processEvents();

  /* First look for a local file.  */
  local = searchLocal (id, &file);
  /* If we have no local file, and no remote connection, barf.  */
  if (!local && (!remote || ks == 0))
    return false;

  m_tracks = num_tracks;
  m_title = "";
  m_artist = "";
  m_names.clear();
  m_discid = id;
  if (local)
    {
      QTextStream stream(&file);
      /* XXX Hmm, what encoding is used by CDDB files?  local? Unicode?
         Nothing?  */
      //stream.setEncoding(QTextStream::Locale);
      parse_read_resp(&stream, 0);
      file.close();
      return true;
    }

  emit cddbMessage(i18n("Searching remote cddb entry ..."));
  qApp->processEvents();

  /* Remote CDDB query.  */
  unsigned int length = track_ofs[num_tracks+1] - track_ofs[num_tracks];
  QCString q;
  q.sprintf("cddb query %08x %d", id, num_tracks);
  QCString num;
  for (int i = 0; i < num_tracks; i++)
    q += " " + num.setNum(track_ofs[i]);
  q += " " + num.setNum(length / 75);
  if (!writeLine(q))
    return false;
  QCString r;
  if (!readLine(r))
    return false;
  r = r.stripWhiteSpace();
  int code = get_code(r);
  if (code == 200)
    {
      QCString catg, d_id, title;
	QDir dir;
	QCString s, pid;

	emit cddbMessage(i18n("Found exact match cddb entry ..."));
	qApp->processEvents();

      /* an exact match */
      r.remove(0, 3);
      parse_query_resp(r, catg, d_id, title);
      //kdDebug(7101) << "CDDB: found exact CD: category=" << catg << " DiscId=" << d_id << " Title=`" << title << "'" << endl;
      q = "cddb read " + catg + " " + d_id;
      if (!writeLine(q))
        return false;
      if (!readLine(r))
        return false;
      r = r.stripWhiteSpace();
      code = get_code(r);
      if (code != 210)
        return false;

	pid.setNum(::getpid());
	s = cddb_dirs[0].latin1();
	//s = s + "/" +catg; // xine seems to not search in local subdirs
	dir.setPath( s );
	if ( !dir.exists() ) dir.mkdir( s );
	s = s+"/"+ d_id;
	file.setName( s );

      if (save_local && file.open(IO_WriteOnly))
        {
	  //kdDebug(7101) << "CDDB: file name to save =" << file.name() << endl;
	  QTextStream stream(&file);
	  if (!parse_read_resp(0, &stream))
	    {
	      file.remove();
	      return false;
	    }
	  file.close();
	  /*QString newname (file.name());
	  newname.truncate(newname.findRev('.'));
	  if (QDir::current().rename(file.name(), newname)) {
	    //kdDebug(7101) << "CDDB: rename failed" << endl;
	    file.remove();
	  }  */
	}
      else if (!parse_read_resp(0, 0))
        return false;
    }
  else if (code == 211)
    {
      // Found some close matches. We'll read the query response and ask the user
      // which one should be fetched from the server.
      QCString end = ".";
      QCString catg, d_id, title;
      QDir dir;
      QCString s, pid, first_match;
      QStringList disc_ids;

      /* some close matches */
      //XXX may be try to find marker based on r
      emit cddbMessage(i18n("Found close cddb entry ..."));
      qApp->processEvents();

      int i=0;
      while (1)
        {
	  if (!readLine(r))
	    return false;
	  r = r.stripWhiteSpace();
	  if (r == end)
	    break;
	  disc_ids.append(r);
	  if (i == 0)
		first_match = r;
	  i++;
	}
	
	bool ok = false;
	
	// We don't want to be thinking too much, do we?
// 	QApplication::restoreOverrideCursor();
	
	// Oh, mylord, which match should I serve you?
	QString _answer = KInputDialog::getItem(i18n("CDDB Matches"), i18n("Several close CDDB entries found. Choose one:"),
					disc_ids, 0, false, &ok );
	QCString answer = _answer.utf8();
	
	if (ok){ // Get user selected match
		parse_query_resp(answer, catg, d_id, title);
	}
	else{ // Get first match
		parse_query_resp(first_match, catg, d_id, title);
	}
	
	// Now we can continue thinking...
// 	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
	
	/*kdDebug(7101) << "CDDB: found close CD: category=" << catg << " DiscId="
 	    << d_id << " Title=`" << title << "'" << endl;*/

	// ... and forth we go as usual
	
	q = "cddb read " + catg + " " + d_id;
        if (!writeLine(q))
          return false;
        if (!readLine(r))
          return false;
        r = r.stripWhiteSpace();
        code = get_code(r);
        if (code != 210)
          return false;

  	pid.setNum(::getpid());
	s = cddb_dirs[0].latin1();
	dir.setPath( s );
	if ( !dir.exists() ) dir.mkdir( s );
	s = s+"/"+ d_id;
	file.setName( s );

        if (save_local && file.open(IO_WriteOnly))
        {
	  //kdDebug(7101) << "CDDB: file name to save =" << file.name() << endl;
	  QTextStream stream(&file);
	  if (!parse_read_resp(0, &stream))
	    {
	      file.remove();
	      return false;
	    }
	  file.close();
	}
        else if (!parse_read_resp(0, 0))
          return false;
	    
    }
  else
    {
      /* 202 - no match found
         403 - Database entry corrupt
	 409 - no handshake */
      //kdDebug(7101) << "CDDB: query returned code " << code << endl;
      return false;
    }

  return true;
}

/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#ifndef _D_MULTI_DISK_ADAPTOR_H_
#define _D_MULTI_DISK_ADAPTOR_H_

#include "DiskAdaptor.h"
#include "Option.h"
#include "DiskWriter.h"
#include "messageDigest.h"

class DiskWriterEntry {
public:
  FileEntryHandle fileEntry;
private:
  DiskWriterHandle diskWriter;
public:
  DiskWriterEntry(const FileEntryHandle& fileEntry):
    fileEntry(fileEntry),
    diskWriter(0) {}

  ~DiskWriterEntry() {}

  string getFilePath(const string& topDir) const
  {
    return topDir+"/"+fileEntry->getPath();
  }

  void initAndOpenFile(const string& topDir)
  {
    diskWriter->initAndOpenFile(getFilePath(topDir), fileEntry->getLength());
  }

  void openFile(const string& topDir)
  {
    diskWriter->openFile(getFilePath(topDir), fileEntry->getLength());
  }

  void openExistingFile(const string& topDir)
  {
    diskWriter->openExistingFile(getFilePath(topDir));
  }

  void closeFile()
  {
    diskWriter->closeFile();
  }

  void setDiskWriter(const DiskWriterHandle& diskWriter) {
    this->diskWriter = diskWriter;
  }

  DiskWriterHandle getDiskWriter() const {
    return diskWriter;
  }
};

typedef SharedHandle<DiskWriterEntry> DiskWriterEntryHandle;

typedef deque<DiskWriterEntryHandle> DiskWriterEntries;

class MultiDiskAdaptor : public DiskAdaptor {
private:
  string topDir;
  uint32_t pieceLength;
  MessageDigestContext ctx;
  DiskWriterEntries diskWriterEntries;
  const Option* option;

  void resetDiskWriterEntries();

  void mkdir() const;

  bool isInRange(const DiskWriterEntryHandle entry, int64_t offset) const;

  uint32_t calculateLength(const DiskWriterEntryHandle entry,
			   int64_t fileOffset,
			   uint32_t rem) const;

  void hashUpdate(const DiskWriterEntryHandle entry,
		  int64_t offset, uint64_t length);

  string getTopDirPath() const;
public:
  MultiDiskAdaptor():pieceLength(0),
		     ctx(DIGEST_ALGO_SHA1),
		     option(0)
  {
    ctx.digestInit();
  }

  virtual ~MultiDiskAdaptor() {}

  virtual void initAndOpenFile();

  virtual void openFile();

  virtual void openExistingFile();

  virtual void closeFile();

  virtual void onDownloadComplete();

  virtual void writeData(const unsigned char* data, uint32_t len,
			 int64_t offset);

  virtual int readData(unsigned char* data, uint32_t len, int64_t offset);

  virtual string sha1Sum(int64_t offset, uint64_t length);

  void setTopDir(const string& topDir) {
    this->topDir = topDir;
  }

  const string& getTopDir() const {
    return topDir;
  }

  void setPieceLength(uint32_t pieceLength) {
    this->pieceLength = pieceLength;
  }

  uint32_t getPieceLength() const {
    return pieceLength;
  }

  void setOption(const Option* option) {
    this->option = option;
  }

  const Option* getOption() const {
    return option;
  }
};

typedef SharedHandle<MultiDiskAdaptor> MultiDiskAdaptorHandle;

#endif // _D_MULTI_DISK_ADAPTOR_H_

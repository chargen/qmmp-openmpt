/*-
 * Copyright (c) 2015 Chris Spiegel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <QIODevice>
#include <QList>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <QtPlugin>

#include <qmmp/fileinfo.h>
#include <qmmp/qmmp.h>

#include "decoderfactory.h"
#include "decoder.h"
#include "metadatamodel.h"
#include "settingsdialog.h"
#include "mptwrap.h"

bool MPTDecoderFactory::supports(const QString &source) const
{
  return MPTWrap::can_play(source.toUtf8().constData());
}

bool MPTDecoderFactory::canDecode(QIODevice *) const
{
  return false;
}

const DecoderProperties MPTDecoderFactory::properties() const
{
  DecoderProperties properties;

  properties.name = tr("OpenMPT Plugin");
  properties.filters << "*.669" << "*.amf" << "*.dbm" << "*.digi" << "*.emod" << "*.far" << "*.fnk"
                     << "*.gdm" << "*.gmc" << "*.imf" << "*.ims" << "*.it" << "*.j2b" << "*.liq"
                     << "*.mdl" << "*.med" << "*.mgt" << "*.mod" << "*.mtm" << "*.ntp" << "*.oct"
                     << "*.okta" << "*.psm" << "*.ptm" << "*.rad" << "*.rtm" << "*.s3m" << "*.stm"
                     << "*.ult" << "*.umx" << "*.xm";
  properties.description = tr("OpenMPT Module Files");
  properties.shortName = "cas-openmpt";
  properties.hasAbout = true;
  properties.hasSettings = true;
  properties.noInput = true;
  properties.protocols << "file";

  return properties;
}

Decoder *MPTDecoderFactory::create(const QString &path, QIODevice *)
{
  return new MPTDecoder(path);
}

#if QMMP_VERSION_MAJOR == 0 && QMMP_VERSION_MINOR == 8
QList<FileInfo *> MPTDecoderFactory::createPlayList(const QString &filename, bool use_metadata)
#elif QMMP_VERSION_MAJOR == 0 && QMMP_VERSION_MINOR == 9
QList<FileInfo *> MPTDecoderFactory::createPlayList(const QString &filename, bool use_metadata, QStringList *)
#endif
{
  QList<FileInfo *> list;

  try
  {
    MPTWrap mpt(filename.toUtf8().constData());
    FileInfo *file_info = new FileInfo(filename);

    file_info->setLength(mpt.duration() / 1000);
    if(settings.get_use_filename())
    {
      file_info->setMetaData(Qmmp::TITLE, filename.section('/', -1));
    }
    else if(use_metadata && !mpt.title().empty())
    {
      file_info->setMetaData(Qmmp::TITLE, QString::fromStdString(mpt.title()));
    }

    list << file_info;
  }
  catch(MPTWrap::InvalidFile)
  {
  }

  return list;
}

MetaDataModel *MPTDecoderFactory::createMetaDataModel(const QString &path, QObject *parent)
{
  return new MPTMetaDataModel(path, parent);
}

void MPTDecoderFactory::showSettings(QWidget *parent)
{
  SettingsDialog *d = new SettingsDialog(parent);
  d->show();
}

void MPTDecoderFactory::showAbout(QWidget *parent)
{
  QString title = tr("About OpenMPT Audio Plugin");
  QString text = tr("OpenMPT Audio Plugin");
  text += "\n";
  text += tr("Written by: Chris Spiegel <cspiegel@gmail.com>");

  QMessageBox::about(parent, title, text);
}

QTranslator *MPTDecoderFactory::createTranslator(QObject *)
{
  return nullptr;
}

Q_EXPORT_PLUGIN2(cas-openmpt, MPTDecoderFactory)

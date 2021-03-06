#include "OLTConstants.h"

#include <QFile>

void OLTConstants::ConstantsBase::fromFileLine(const QByteArray &line)
{
	int pos = line.indexOf('=');
	if( pos != -1 )
		insert(line.left(pos).trimmed(), line.mid(pos+1).trimmed().replace("\\n", "\n"));
}

QString OLTConstants::ErrorStrings::errorString(const QString &text) const
{
	OLTConstants::ErrorStrings::const_iterator i = constBegin();
	while( i != constEnd() )
	{
		int pos = text.indexOf(i.key());
		if( pos != -1 )
			return QString("%1\n%2").arg(i.key(), text.mid(pos), i.value());
		i++;
	}
	// No error.
	return QString();
}

OLTConstants::OLTConstants()
{
	loadOLTConstantsFile();
}

void OLTConstants::loadOLTConstantsFile()
{
	QFile f("OLTConstants.cnf");
	if( !f.open(QIODevice::ReadOnly) )
	{
		f.setFileName(":/files/oltConstants");
		Q_ASSERT_X( f.open(QIODevice::ReadOnly), "loadOLTConstantsFile", QObject::tr("Cannot open %1 to read OLT constant strings").arg(f.fileName()).toLatin1().data() );
//		QFile lf("OLTConstants.cnf");
//		Q_ASSERT_X( lf.open(QIODevice::WriteOnly), "loadOLTConstantsFile", QObject::tr("Cannot open local file %1 to write initial OLT constant strings").arg(lf.fileName()).toLatin1().data() );
//		lf.write(f.readAll());
//		f.seek(0);
	}
	QByteArray linea;

	while( !f.atEnd() )
		if( ((linea = f.readLine()).count() > 2) && !linea.startsWith('#') )
		{
			if( linea.startsWith("COMMAND_ERROR:") )
				m_cmdErrors.fromFileLine( linea.mid(14) );
			else
			if( linea.startsWith("LOGIN_ERROR:") )
				m_loginErrors.fromFileLine( linea.mid(12) );
			else
			if( linea.startsWith("LOGIN_PROMPT:") )
				m_loginPrompt.setPattern( linea.mid(14).trimmed() );
			else
			if( linea.startsWith("COMMAND_PROMPT:") )
				m_cmdPrompt.setPattern( linea.mid(17).trimmed() );
			else
			if( linea.startsWith("CONSTANT:") )
				m_constants.fromFileLine( linea.mid(9) );
			else
			if( linea.startsWith("COMMAND:") )
				m_oltCommands.fromFileLine( linea.mid(8) );
		}
	f.close();
}

QString OLTConstants::parseCommand(QString command, const QStringList data) const
{

	for( int i = 0; i < data.count(); i+=2 )
		command.replace(data[i+0], data[i+1]);
	return command;
}

OLTConstants oltConstants;

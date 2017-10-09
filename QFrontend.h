#ifndef QFRONTEND_H
#define QFRONTEND_H

#include <QMainWindow>

#include "QOLTCommands.h"

namespace Ui
{
	class QFrontend;
}

class QFrontend : public QMainWindow
{
Q_OBJECT
	Ui::QFrontend *ui;
	QOLTCommands huaweiOLT;

	enum CommandIndex
	{
		CmdScroll,
		CmdGetUnmanaged,
		CmdBoardInfo,
		CmdONTInfo,
		CmdONTWANInfo,
		CmdONTMACInfo,
		CmdGPONServiceProfiles
	};
	QList<QWidget*> m_paramWidgets;

public:
	explicit QFrontend(QWidget *parent = 0);
	~QFrontend();

protected:
	void addViewerText(const QString &text);

private slots:
	void oltTelnetIncommings(const char *data, int length);
	void oltTelnetStatusChanged(QAbstractSocket::SocketState state);
	void oltStatusChanged(QTelnetInterface::OLTState state);
	void oltErrorResponse(const QString &tag, const QString &cmd, const QString &err);

	void on_btLogin_clicked();

	void boardInfoReceived(const BoardInfo &boardInfo);
	void unmanagedReceived(const UnmanagedONTs &unmanaged);
	void ontsWANInfoReceived(const ONTWANInfo &ontWANInfo);
	void ontsMACInfoReceived(const ONTMACInfo &ontMACInfo);
	void gponSrvPrfReceived(const GPONServiceProfiles &gponSrvProfiles);

	void on_command_currentIndexChanged(int index);
	void on_sendCMD_clicked();
};

#endif // QFRONTEND_H

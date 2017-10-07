#include "QFrontend.h"
#include "ui_QFrontend.h"

#include <QMessageBox>

#include "QInfoDialog.h"
#include "GlobalConfig.h"

QFrontend::QFrontend(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::QFrontend)
{
	ui->setupUi(this);
	globalConfig.load();
	ui->leFQDN->setText(globalConfig.host());
	ui->leUName->setText(globalConfig.uname());
	ui->leUPass->setText(globalConfig.upass());
	ui->sbPort->setValue(globalConfig.port());

	// Connect all telnet comming data to the main text viewer.
	connect( &huaweiOLT, SIGNAL(newData(const char*,int)), this, SLOT(oltTelnetIncommings(const char*,int)) );
	// TO see telnet status.
	connect( &huaweiOLT, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(oltTelnetStatusChanged(QAbstractSocket::SocketState)) );
	connect( &huaweiOLT, SIGNAL(oltStateChanged(QTelnetInterface::OLTState)), this, SLOT(oltStatusChanged(QTelnetInterface::OLTState)) );
	connect( &huaweiOLT, SIGNAL(errorResponse(QString,QString,QString)), this, SLOT(oltErrorResponse(QString,QString,QString)) );
	connect( &huaweiOLT, SIGNAL(boardInfo(BoardInfo)), this, SLOT(boardInfoReceived(BoardInfo)) );
	connect( &huaweiOLT, SIGNAL(unmanagedOnts(UnmanagedONTs)), this, SLOT(unmanagedReceived(UnmanagedONTs)) );
}

QFrontend::~QFrontend()
{
	huaweiOLT.close();
	globalConfig.setHost(ui->leFQDN->text());
	globalConfig.setUName(ui->leUName->text());
	globalConfig.setUPass(ui->leUPass->text());
	globalConfig.setPort(ui->sbPort->value());
	globalConfig.save();
	delete ui;
	ui = Q_NULLPTR;
}

void QFrontend::addViewerText(const QString &text)
{
	ui->telnetOut->textCursor().insertText(text);
//	ui->telnetOut->appendPlainText( text );
}

void QFrontend::oltTelnetIncommings(const char *data, int length)
{
	addViewerText( QByteArray(data, length) );
}

void QFrontend::oltTelnetStatusChanged(QAbstractSocket::SocketState state)
{
	switch(state)
	{
	case QAbstractSocket::UnconnectedState:
		addViewerText( tr("Desconectado de %1\n").arg(huaweiOLT.peerInfo()) );
		ui->btLogin->setText( tr("Conectar") );
		break;
	case QAbstractSocket::HostLookupState:
		addViewerText( tr("Resolviendo DNS %1\n").arg(huaweiOLT.peerInfo()) );
		ui->btLogin->setText( tr("Cancelar") );
		break;
	case QAbstractSocket::ConnectingState:
		addViewerText( tr("Conectando a %1\n").arg(huaweiOLT.peerInfo()) );
		ui->btLogin->setText( tr("Cancelar") );
		break;
	case QAbstractSocket::ConnectedState:
		addViewerText( tr("Conectado a %1\n").arg(huaweiOLT.peerInfo()) );
		ui->btLogin->setText( tr("Desconectar") );
		break;
	case QAbstractSocket::BoundState:
		addViewerText( tr("Enlazado") );
		ui->btLogin->setText( tr("Cancelar") );
		break;
	case QAbstractSocket::ListeningState:
		addViewerText( tr("Escuchando") );
		ui->btLogin->setText( tr("Cancelar") );
		break;
	case QAbstractSocket::ClosingState:
		addViewerText( tr("Cerrando conexión de %1\n").arg(huaweiOLT.peerInfo()) );
		ui->btLogin->setText( tr("Forzar") );
		break;
	}
}

void QFrontend::oltStatusChanged(QTelnetInterface::OLTState state)
{
	switch( state )
	{
	case QTelnetInterface::OltUnknownSate:
		break;
	case QTelnetInterface::OltUnconnected:
		break;
	case QTelnetInterface::OltConnected:
		break;
	case QTelnetInterface::OltLogging:
		break;
	case QTelnetInterface::OltLogged:
		huaweiOLT.setConfigMode();
		break;
	case QTelnetInterface::OltAdminMode:
		break;
	case QTelnetInterface::OltConfigMode:
		break;
	}
}

void QFrontend::oltErrorResponse(const QString &tag, const QString &cmd, const QString &err)
{
	QMessageBox::warning(this, this->windowTitle(),
						 tr("Error reported from OLT while processing cmd \"%1\" Tag=\"%2\": \n%3").arg(cmd, tag, err));
	addViewerText(tr("Error on command [%1] %2; %3\n").arg(tag, cmd, err));
}

void QFrontend::on_btLogin_clicked()
{
	switch( huaweiOLT.state() )
	{
	case QAbstractSocket::UnconnectedState:
		huaweiOLT.connectToOLT(ui->leFQDN->text(),
							   ui->sbPort->value(),
							   ui->leUName->text(),
							   ui->leUPass->text());
		break;
	case QAbstractSocket::HostLookupState:
		huaweiOLT.disconnectFromHost();
		break;
	case QAbstractSocket::ConnectingState:
		huaweiOLT.disconnectFromHost();
		break;
	case QAbstractSocket::ConnectedState:
		huaweiOLT.disconnectFromHost();
		break;
	case QAbstractSocket::BoundState:
		huaweiOLT.close();
		break;
	case QAbstractSocket::ListeningState:
		huaweiOLT.close();
		break;
	case QAbstractSocket::ClosingState:
		huaweiOLT.close();
		break;
	}
}

void QFrontend::on_btNewOLTs_clicked()
{
	huaweiOLT.getUnmanaged();
}

void QFrontend::on_btScroll_clicked()
{
	huaweiOLT.setScroll(ui->sbScroll->value());
}

void QFrontend::on_btBoardInfo_clicked()
{
	huaweiOLT.getBoardInfo(ui->sbFrame->value(), ui->sbSlot->value());
}

void QFrontend::boardInfoReceived(const BoardInfo &boardInfo)
{
	QInfoDialog( tr("Board Info"), boardInfo.toString() ).exec();
}

void QFrontend::unmanagedReceived(const UnmanagedONTs &unmanaged)
{
	QInfoDialog( tr("Unmanaged ONTs"), unmanaged.toString() ).exec();
}

#include "VNCClient.h"

#include <assert.h>
#include <math.h>
#include <vector>
#include <windows.h>
#include <rfb/LogWriter.h>

#include <rfb_win32/MsgBox.h>



using namespace rfb::unity;
using namespace rfb;
using namespace rfb::win32;
using namespace rdr;

static LogWriter vlog("VNCClient");

TStr rfb::win32::AppName("UNITY VNC Client");

VNCClient::VNCClient()
{
	vlog.debug("VNCClient constructor");

	setConnectionState(Iddle);
	m_ConnectionThread = NULL;
	texture = new UnityTextureHandler();
}

VNCClient::~VNCClient()
{
	vlog.debug("VNCClient destructor");
	if (texture != NULL) delete texture;

	stopConnectionThread();
}


void VNCClient::Connect(const char * host, int port)
{
	vlog.debug("Connect %s:%d", host, port);
	stopConnectionThread();
		
	m_ConnectionThread = new ConnectionThread();
	m_ConnectionThread->Connect(this, host, port);
}

void VNCClient::Update()
{
	if (texture != NULL)
	{
		texture->Update();
	}
}

ConnectionState VNCClient::GetConnectionState()
{
	return m_connectionState;
}

void VNCClient::Disconnect()
{
	stopConnectionThread();
}

int VNCClient::GetWidth()
{
	if (texture == NULL)
	{
		vlog.error("Error no texture");
		return -1;
	}
	return texture->width();
}

int VNCClient::GetHeight()
{
	if (texture == NULL)
	{
		vlog.error("Error no texture");
		return -1;
	}
	return texture->height();
}

void VNCClient::stopConnectionThread()
{
	if (m_ConnectionThread != NULL)
	{
		m_ConnectionThread->QuitAndDelete();
	}
	m_ConnectionThread = NULL;
}

void VNCClient::onTextureBuilt()
{
	if (m_connectionState == BufferSizeChanged)
		setConnectionState(Connected);

}


void VNCClient::setConnectionState(ConnectionState state)
{
	m_connectionState = state;
	switch (state)
	{
	case Iddle:
		vlog.info("setConnectionState Iddle");
		break;
	case Connecting:
		vlog.info("setConnectionState Connecting");
		break;
	case PasswordNeeded:
		vlog.info("setConnectionState PasswordNeeded");
		break;
	case BufferSizeChanged:
		vlog.info("setConnectionState BufferSizeChanged");
		break;
	case Connected:
		vlog.info("setConnectionState Connected");
		break;
	case Error:
		vlog.info("setConnectionState Error");
		stopConnectionThread();
		break;
	default:
		break;
	}
} 
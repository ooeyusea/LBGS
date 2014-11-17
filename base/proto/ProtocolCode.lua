module("ProtocolCode", package.seeall)

GateAndTS = {
	ReportGate = 1,
	CreatePlayer = 2,
	DestroyPlayer = 3,
	PlayerMessage = 4,
	KickOff = 5,
}

ClientAndServer = {
	LoginReq = 1,
	LoginAck = 2,
	Logout = 3,
}

TSAndCS = {
	LoadReq = 1,
	LoadAck = 2,
	CheckAndNotifyOffline = 3,
	KickOff = 4,
}

TSAndLS = {
	AuthenReq = 1,
	AuthenAck = 2,
}

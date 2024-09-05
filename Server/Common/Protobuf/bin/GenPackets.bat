pushd %~dp0

protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=S_ --send=C_
GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=C_ --send=S_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../GameServer"
XCOPY /Y Enum.pb.cc "../../../GameServer"
XCOPY /Y Struct.pb.h "../../../GameServer"
XCOPY /Y Struct.pb.cc "../../../GameServer"
XCOPY /Y Protocol.pb.h "../../../GameServer"
XCOPY /Y Protocol.pb.cc "../../../GameServer"
XCOPY /Y ServerPacketHandler.h "../../../GameServer"

XCOPY /Y Enum.pb.h "../../../DummyClient"
XCOPY /Y Enum.pb.cc "../../../DummyClient"
XCOPY /Y Struct.pb.h "../../../DummyClient"
XCOPY /Y Struct.pb.cc "../../../DummyClient"
XCOPY /Y Protocol.pb.h "../../../DummyClient"
XCOPY /Y Protocol.pb.cc "../../../DummyClient"
XCOPY /Y ClientPacketHandler.h "../../../DummyClient"

XCOPY /Y Enum.pb.h "../../../../AI_MAP/Source/AI_MAP/Network"
XCOPY /Y Enum.pb.cc "../../../../AI_MAP/Source/AI_MAP/Network"
XCOPY /Y Struct.pb.h "../../../../AI_MAP/Source/AI_MAP/Network"
XCOPY /Y Struct.pb.cc "../../../../AI_MAP/Source/AI_MAP/Network"
XCOPY /Y Protocol.pb.h "../../../../AI_MAP/Source/AI_MAP/Network"
XCOPY /Y Protocol.pb.cc "../../../../AI_MAP/Source/AI_MAP/Network"
XCOPY /Y ClientPacketHandler.h "../../../../AI_MAP/Source/AI_MAP/Network"

XCOPY /Y Enum.proto "../../../../AI_MAP/Source/AI_MAP/Network/Proto"
XCOPY /Y Protocol.proto "../../../../AI_MAP/Source/AI_MAP/Network/Proto"
XCOPY /Y Struct.proto "../../../../AI_MAP/Source/AI_MAP/Network/Proto"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE
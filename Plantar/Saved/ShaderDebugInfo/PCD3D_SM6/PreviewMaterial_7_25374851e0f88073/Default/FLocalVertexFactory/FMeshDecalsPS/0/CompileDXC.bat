@ECHO OFF
SET DXC="C:\Users\mrgor\OneDrive\Desktop\UE_5.6\Engine\Binaries\ThirdParty\ShaderConductor\Win64\dxc.exe"
IF NOT EXIST %DXC% (
	ECHO Couldn't find dxc.exe under "C:\Users\mrgor\OneDrive\Desktop\UE_5.6\Engine\Binaries\ThirdParty\ShaderConductor\Win64"
	GOTO :END
)
%DXC% -HV 2018 -Zpr -O3 -auto-binding-space 0 -Wno-parentheses-equality -disable-lifetime-markers -T ps_6_6 -E MainPS -Fc MeshDecals.d3dasm -Fo MeshDecals.dxil MeshDecals.usf
:END
PAUSE

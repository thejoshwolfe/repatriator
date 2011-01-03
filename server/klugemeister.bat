@echo off
echo entering infinite loop
:LoopForever
python %~dp0\server.py
goto LoopForever


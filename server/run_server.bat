@echo off
:LoopForever
python %~dp0\server.py
goto LoopForever


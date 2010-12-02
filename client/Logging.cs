using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace repatriator_client
{
    public static class Logging
    {
        private static LogLevel currentLogLevel = LogLevel.Debug;
        private static StreamWriter logFile;
        private static object _lock = new object();
        static Logging()
        {
            if (currentLogLevel == LogLevel.None)
                return;
            string path = "client.log";
            logFile = new StreamWriter(path, true);
            logFile.Write("\n");
            logSomething(null, "startup");
        }
        public static void error(string message)
        {
            log(message, LogLevel.Error);
        }
        public static void error(Exception ex)
        {
            error(ex.GetType().Name + ": " + ex.Message);
        }
        public static void warning(string message)
        {
            log(message, LogLevel.Warning);
        }
        public static void debug(string message)
        {
            log(message, LogLevel.Debug);
        }
        public static void log(string message, LogLevel logLevel)
        {
            if (currentLogLevel < logLevel)
                return;
            logSomething(getLevelHeader(logLevel), message);
        }
        private static string getLevelHeader(LogLevel logLevel)
        {
            switch (logLevel)
            {
                case LogLevel.Error:
                    return "ERROR";
                case LogLevel.Warning:
                    return "WARNING";
                case LogLevel.Debug:
                    return "DEBUG";
            }
            throw new Exception();
        }
        private static void logSomething(string header, string message)
        {
            lock (_lock)
            {
                logFile.Write(DateTime.Now.ToString("0:MM/dd/yyyy HH:mm:ss "));
                if (header != null)
                    logFile.Write(header + ": ");
                logFile.Write(message);
                logFile.Write("\n");
                logFile.Flush();
            }
        }
        public static string bytesToString(byte[] bytes)
        {
            int jpegStart = bytes.indexOf(new byte[] { 0xff, 0xd8 });
            if (jpegStart != -1)
            {
                // omit the jpeg
                return bytesToString(bytes, 0, jpegStart + 5) + " ... jpeg ... " + bytesToString(bytes, bytes.Length - 5, 5);
            }
            return bytesToString(bytes, 0, bytes.Length);
        }
        private static string bytesToString(byte[] bytes, int start, int length)
        {
            if (length == 0)
                return "";
            StringBuilder stringBuidler = new StringBuilder(length * 3 - 1);
            stringBuidler.Append(bytes[start].ToString("x2"));
            for (int i = start + 1; i < start + length; i++)
                stringBuidler.Append(' ').Append(bytes[i].ToString("x2"));
            return stringBuidler.ToString();
        }
    }
    public enum LogLevel
    {
        None = 0,
        Error = 1,
        Warning = 2,
        Debug = 3,
    }
}

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
            string path = "C:\\aoeu.txt";
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
            DateTime time = DateTime.Now;
            lock (_lock)
            {
                logFile.Write(time.ToString("0:MM/dd/yy H:mm:ss "));
                if (header != null)
                    logFile.Write(header + ": ");
                logFile.Write(message);
                logFile.Write("\n");
                logFile.Flush();
            }
        }
        public static string bytesToString(byte[] bytes)
        {
            StringBuilder stringBuidler = new StringBuilder(bytes.Length * 3);
            for (int i = 0; i < bytes.Length; i++)
                stringBuidler.Append(bytes[i].ToString("x2")).Append(' ');
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

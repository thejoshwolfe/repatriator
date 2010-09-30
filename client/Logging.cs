using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace repatriator_client
{
    public static class Logging
    {
        private const int LEVEL_debug = 3;
        private const int LEVEL_warning = 2;
        private const int LEVEL_error = 1;
        private const int LEVEL_none = 0;
        private static int logLevel = LEVEL_debug;
        private static StreamWriter logFile;
        private static object _lock = new object();
        static Logging()
        {
            if (logLevel == LEVEL_none)
                return;
            string path = "C:\\aoeu.txt";
            logFile = new StreamWriter(path, true);
            logFile.Write("\n");
            logFile.Write("startup: " + DateTime.Now.ToString() + "\n");
        }
        public static void debug(string message)
        {
            if (logLevel < LEVEL_debug)
                return;
            logSomething("DEBUG", message);
        }
        public static void error(string message)
        {
            if (logLevel < LEVEL_error)
                return;
            logSomething("ERROR", message);
        }
        public static void error(Exception ex)
        {
            if (logLevel < LEVEL_error)
                return;
            logSomething("ERROR", ex.GetType().Name + ": " + ex.Message);
        }
        public static void warning(string message)
        {
            if (logLevel < LEVEL_warning)
                return;
            logSomething("WARNING", message);
        }
        private static void logSomething(string header, string message)
        {
            lock (_lock)
            {
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
}

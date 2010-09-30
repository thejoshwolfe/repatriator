using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace repatriator_client
{
    public static class Logging
    {
        private static bool logging = true;
        private static string lastLoggingMode = null;
        private static StreamWriter logFile;
        private static object _lock = new object();
        static Logging()
        {
            if (!logging)
                return;
            string path = "C:\\aoeu.txt";
            logFile = new StreamWriter(path, true);
        }
        public static void logCommunication(string header, byte[] bytes, int index, int length)
        {
            if (!logging)
                return;
            string text = bytesToText(bytes, index, length);
            logSomething(header, text);
        }
        public static void logMessage(string header, string message)
        {
            if (!logging)
                return;
            logSomething(header, message);
        }
        private static void logSomething(string header, string text)
        {
            lock (_lock)
            {
                if (header != lastLoggingMode)
                {
                    logFile.Write("\n" + header + ": ");
                }
                lastLoggingMode = header;
                logFile.Write(text);
                logFile.Flush();
            }
        }
        private static string bytesToText(byte[] bytes, int index, int length)
        {
            StringBuilder stringBuidler = new StringBuilder(bytes.Length * 3);
            for (int i = index; i < index + length; i++)
                stringBuidler.Append(bytes[i].ToString("x2")).Append(' ');
            return stringBuidler.ToString();
        }
    }
}

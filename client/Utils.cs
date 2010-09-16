using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace repatriator_client
{
    public static class Utils
    {
        public static string[] Split(this string source, string delimiter)
        {
            List<string> result = new List<string>();
            int index = 0;
            while (index < source.Length)
            {
                int nextIndex = source.IndexOf(delimiter, index);
                if (nextIndex == -1)
                {
                    result.Add(source.Substring(index));
                    break;
                }
                result.Add(source.Substring(index, nextIndex - index));
                index = nextIndex + delimiter.Length;
            }
            return result.ToArray();
        }
        public static string Repeat(this string s, int count)
        {
            StringBuilder stringBuilder = new StringBuilder(s.Length * count);
            for (int i = 0; i < count; i++)
                stringBuilder.Append(s);
            return stringBuilder.ToString();
        }
        public static bool arrayEquals(byte[] a, byte[] b)
        {
            if (a.Length != b.Length)
                return false;
            for (int i = 0; i < a.Length; i++)
                if (a[i] != b[i])
                    return false;
            return true;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace repatriator_client
{
    class Settings
    {
        public static List<ConnectionSettings> connections = new List<ConnectionSettings>();

        private const string settings_file = "repatriator.xml";

        public static void load()
        {
            System.Xml.XmlTextReader reader = new System.Xml.XmlTextReader(settings_file);

            int state = 0;
            connections.Clear();
            ConnectionSettings connection = new ConnectionSettings();
            try
            {
                while (reader.Read())
                {
                    switch (reader.NodeType)
                    {
                        case XmlNodeType.Element:
                            switch (reader.Name)
                            {
                                case "connection":
                                    state = 1;
                                    break;
                                case "url":
                                    state = 2;
                                    break;
                                case "port":
                                    state = 3;
                                    break;
                                case "user":
                                    state = 4;
                                    break;
                                case "password":
                                    state = 5;
                                    break;
                            }
                            break;
                        case XmlNodeType.Text:
                            switch (state)
                            {
                                case 2:
                                    connection.url = reader.Value.Trim();
                                    break;
                                case 3:
                                    connection.port = 0;
                                    int.TryParse(reader.Value.Trim(), out connection.port);
                                    break;
                                case 4:
                                    connection.username = reader.Value.Trim();
                                    break;
                                case 5:
                                    connection.password = reader.Value.Trim();
                                    break;
                            }
                            break;
                        case XmlNodeType.EndElement:
                            switch (reader.Name)
                            {
                                case "connection":
                                    state = 0;
                                    connections.Add(connection);
                                    connection = new ConnectionSettings();
                                    break;
                                case "url":
                                    state = 1;
                                    break;
                                case "port":
                                    state = 1;
                                    break;
                                case "user":
                                    state = 1;
                                    break;
                                case "password":
                                    state = 1;
                                    break;

                            }
                            break;
                    }
                }
            }
            catch (FileNotFoundException)
            {
                // do nothing
            }
            reader.Close();
        }

        public static void save()
        {
            StreamWriter writer = new StreamWriter(settings_file);
            writer.WriteLine("<connections>");
            foreach (ConnectionSettings connection in connections)
            {
                writer.WriteLine("    <connection>");
                writer.Write("        <url>");
                writer.Write(connection.url);
                writer.WriteLine("</url>");
                writer.Write("        <port>");
                writer.Write(connection.port);
                writer.WriteLine("</port>");
                writer.Write("        <user>");
                writer.Write(connection.username);
                writer.WriteLine("</user>");
                writer.Write("        <password>");
                writer.Write(connection.password);
                writer.WriteLine("</password>");
                writer.WriteLine("    </connection>");
            }
            writer.WriteLine("</connections>");
            writer.Close();
        }
    }
}

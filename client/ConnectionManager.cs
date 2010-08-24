using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Threading;
using System.Net.Sockets;
using System.Text;

namespace repatriator_client
{
    public class ConnectionManager
    {
        private static readonly byte[] magicClientWord = { 0xd1, 0xb6, 0xd7, 0x92, 0x8a, 0xc5, 0x51, 0xa4 };
        private static readonly byte[] magicServerWord = { 0xb5, 0xac, 0x71, 0x2a, 0x08, 0x3d, 0xe5, 0x07 };
        private const byte status_loginSuccess = 0xa3;

        public event Action<ConnectionStatus> connectionUpdate;
        public event Action<LoginStatus> loginFinished;

        private string serverHostName;
        private int serverPortNumber;
        private string userName;

        private Socket socket;
        private Thread establishConnectionThread;
        private Thread receiveThread;

        private ConnectionState connectionState = ConnectionState.Inactive;

        private ConnectionManager()
        {
            createSocket();
        }

        private void createSocket()
        {
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.ReceiveTimeout = 5;
        }

        public bool hasValidSettings()
        {
            if (!(1 <= serverPortNumber && serverPortNumber <= 65535))
                return false;
            if (serverHostName.Length == 0)
                return false;
            if (userName.Length == 0)
                return false;
            return true;
        }
        public void setEverything(string serverHostName, int serverPortNumber, string userName)
        {
            this.serverHostName = serverHostName;
            this.serverPortNumber = serverPortNumber;
            this.userName = userName;
        }

        private void receive_run()
        {
            while (true)
            {
                byte typeCode = readByte();
                int messageLength = readInt();
            }
        }
        private void establishConnection_run()
        {
            while (true)
            {
                try
                {
                    socket.Connect(serverHostName, serverPortNumber);
                }
                catch (SocketException)
                {
                }
                if (!socket.Connected)
                {
                    connectionUpdate(ConnectionStatus.Trouble);
                    continue;
                }
                connectionUpdate(ConnectionStatus.Success);
                // so far so good
                try
                {
                    write(magicClientWord);
                    if (!readMagicWord())
                    {
                        terminateConnection();
                        loginFinished(LoginStatus.ServerIsBogus);
                        return;
                    }
                    // this is a real host
                    writeString(userName);
                    byte status = readByte();
                    if (status != status_loginSuccess)
                    {
                        terminateConnection();
                        loginFinished(LoginStatus.LoginIsInvalid);
                        return;
                    }
                    // login suceeded
                    connectionState = ConnectionState.GoodConnection;
                    loginFinished(LoginStatus.Success);
                    receiveThread = new Thread(receive_run);
                    receiveThread.IsBackground = true;
                    receiveThread.Start();
                    return;
                }
                catch (SocketException)
                {
                    // server isn't communication in time
                    terminateConnection();
                    loginFinished(LoginStatus.ServerIsBogus);
                    return;
                }
            }
        }
        private void terminateConnection()
        {
            socket.Close();
            createSocket();
            connectionState = ConnectionState.Inactive;
        }

        public void start()
        {
            if (!hasValidSettings() || connectionState != ConnectionState.Inactive)
                throw new InvalidOperationException();
            connectionState = ConnectionState.Trying;
            establishConnectionThread = new Thread(establishConnection_run);
            establishConnectionThread.IsBackground = true;
            establishConnectionThread.Start();
        }

        private void write(byte[] bytes)
        {
            socket.Send(bytes);
        }
        private void writeInt(int i)
        {
            write(BitConverter.GetBytes(i));
        }
        private void writeString(string s)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(s);
            writeInt(bytes.Length);
            write(bytes);
        }
        private byte[] read(int length)
        {
            byte[] buffer = new byte[length];
            int offset = 0;
            int remainingSize = length;
            while (0 < remainingSize)
            {
                int readSize = socket.Receive(buffer, offset, remainingSize, SocketFlags.None);
                offset += readSize;
                remainingSize -= readSize;
            }
            return buffer;
        }
        private byte readByte()
        {
            return read(1)[0];
        }
        private int readInt()
        {
            byte[] buffer = read(4);
            return BitConverter.ToInt32(buffer, 0);
        }
        private bool readMagicWord()
        {
            int length = magicServerWord.Length;
            byte[] candidate = read(length);
            return Utils.arrayEquals(candidate, magicServerWord);
        }

        public static ConnectionManager load()
        {
            string serverHostName = "";
            int serverPortNumer = -1;
            string userName = "";
            string settingsPath = Path.Combine(Path.GetDirectoryName(Environment.GetCommandLineArgs()[0]), "settings.txt");

            if (File.Exists(settingsPath))
            {
                // TODO: read the settings file
            }

            ConnectionManager connectionManager = new ConnectionManager();
            connectionManager.setEverything(serverHostName, serverPortNumer, userName);
            return connectionManager;
        }
        private enum ConnectionState
        {
            Inactive, Trying, GoodConnection,
        }
    }
    public enum ConnectionStatus
    {
        Trouble, Success,
    }
    public enum LoginStatus
    {
        ServerIsBogus, LoginIsInvalid, Success,
    }
}

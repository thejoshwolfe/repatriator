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
        private static readonly byte[] magicalRequest = { 0xd1, 0xb6, 0xd7, 0x92, 0x8a, 0xc5, 0x51, 0xa4 };
        private static readonly byte[] magicalResponse = { 0xb5, 0xac, 0x71, 0x2a, 0x08, 0x3d, 0xe5, 0x07 };
        private const byte clientMajorVersion = 0;
        private const byte clientMinorVersion = 0;
        private const byte clientRevisionVersion = 0;
        private const byte serverMajorVersion = 0;
        private const byte serverMinorVersion = 0;
        private const byte serverRevisionVersion = 0;

        public event Action<ConnectionStatus> connectionUpdate;
        public event Action<LoginStatus> loginFinished;

        private string serverHostName;
        private int serverPortNumber;
        private string userName;
        private string password;

        private Socket socket;
        private StreamManager socketStream;
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
            socketStream = new SocketStreamManager(socket);
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
        public void setEverything(string serverHostName, int serverPortNumber, string userName, string password)
        {
            this.serverHostName = serverHostName;
            this.serverPortNumber = serverPortNumber;
            this.userName = userName;
            this.password = password;
        }

        private void receive_run()
        {
            while (true)
            {
                byte typeCode = socketStream.readByte();
                int messageLength = socketStream.readInt();
            }
        }
        private void establishConnection_run()
        {
            loginFinished(establishConnection());
        }
        private LoginStatus establishConnection()
        {
            const int connectionRetryCount = 5;
            for (int i = 0; i < connectionRetryCount; i++)
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
                    socketStream.writeMagicalRequest();
                    if (!socketStream.readMagicalResponse())
                    {
                        terminateConnection();
                        return LoginStatus.ServerIsBogus;
                    }
                    // this is a real host
                    socketStream.writeConnectionRequest(userName, password);
                    ConnectionResult connectionResult = socketStream.readConnectionResult();
                    if (connectionResult.connectionStatus != ServerConnectionStatus.Success)
                    {
                        terminateConnection();
                        switch (connectionResult.connectionStatus)
                        {
                            case ServerConnectionStatus.InvalidLogin:
                                return LoginStatus.LoginIsInvalid;
                            case ServerConnectionStatus.InsufficientPrivileges:
                                return LoginStatus.InsufficientPrivileges;
                            default:
                                throw null;
                        }
                    }
                    // login suceeded
                    connectionState = ConnectionState.GoodConnection;
                    receiveThread = new Thread(receive_run);
                    receiveThread.IsBackground = true;
                    receiveThread.Start();
                    return LoginStatus.Success;
                }
                catch (SocketException)
                {
                    // server isn't communication in time
                    terminateConnection();
                    return LoginStatus.ServerIsBogus;
                }
            }
            return LoginStatus.ConnectionTrouble;
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

        public static ConnectionManager load()
        {
            string serverHostName = "";
            int serverPortNumer = -1;
            string userName = "";
            string password = "";
            string settingsPath = Path.Combine(Path.GetDirectoryName(Environment.GetCommandLineArgs()[0]), "settings.txt");

            if (File.Exists(settingsPath))
            {
                // TODO: read the settings file
            }

            ConnectionManager connectionManager = new ConnectionManager();
            connectionManager.setEverything(serverHostName, serverPortNumer, userName, password);
            return connectionManager;
        }
        private enum ConnectionState
        {
            Inactive, Trying, GoodConnection,
        }
        private static class RequestTypes
        {
            public const byte MagicalRequest = 0;
            public const byte ConnectionRequest = 1;
            public const byte TakePicture = 2;
            public const byte MotorMovement = 3;
            public const byte DirectoryListingRequest = 4;
            public const byte FileDownloadRequest = 5;
            public const byte AddUser = 6;
            public const byte UpdateUser = 7;
            public const byte DeleteUser = 8;
        }
        private static class ResponseTypes
        {
            public const byte MagicalResponse = 0;
            public const byte ConnectionResult = 1;
            public const byte FullUpdate = 2;
            public const byte DirectoryListingResult = 3;
            public const byte FileDownloadResult = 4;
        }
        private class ConnectionResult
        {
            public byte serverMajorVersion;
            public byte serverMinorVersion;
            public byte serverRevisionVersion;
            public ServerConnectionStatus connectionStatus;
            public HashSet<Permission> permissions = new HashSet<Permission>();
        }
        private enum ServerConnectionStatus
        {
            InvalidLogin = 0,
            InsufficientPrivileges = 1,
            Success = 2,
        }
        private enum Permission
        {
            Hardware = 0,
            Admin = 1,
        }
        private abstract class StreamManager
        {
            public void writeByte(byte value)
            {
                write(new byte[] { value });
            }
            public void writeInt(int i)
            {
                write(BitConverter.GetBytes(i));
            }
            public void writeLong(long l)
            {
                write(BitConverter.GetBytes(l));
            }
            public void writeString(string s)
            {
                byte[] bytes = Encoding.UTF8.GetBytes(s);
                writeInt(bytes.Length);
                write(bytes);
            }
            public void writeMagicalRequest()
            {
                writeByte(RequestTypes.MagicalRequest);
                writeLong(magicalRequest.Length + 9);
                write(magicalRequest);
            }
            public void writeConnectionRequest(string userName, string password)
            {
                FakeStreamWriter buffer = new FakeStreamWriter();
                buffer.writeByte(clientMajorVersion);
                buffer.writeByte(clientMinorVersion);
                buffer.writeByte(clientRevisionVersion);
                buffer.writeByte(0);
                buffer.writeString(userName);
                buffer.writeString(password);
                byte[] bytes = buffer.toByteArray();

                writeByte(RequestTypes.ConnectionRequest);
                writeLong(bytes.Length + 9);
                write(bytes);
            }

            public abstract void write(byte[] bytes);
            public abstract byte[] read(int length);
            public byte readByte()
            {
                return read(1)[0];
            }
            public int readInt()
            {
                byte[] buffer = read(4);
                return BitConverter.ToInt32(buffer, 0);
            }
            public long readLong()
            {
                byte[] buffer = read(8);
                return BitConverter.ToInt64(buffer, 0);
            }
            public bool readMagicalResponse()
            {
                byte typeCode = readByte();
                if (typeCode != ResponseTypes.MagicalResponse)
                    return false;
                long length = readLong();
                if (length != 17)
                    return false;
                byte[] candidate = read(magicalResponse.Length);
                return Utils.arrayEquals(candidate, magicalResponse);
            }
            public ConnectionResult readConnectionResult()
            {
                RawResponse response = readRawResponse();
                if (response.typeCode != ResponseTypes.ConnectionResult)
                    throw null;
                FakeStreamReader reader = response.getReader();
                ConnectionResult result = new ConnectionResult();
                result.serverMajorVersion = reader.readByte();
                result.serverMinorVersion = reader.readByte();
                result.serverRevisionVersion = reader.readByte();
                reader.readByte();
                result.connectionStatus = (ServerConnectionStatus)Enum.ToObject(typeof(ServerConnectionStatus), reader.readInt());
                int permissionsCount = reader.readInt();
                for (int i = 0; i < permissionsCount; i++)
                    result.permissions.Add((Permission)Enum.ToObject(typeof(Permission), reader.readInt()));
                return result;
            }
            private RawResponse readRawResponse()
            {
                byte typeCode = readByte();
                long length = readLong();
                byte[] buffer = read((int)length);
                return new RawResponse(typeCode, buffer);
            }
        }
        private class SocketStreamManager : StreamManager
        {
            private Socket socket;
            public SocketStreamManager(Socket socket)
            {
                this.socket = socket;
            }
            public override void write(byte[] bytes)
            {
                int index = 0;
                int amountToWrite = bytes.Length;
                while (0 < amountToWrite)
                {
                    int amountWritten = socket.Send(bytes, index, amountToWrite, 0);
                    if (amountWritten == 0)
                        throw new SocketException();
                    index += amountWritten;
                    amountToWrite -= amountWritten;
                }
            }
            public override byte[] read(int length)
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
        }
        private class FakeStreamWriter : StreamManager
        {
            private MemoryStream memoryStream = new MemoryStream();
            public override void write(byte[] bytes)
            {
                memoryStream.Write(bytes, 0, bytes.Length);
            }
            public override byte[] read(int length)
            {
                throw new NotImplementedException();
            }
            public byte[] toByteArray()
            {
                return memoryStream.ToArray();
            }
        }
        private class FakeStreamReader : StreamManager
        {
            private readonly byte[] buffer;
            private int index = 0;
            public FakeStreamReader(byte[] buffer)
            {
                this.buffer = buffer;
            }
            public override byte[] read(int length)
            {
                byte[] miniBuffer = new byte[length];
                Array.Copy(buffer, index, miniBuffer, 0, length);
                index += length;
                return miniBuffer;
            }
            public override void write(byte[] bytes)
            {
                throw new NotImplementedException();
            }
        }
        private class RawResponse
        {
            public readonly byte typeCode;
            public readonly byte[] buffer;
            public RawResponse(byte typeCode, byte[] buffer)
            {
                this.typeCode = typeCode;
                this.buffer = buffer;
            }
            public FakeStreamReader getReader()
            {
                return new FakeStreamReader(buffer);
            }
        }
    }
    public enum ConnectionStatus
    {
        Trouble, Success,
    }
    public enum LoginStatus
    {
        ConnectionTrouble, ServerIsBogus, LoginIsInvalid, InsufficientPrivileges, Success,
    }
}

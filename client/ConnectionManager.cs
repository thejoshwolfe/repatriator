using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Threading;
using System.Net.Sockets;
using System.Text;
using System.Drawing;

namespace repatriator_client
{
    public class ConnectionManager
    {
        private static readonly byte[] magicalRequest = { 0xd1, 0xb6, 0xd7, 0x92, 0x8a, 0xc5, 0x51, 0xa4 };
        private static readonly byte[] magicalResponse = { 0xb5, 0xac, 0x71, 0x2a, 0x08, 0x3d, 0xe5, 0x07 };
        private const int clientMajorVersion = 0;
        private const int clientMinorVersion = 0;
        private const int clientBuildVersion = 0;
        private const int serverMajorVersion = 0;
        private const int serverMinorVersion = 0;
        private const int serverBuildVersion = 0;

        public event Action<ConnectionStatus> connectionUpdate;
        public event Action<LoginStatus> loginFinished;
        public event Action fullUpdated;
        public event Action directoryListUpdated;

        private string serverHostName;
        private int serverPortNumber;
        private string userName;
        private string password;
        private string downloadDirectory;

        private Socket socket;
        private StreamManager socketStream;
        private Thread establishConnectionThread;
        private Thread receiveThread;

        private ConnectionState connectionState = ConnectionState.Inactive;

        private long a, b, x, y, z;
        private Image image;
        private DirectoryItem[] directoryList = { };

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
            if (!Directory.Exists(downloadDirectory))
                return false;
            return true;
        }
        public void setEverything(string serverHostName, int serverPortNumber, string userName, string password, string downloadDirectory)
        {
            this.serverHostName = serverHostName;
            this.serverPortNumber = serverPortNumber;
            this.userName = userName;
            this.password = password;
            this.downloadDirectory = downloadDirectory;
        }

        private void receive_run()
        {
            while (true)
            {
                readAndDispatchEvent();
            }
        }
        private void readAndDispatchEvent()
        {
            EventResponse eventResponse = socketStream.readEventResponse();
            if (eventResponse is FullUpdateEventResponse)
                fullUpdate((FullUpdateEventResponse)eventResponse);
            else if (eventResponse is DirectoryListingEventResponse)
                updateDirectoryList((DirectoryListingEventResponse)eventResponse);
            else if (eventResponse is FileDownloadEventResponse)
                handleFileDownloaded((FileDownloadEventResponse)eventResponse);
            else
                throw null;
        }

        private void fullUpdate(FullUpdateEventResponse response)
        {
            a = response.a;
            b = response.b;
            x = response.x;
            y = response.y;
            z = response.z;
            image = response.image;
            fullUpdated();
        }
        private void updateDirectoryList(DirectoryListingEventResponse directoryListingEventResponse)
        {
            directoryList = directoryListingEventResponse.directoryList;
            directoryListUpdated();
        }
        private void handleFileDownloaded(FileDownloadEventResponse fileDownloadEventResponse)
        {
            string filename = getNextDownloadFilename();
            fileDownloadEventResponse.image.Save(filename);
        }
        private int nextDownloadNumber = 0;
        private string getNextDownloadFilename()
        {
            HashSet<string> existingNames = new HashSet<string>(Directory.GetFiles(downloadDirectory));
            while (true)
            {
                string filename = downloadDirectory + "/img_" + nextDownloadNumber.ToString("0000");
                if (!File.Exists(filename))
                    return filename;
                nextDownloadNumber += 1;
            }
        }
        private void establishConnection_run()
        {
            LoginStatus result = establishConnection();
            if (result != LoginStatus.Success)
                terminateConnection();
            loginFinished(result);
        }
        private LoginStatus establishConnection()
        {
            const int connectionRetryCount = 5;
            for (int i = 0; i < connectionRetryCount; i++)
            {
                try { socket.Connect(serverHostName, serverPortNumber); }
                catch (SocketException) { }
                if (!socket.Connected)
                {
                    if (connectionState == ConnectionState.Cancelling)
                        return LoginStatus.Cancelled;
                    connectionUpdate(ConnectionStatus.Trouble);
                    continue;
                }
                connectionUpdate(ConnectionStatus.Success);
                // so far so good
                try
                {
                    socketStream.writeMagicalRequest();
                    if (!socketStream.readMagicalResponse())
                        return LoginStatus.ServerIsBogus;
                    // this is a real host
                    socketStream.writeConnectionRequest(userName, password);
                    ConnectionResult connectionResult = socketStream.readConnectionResult();
                    if (connectionResult.connectionStatus != ServerConnectionStatus.Success)
                    {
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
                    if (connectionState == ConnectionState.Cancelling)
                        return LoginStatus.Cancelled;
                    // server isn't communicating in time
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
        public void cancel()
        {
            // TODO thread-safe this
            if (connectionState != ConnectionState.Trying)
                return;
            connectionState = ConnectionState.Cancelling;
            socket.Close();
        }

        public static ConnectionManager load()
        {
            string serverHostName = "";
            int serverPortNumer = -1;
            string userName = "";
            string password = "";
            string settingsPath = Path.Combine(Path.GetDirectoryName(Environment.GetCommandLineArgs()[0]), "settings.txt");
            string downloadDirectory = "";

            if (File.Exists(settingsPath))
            {
                // TODO: read the settings file
            }

            ConnectionManager connectionManager = new ConnectionManager();
            connectionManager.setEverything(serverHostName, serverPortNumer, userName, password, downloadDirectory);
            return connectionManager;
        }
        private enum ConnectionState
        {
            Inactive, Trying, Cancelling, GoodConnection,
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
            public int serverMajorVersion;
            public int serverMinorVersion;
            public int serverBuildVersion;
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
            OperateHardware = 0,
            ManageUsers = 1,
        }
        private abstract class StreamManager
        {
            public void writeByte(byte value)
            {
                write(new byte[] { value });
            }
            public void writeInt(int value)
            {
                byte[] bytes = {
                    (byte)((value >> 24) & 0xff),
                    (byte)((value >> 16) & 0xff),
                    (byte)((value >> 8) & 0xff),
                    (byte)((value >> 0) & 0xff),
                };
                write(bytes);
            }
            public void writeLong(long value)
            {
                byte[] bytes = {
                    (byte)((value >> 56) & 0xff),
                    (byte)((value >> 48) & 0xff),
                    (byte)((value >> 40) & 0xff),
                    (byte)((value >> 32) & 0xff),
                    (byte)((value >> 24) & 0xff),
                    (byte)((value >> 16) & 0xff),
                    (byte)((value >> 8) & 0xff),
                    (byte)((value >> 0) & 0xff),
                };
                write(bytes);
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
                writeLong(1 + 8 + magicalRequest.Length);
                write(magicalRequest);
            }
            public void writeConnectionRequest(string userName, string password)
            {
                FakeStreamWriter buffer = new FakeStreamWriter();
                buffer.writeInt(clientMajorVersion);
                buffer.writeInt(clientMinorVersion);
                buffer.writeInt(clientBuildVersion);
                buffer.writeString(userName);
                buffer.writeString(password);
                byte[] bytes = buffer.toByteArray();

                writeByte(RequestTypes.ConnectionRequest);
                writeLong(1 + 8 + bytes.Length);
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
                byte[] bytes = read(4);
                int value = 0;
                value += (bytes[0] & 0xff) << 24;
                value += (bytes[1] & 0xff) << 16;
                value += (bytes[2] & 0xff) << 8;
                value += (bytes[3] & 0xff) << 0;
                return value;
            }
            public long readLong()
            {
                byte[] bytes = read(8);
                long value = 0;
                value += (long)(bytes[0] & 0xff) << 56;
                value += (long)(bytes[1] & 0xff) << 48;
                value += (long)(bytes[2] & 0xff) << 40;
                value += (long)(bytes[3] & 0xff) << 32;
                value += (long)(bytes[4] & 0xff) << 24;
                value += (long)(bytes[5] & 0xff) << 16;
                value += (long)(bytes[6] & 0xff) << 8;
                value += (long)(bytes[7] & 0xff) << 0;
                return value;
            }
            public string readString()
            {
                int length = readInt();
                byte[] bytes = read(length);
                return Encoding.UTF8.GetString(bytes);
            }
            public Image readImage()
            {
                long imageLength = readLong();
                return Image.FromStream(new MemoryStream(read((int)imageLength), false));
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
                result.serverMajorVersion = reader.readInt();
                result.serverMinorVersion = reader.readInt();
                result.serverBuildVersion = reader.readInt();
                result.connectionStatus = (ServerConnectionStatus)Enum.ToObject(typeof(ServerConnectionStatus), reader.readInt());
                int permissionsCount = reader.readInt();
                for (int i = 0; i < permissionsCount; i++)
                    result.permissions.Add((Permission)Enum.ToObject(typeof(Permission), reader.readInt()));
                return result;
            }
            public EventResponse readEventResponse()
            {
                RawResponse rawResponse = readRawResponse();
                FakeStreamReader reader = rawResponse.getReader();
                switch (rawResponse.typeCode)
                {
                    case ResponseTypes.FullUpdate:
                        {
                            FullUpdateEventResponse response = new FullUpdateEventResponse();
                            response.a = reader.readLong();
                            response.b = reader.readLong();
                            response.x = reader.readLong();
                            response.y = reader.readLong();
                            response.z = reader.readLong();
                            response.image = reader.readImage();
                            return response;
                        }
                    case ResponseTypes.DirectoryListingResult:
                        {
                            DirectoryListingEventResponse response = new DirectoryListingEventResponse();
                            response.directoryList = new DirectoryItem[reader.readInt()];
                            for (int i = 0; i < response.directoryList.Length; i++)
                            {
                                response.directoryList[i] = new DirectoryItem();
                                response.directoryList[i].filename = reader.readString();
                                response.directoryList[i].thumbNail = reader.readImage();
                            }
                            return response;
                        }
                    case ResponseTypes.FileDownloadResult:
                        {
                            FileDownloadEventResponse response = new FileDownloadEventResponse();
                            response.image = reader.readImage();
                            return response;
                        }
                    default:
                        throw null;
                }
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
                Logging.logMessage("\ninit", "instance = " + this.GetHashCode() + ". ");
            }
            public override void write(byte[] bytes)
            {
                int index = 0;
                int amountToWrite = bytes.Length;
                while (0 < amountToWrite)
                {
                    int amountWritten = socket.Send(bytes, index, amountToWrite, 0);
                    if (amountWritten == 0)
                    {
                        Logging.logMessage("error", "can't send anything. ");
                        throw new SocketException();
                    }
                    Logging.logCommunication("write", bytes, index, amountWritten);
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
                    // "read" as in "red" not "reed". English is... well at least America has lots of nukes.
                    int readSize = socket.Receive(buffer, offset, remainingSize, SocketFlags.None);
                    if (readSize == 0)
                    {
                        Logging.logMessage("error", "can't read anything. ");
                        throw new SocketException();
                    }
                    Logging.logCommunication("read", buffer, offset, readSize);
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
        ConnectionTrouble, ServerIsBogus, LoginIsInvalid, InsufficientPrivileges, Cancelled, Success,
    }
    public class EventResponse
    {
    }
    public class FullUpdateEventResponse : EventResponse
    {
        public long a, b, x, y, z;
        public Image image;
    }
    public class DirectoryListingEventResponse : EventResponse
    {
        public DirectoryItem[] directoryList;
    }
    public class FileDownloadEventResponse : EventResponse
    {
        public Image image;
    }
    public class DirectoryItem
    {
        public string filename;
        public Image thumbNail;
    }
}

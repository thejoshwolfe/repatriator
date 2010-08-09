using System;
using System.Drawing;
using System.Net.Sockets;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace repatriator_client
{
    public partial class Main : Form
    {
        private Socket socket;
        private Thread receiveThread;


        public Main()
        {
            InitializeComponent();
            receiveThread = new Thread(receive_run);
            receiveThread.IsBackground = true;
        }

        private void receive_run()
        {
            while (socket != null)
            {
                try
                {
                    byte[] message_buffer = receiveMessage(socket);
                    MessageToClient message = MessageToClient.decode(message_buffer);
                    switch (message.messageType)
                    {
                        case MessageToClientType.fullUpdate:
                            FullUpdateMessage fullUpdateMessage = ((FullUpdateMessage)message);
                            BeginInvoke(new Action(delegate()
                            {
                                Image oldImage = pictureBox.Image;
                                pictureBox.Image = fullUpdateMessage.image;
                                if (oldImage != null)
                                    oldImage.Dispose();
                            }));
                            break;
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex);
                }
            }
        }
        private void connectButton_Click(object sender, EventArgs e)
        {
            if (socket == null)
            {
                // connect
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                try
                {
                    socket.Connect(hostNameText.Text, int.Parse(hostPortText.Text));
                }
                catch (SocketException)
                {
                }
                if (!socket.Connected)
                {
                    socket = null;
                    return;
                }
                receiveThread.Start();
            }
            else
            {
                // disconnect
                socket.Close();
                socket = null;
            }
            updateConnectionStatus();
        }
        private void updateConnectionStatus()
        {
            bool connected = socket != null;
            hostNameText.Enabled = !connected;
            connectButton.Text = connected ? "Disconnect" : "Connect";
            takePictureButton.Enabled = connected;
            pictureBox.Enabled = connected;
        }
        private void takePictureButton_Click(object sender, EventArgs e)
        {
            byte[] message_buffer = { (byte)MessageToServerType.takePicture };
            sendMessage(socket, message_buffer);
        }

        private void sendMessage(Socket socket, byte[] message_buffer)
        {
            byte[] size_buffer = BitConverter.GetBytes(message_buffer.Length);
            socket.Send(size_buffer);
            socket.Send(message_buffer);
        }

        private static byte[] receiveMessage(Socket socket)
        {
            byte[] size_buffer = BitConverter.GetBytes(0);
            socket.Receive(size_buffer);
            int size = BitConverter.ToInt32(size_buffer, 0);
            byte[] message_buffer = new byte[size];
            socket.Receive(message_buffer);
            return message_buffer;
        }
    }

    public abstract class MessageToClient
    {
        public abstract MessageToClientType messageType { get; }
        public static MessageToClient decode(byte[] message_buffer)
        {
            switch ((MessageToClientType)message_buffer[0])
            {
                case MessageToClientType.fullUpdate:
                    Image image = Image.FromStream(new MemoryStream(message_buffer, 1, message_buffer.Length - 1));
                    return new FullUpdateMessage(image);
            }
            throw null;
        }
    }
    public class FullUpdateMessage : MessageToClient
    {
        public override MessageToClientType messageType { get { return MessageToClientType.fullUpdate; } }

        public readonly Image image;
        public FullUpdateMessage(Image image)
        {
            this.image = image;
        }
    }

    public enum MessageToServerType : byte
    {
        takePicture = 0,
    }
    public enum MessageToClientType : byte
    {
        fullUpdate = 0,
    }
}

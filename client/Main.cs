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
                    Message message = Message.decode(message_buffer);
                    switch (message.messageType)
                    {
                        case MessageTypeToClient.fullUpdate:
                            Image image = ((FullUpdateMessage)message).image;
                            BeginInvoke(new Action(delegate()
                            {
                                Image oldImage = pictureBox.Image;
                                pictureBox.Image = image;
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
            pictureBox.Enabled = connected;
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

    public abstract class Message
    {
        public abstract MessageTypeToClient messageType { get; }
        public static Message decode(byte[] message_buffer)
        {
            switch ((MessageTypeToClient)message_buffer[0])
            {
                case MessageTypeToClient.fullUpdate:
                    Image image = Image.FromStream(new MemoryStream(message_buffer, 1, message_buffer.Length - 1));
                    return new FullUpdateMessage(image);
            }
            throw null;
        }
    }
    public class FullUpdateMessage : Message
    {
        public override MessageTypeToClient messageType { get { return MessageTypeToClient.fullUpdate; } }

        public readonly Image image;
        public FullUpdateMessage(Image image)
        {
            this.image = image;
        }
    }

    public enum MessageTypeToServer : byte
    {
        takePicture = 0,
    }
    public enum MessageTypeToClient : byte
    {
        fullUpdate = 0,
    }
}

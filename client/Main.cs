using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Net.Sockets;
using System.Linq;
using System.Text;
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
                    byte[] size_buffer = BitConverter.GetBytes(0);
                    socket.Receive(size_buffer);
                    int size = BitConverter.ToInt32(size_buffer, 0);
                    byte[] message_buffer = new byte[size];
                    socket.Receive(message_buffer);
                    Image image = Image.FromStream(new MemoryStream(message_buffer));
                    this.BeginInvoke(new Action(delegate()
                    {
                        Image oldImage = pictureBox.Image;
                        pictureBox.Image = image;
                        if (oldImage != null)
                            oldImage.Dispose();
                    }));
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
            chatInputText.Enabled = connected;
            pictureBox.Enabled = connected;
        }

        private void chatInputText_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == '\r')
            {
                string sendMessage = chatInputText.Text;
                chatInputText.Text = "";

                byte[] message_bytes = Encoding.UTF8.GetBytes(sendMessage);
                int length = message_bytes.Length;
                byte[] length_bytes = BitConverter.GetBytes(length);
                socket.Send(length_bytes);
                socket.Send(message_bytes);
            }
        }
    }
}
